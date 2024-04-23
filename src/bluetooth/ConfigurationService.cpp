#include "bluetooth/ConfigurationService.hpp"
#include <BLE2902.h>
#include <debug.h>
#include <SensorManager.hpp>
#include <RTCSingleton.hpp>
#include <AlarmManager.hpp>

uint8_t cmd = 0;
uint8_t arg = 0;
SensorManager *sensorManager2 = nullptr;
AlarmManager *alarmManager1 = nullptr;
uint8_t response[20] = {};

static inline void setRecurrenceBytes(uint32_t recurrence);
static inline uint32_t getRecurrenceBytes(const uint8_t *data);
static inline void setAlarmComand(uint8_t alarmIndex, const std::string &value);
static inline void setUnixTimeBytes(uint64_t unixTime);
static inline uint64_t getUnixTimeBytes(const uint8_t *data);

class ConfigCharacteristicCallback : public BLECharacteristicCallbacks
{
    void onWrite(BLECharacteristic *pCharacteristic);
    void onRead(BLECharacteristic *pCharacteristic);
};

void ConfigCharacteristicCallback::onWrite(BLECharacteristic *pCharacteristic)
{
    std::string value = pCharacteristic->getValue();
    TIMESTAMP();
    DEBUG("Received config data\n");

    if (value.length() < 1)
    {
        // Invalid command
        DEBUG("Empty command\n");
        return;
    }

    for (int i = 0; i < value.length(); i++)
    {
        DEBUG("B%d   ", i);
    }
    DEBUG("\n");
    for (int i = 0; i < value.length(); i++)
    {
        DEBUG("0x%x ", value[i]);
    }
    DEBUG(", length %d\n", value.length());
    cmd = value[0];                             // Save the command byte
    arg = (value.length() >= 2) ? value[1] : 0; // Default argument to 0 if not provided

    // Check the command type
    switch (cmd)
    {
    case 0xF1: // Command for accessing recurrence configuration
        if (value.length() >= 7)
        {
            uint32_t recurrenceValue = getRecurrenceBytes(reinterpret_cast<const uint8_t *>(value.data()));
            DEBUG("New REcurrence %d for p %d\n", recurrenceValue, arg);
            sensorManager2->setRecurrenceWithIndex(static_cast<ParameterType>(arg), recurrenceValue);
        }
        else if (value.length() >= 2)
        {
            DEBUG("Reading recurrence\n");
            uint32_t recurrence = sensorManager2->getRecurrenceWithIndex(static_cast<ParameterType>(arg));
            response[0] = cmd;
            response[1] = arg;
            setRecurrenceBytes(recurrence);
            pCharacteristic->setValue(response, 7);
            pCharacteristic->notify();
        }
        break;

    case 0xF2: // Command for resetting to default recurrence
        if (arg != 0)
        {
            sensorManager2->setRecurrenceWithIndex(static_cast<ParameterType>(arg), DEFAULT_RECURRENCE);
            response[0] = cmd;
            response[1] = arg;
            setRecurrenceBytes(DEFAULT_RECURRENCE);
            pCharacteristic->setValue(response, 7);
            pCharacteristic->notify();
        }
        break;

    case 0xA0: // Command to set alarm parameter 0
    case 0xA1: // Command to set alarm parameter 1
    case 0xA2: // Command to set alarm parameter 2
        // Implement the logic to handle setting alarms
        setAlarmComand(cmd - 0xA0, value);
        break;

    case 0xB0: // Command to disable alarm 0
    case 0xB1: // Command to disable alarm 1
    case 0xB2: // Command to disable alarm 2
        alarmManager1->disableAlarm(cmd - 0xB0);

    case 0xD0: // Command to delete alarm 0
    case 0xD1: // Command to delete alarm 1
    case 0xD2: // Command to delete alarm 2
        alarmManager1->deleteAlarm(cmd - 0xD0);
        break;

    case 0xE0: // Command to enable alarm 0
    case 0xE1: // Command to enable alarm 1
    case 0xE2: // Command to enable alarm 2
        alarmManager1->enableAlarm(cmd - 0xE0);
        break;

    case 0xEE: // Command for time configuration
        if (value.length() >= 11)
        {
            // Extract UNIXTIME from bytes 3 to 10
            uint64_t unixTime = getUnixTimeBytes(reinterpret_cast<const uint8_t *>(value.data()));
            RTCSingleton::rtc.setTime(unixTime);
            response[0] = cmd;
            response[1] = 0;
            setUnixTimeBytes(unixTime);
            pCharacteristic->setValue(response, 11);
            pCharacteristic->notify();
        }
        else
        {
            response[0] = cmd;
            response[1] = 0;
            setUnixTimeBytes(RTCSingleton::rtc.getEpoch());
            pCharacteristic->setValue(response, 11);
            pCharacteristic->notify();
        }
        break;

    default:
        cmd = 0;
        arg = 0;
        break;
    }
}

