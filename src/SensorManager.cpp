#include "SensorManager.hpp"
#include <sensor/Bme680Adapter.hpp>

SensorManager::SensorManager() {
    currentSize = 0;

    // Initialize SensorData elements
    for (int i = 0; i < MAX_SENSORS; ++i) {
        sensors[i].parameter = ParameterType::NONE;
        sensors[i].recurrence = 0;
        sensors[i].sensor = nullptr; // Initialize sensor pointer to nullptr
        sensors[i].value = 0.0f;
    }
}

void SensorManager::addSensor(ParameterType parameter, int recurrence, AbstractSensor* sensor) {
    if (currentSize < MAX_SENSORS) {
        sensors[currentSize].parameter = parameter; 
        sensors[currentSize].recurrence = recurrence; 
        sensors[currentSize].sensor = sensor;
        sensors[currentSize].value = 0.0f;
        currentSize += 1;
    } else {
        // Handle error: array full
    }
}

SensorData* SensorManager::getSensor(ParameterType parameter) {
    for (int i = 0; i < currentSize; ++i) {
        if (sensors[i].parameter == parameter) {
            return &sensors[i];
        }
    }
    return nullptr; // Return nullptr if sensor data not found
}

void SensorManager::setRecurrenceWithIndex(ParameterType index, int recurrence) {
    for (int i = 0; i < currentSize; ++i) {
        if (sensors[i].parameter == index) {
            sensors[i].recurrence = recurrence;
            break; // Exit loop once recurrence is set
        }
    }
}

void SensorManager::beginReadingAll() {
    for (int i = 0; i < currentSize; ++i) {
        sensors[i].sensor->beginReading(sensors[i].parameter);
    }
}

float SensorManager::getValue(int index) {
    for (int i =  0; i < currentSize; i++)
    {
        if (index == sensors[i].parameter){
            return sensors[i].value;
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