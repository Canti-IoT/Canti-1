#include "SensorManager.hpp"
#include <sensor/Bme680Adapter.hpp>
#include <sensor/Veml7700Adapter.hpp>
#include <sensor/AdcMicrophoneAdapter.hpp>
#include "sensor/SHT40Adapter.hpp"
#include <debug.h>
#include <RTCSingleton.hpp>

RTC_DATA_ATTR float value[MAX_SENSORS];
RTC_DATA_ATTR uint64_t last_read[MAX_SENSORS];
RTC_DATA_ATTR int recurrence[MAX_SENSORS];


SensorManager::SensorManager() {
    currentSize = 0;

    SHT40Adapter* sht40 = new SHT40Adapter();   
    Bme680Adapter* bme = new Bme680Adapter();
    addSensor(TEMPERATURE, 60, sht40);
    addSensor(HUMIDITY, 60, sht40);
    addSensor(PRESSURE, 60, bme);
    addSensor(ALTITUDE, 60, bme);
    addSensor(VOCS, 60, bme);
    Veml7700Adapter* veml = new Veml7700Adapter();
    addSensor(ILLUMINATION, 60, veml);
    AdcMicrophoneAdapter *mic = new AdcMicrophoneAdapter();
    addSensor(NOISE, 60, mic);
}

void SensorManager::addSensor(ParameterType parameter, int recurrence_value, AbstractSensor* sensor) {
    if (currentSize < MAX_SENSORS) {
        sensors[currentSize].parameter = parameter; 
        sensors[currentSize].sensor = sensor;
        recurrence[currentSize] = recurrence_value;
        currentSize += 1;
    } else {
        // Handle error: array full
    }
}

void SensorManager::setRecurrenceWithIndex(ParameterType index, int new_recurrence) {
    for (int i = 0; i < currentSize; ++i) {
        if (sensors[i].parameter == index) {
            recurrence[i] = new_recurrence;
            break; // Exit loop once recurrence is set
        }
    }
}

void SensorManager::initAll() {
    for (int i = 0; i < currentSize; ++i) {
        sensors[i].sensor->init();
    }
}

void SensorManager::testAll() {
    for (int i = 0; i < currentSize; ++i) {
        sensors[i].sensor->test();
    }
}

void SensorManager::readAll() {
    for (int i = 0; i < currentSize; ++i) {
        sensors[i].sensor->read(sensors[i].parameter);
    }
}

void SensorManager::loop() {
    // Initialize state and iteration counters
    static ManagingStates currentState = ManagingStates::INITIALIZATION;
    static int iterationCount = 0;

    switch (currentState) {
        case ManagingStates::INITIALIZATION:
            DEBUG("Entering INITIALIZATION state. Iteration count: %d\n", iterationCount);
            // Initialize one sensor at a time
            if (iterationCount < currentSize) {
                sensors[iterationCount].sensor->init();
                iterationCount++;
            } else {
                currentState = ManagingStates::TESTING; // Move to TESTING state after initialization
                iterationCount = 0; // Reset iteration counter
            }
            break;

        case ManagingStates::TESTING:
            DEBUG("Entering TESTING state. Iteration count: %d\n", iterationCount);
            // Test one sensor at a time
            if (iterationCount < currentSize) {
                sensors[iterationCount].sensor->test();
                iterationCount++;
            } else {
                currentState = ManagingStates::READING; // Move to READING state after testing all sensors
                iterationCount = 0; // Reset iteration counter
            }
            break;

        case ManagingStates::READING:
            DEBUG("Entering READING state. Iteration count: %d\n", iterationCount);
            // Read one sensor at a time
            if (iterationCount < currentSize) {
                value[iterationCount] = sensors[iterationCount].sensor->readValue(sensors[iterationCount].parameter);
                last_read[iterationCount] = 1;
                iterationCount++;
            } else {
                currentState = ManagingStates::WAITING; // Move to WAITING state after reading all sensors
                iterationCount = 0; // Reset iteration counter
            }
            break;

        case ManagingStates::WAITING:
            DEBUG("Entering WAITING state. Iteration count: %d\n", iterationCount);
            // Check if any recurrence has expired
            for (int i = 0; i < currentSize; ++i) {
                if (recurrence[i] > 0 && (millis() - last_read[i]) >= recurrence[i] * 1000) {
                    // Recurrence has expired for at least one sensor
                    currentState = ManagingStates::READING; // Transition to READING state
                    break;
                }
            }
            break;

        default:
            break;
    }
}



float SensorManager::getValue(int index) {
    for (int i =  0; i < currentSize; i++)
    {
        if (index == sensors[i].parameter){
            return value[i];
        }
    }
}

bool SensorManager::validateIndex(int index){
    for (int i =  0; i < currentSize; i++)
    {
        if (index == sensors[i].parameter){
            return true;
        }
    }
    return false;
}