void ConfigCharacteristicCallback::onRead(BLECharacteristic *pCharacteristic)
{
    switch (cmd)
    {
    case 0xF1: // Command for accessing recurrence configuration
        DEBUG("0xF1 to read recurrence\n");
        pCharacteristic->setValue(response, 7);
        pCharacteristic->notify();
        break;

    case 0xF2: // Command for resetting to default recurrence
        break;

    case 0xA0: // Command to set alarm parameter 0
    case 0xA1: // Command to set alarm parameter 1
    case 0xA2: // Command to set alarm parameter 2
        break;

    case 0xB0: // Command to disable alarm 0
    case 0xB1: // Command to disable alarm 1
    case 0xB2: // Command to disable alarm 2
        break;

    case 0xD0: // Command to delete alarm 0
    case 0xD1: // Command to delete alarm 1
    case 0xD2: // Command to delete alarm 2
        break;

    case 0xEE: // Command for time configuration
        response[0] = cmd;
        response[1] = 0;
        setUnixTimeBytes(RTCSingleton::rtc.getEpoch());
        pCharacteristic->setValue(response, 11);
        pCharacteristic->notify();
        break;

    default:
        break;
    }
}

ConfigurationService::ConfigurationService(BLEServer *pServer)
{
    sensorManager2 = &SensorManager::getInstance();
    alarmManager1 = &AlarmManager::getInstance();

    // Initialize the service
    pService = pServer->createService(CONFIGURATION_SERVICE_UUID);

    // Create a characteristic
    pCharacteristic = pService->createCharacteristic(
        CONFIG_CHARACTERISTIC_UUID,
        BLECharacteristic::PROPERTY_READ |
            BLECharacteristic::PROPERTY_WRITE |
            BLECharacteristic::PROPERTY_NOTIFY |
            BLECharacteristic::PROPERTY_INDICATE);

    // Set characteristic callbacks
    pCharacteristic->setCallbacks(new ConfigCharacteristicCallback());

    // Create Client Characteristic Configuration Descriptor (CCC) for characteristic
    pCharacteristic->addDescriptor(new BLE2902());

    // Create Characteristic User Description Descriptor for characteristic
    BLEDescriptor pDescriptorUserDescription(BLEUUID((uint16_t)0x2901));
    pDescriptorUserDescription.setValue("config");
    pCharacteristic->addDescriptor(&pDescriptorUserDescription);

    pService->start();
}

void ConfigurationService::noitify()
{
    pCharacteristic->notify();
}

static inline void setRecurrenceBytes(uint32_t recurrence)
{
    response[3] = recurrence >> 24;
    response[4] = recurrence >> 16;
    response[5] = recurrence >> 8;
    response[6] = recurrence;
}

static inline uint32_t getRecurrenceBytes(const uint8_t *data)
{
    uint32_t recurrence = 0;
    recurrence |= data[3] << 24;
    recurrence |= data[4] << 16;
    recurrence |= data[5] << 8;
    recurrence |= data[6];
    return recurrence;
}

float reverseBytesToFloat(const uint8_t *bytes)
{
    uint8_t reversedBytes[sizeof(float)];
    for (size_t i = 0; i < sizeof(float); ++i)
    {
        reversedBytes[i] = bytes[sizeof(float) - 1 - i];
    }
    return *reinterpret_cast<const float *>(reversedBytes);
}

static inline void setAlarmComand(uint8_t alarmIndex, const std::string &value)
{

    ParameterType parameter = static_cast<ParameterType>(value[1]);
    if (value.length() >= 11)
    {
        IntervalType intervalType = static_cast<IntervalType>(value[2]); // Get the interval type
        // Check if the interval type is valid
        if (intervalType != INSIDE && intervalType != OUTSIDE && intervalType != DISABLED_INTERVAL)
        {
            DEBUG("Invalid interval type\n");
            return;
        }
        // If the interval type is disabled, there is no need to set the lower and upper values
        float lowerValue = 0.0f;
        float upperValue = 0.0f;
        if (intervalType != DISABLED_INTERVAL)
        {
            // Extract lower and upper values from the command
            lowerValue = reverseBytesToFloat(reinterpret_cast<const uint8_t *>(&value[3]));
            upperValue = reverseBytesToFloat(reinterpret_cast<const uint8_t *>(&value[7]));
        }
        DEBUG("alarm index %d\nparameter %d\n interval %d\nlower %f\nupper %f\n", alarmIndex, parameter, intervalType, lowerValue, upperValue);
        // alarmManager1->setAlarm(alarmIndex, parameter, intervalType, lowerValue, upperValue);
    }

    // Set the alarm
}

static inline void setUnixTimeBytes(uint64_t unixTime)
{
    for (int byte = 0; byte < 8; byte++)
    {
        response[3 + byte] = (unixTime >> ((7 - byte) * 8)) & 0xFF;
    }
}

static inline uint64_t getUnixTimeBytes(const uint8_t *data)
{
    uint64_t unixTime = 0;
    for (int byte = 0; byte < 8; byte++)
    {
        unixTime |= static_cast<uint64_t>(data[3 + byte]) << ((7 - byte) * 8);
    }
    return unixTime;
}
