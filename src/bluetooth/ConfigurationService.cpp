#include "bluetooth/ConfigurationService.hpp"
#include <BLE2902.h>
#include <debug.h>
#include <SensorManager.hpp>

uint8_t cmd = 0;
uint8_t arg = 0;
SensorManager *sensorManager2 = nullptr;
uint8_t response[20] = {};

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
        DEBUG("%x", value[i]);
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
            // Extract the recurrence value from bytes 3 to 6
            uint32_t recurrenceValue = 0;
            for (int byte = 0; byte < 4; byte++)
            {
                recurrenceValue |= value[3 + byte] << (3 - byte) * 8;
            }
            DEBUG("New REcurrence %d for p %d\n", recurrenceValue, arg);
            sensorManager2->setRecurrenceWithIndex(static_cast<ParameterType>(arg), recurrenceValue);
            // Call function to update recurrence configuration
            // updateRecurrence(arg, recurrenceValue);

            // Read and store the recurrence value for future use
            // (if necessary)
        }
        else if (value.length() >= 2)
        {
            DEBUG("Reading recurrence\n");
            uint32_t recurrence = sensorManager2->getRecurrenceWithIndex(static_cast<ParameterType>(arg));
            response[0] = cmd;
            response[1] = arg;
            response[3] = recurrence >> 24;
            response[4] = recurrence >> 16;
            response[5] = recurrence >> 8;
            response[6] = recurrence;
            pCharacteristic->setValue(response, 7);
            pCharacteristic->notify();
        }
        break;

    case 0xF2: // Command for resetting to default recurrence
        // Implement the logic to handle resetting to default recurrence
        break;

    case 0xA0: // Command to set alarm parameter 0
    case 0xA1: // Command to set alarm parameter 1
    case 0xA2: // Command to set alarm parameter 2
        // Implement the logic to handle setting alarms
        break;

    case 0xB0: // Command to disable alarm 0
    case 0xB1: // Command to disable alarm 1
    case 0xB2: // Command to disable alarm 2
        // Implement the logic to handle disabling alarms
        break;

    case 0xD0: // Command to delete alarm 0
    case 0xD1: // Command to delete alarm 1
    case 0xD2: // Command to delete alarm 2
        // Implement the logic to handle deleting alarms
        break;

    case 0xEE: // Command for time configuration
        if (value.length() >= 10)
        {
            // Extract UNIXTIME from bytes 3 to 10
            uint64_t unixTime = *((uint64_t *)&value[3]);

            // Call function to set time configuration
            // setTimeConfiguration(unixTime);

            // Read and store the UNIXTIME for future use
            // (if necessary)
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
        break;

    default:
        break;
    }
}

ConfigurationService::ConfigurationService(BLEServer *pServer)
{
    sensorManager2 = &SensorManager::getInstance();

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