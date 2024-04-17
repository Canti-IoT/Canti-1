#include "SensorManager.hpp"
#include <sensor/Bme680Adapter.hpp>
#include <sensor/Veml7700Adapter.hpp>
#include <debug.h>

SensorManager::SensorManager() {
    currentSize = 0;

    Bme680Adapter* bme = new Bme680Adapter();
    addSensor(TEMPERATURE, 60, bme);
    addSensor(HUMIDITY, 60, bme);
    addSensor(PRESSURE, 60, bme);
    addSensor(ALTITUDE, 60, bme);
    addSensor(VOCS, 60, bme);
    Veml7700Adapter* veml = new Veml7700Adapter();
    addSensor(ILLUMINATION, 60, veml);
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

void SensorManager::setRecurrenceWithIndex(ParameterType index, int recurrence) {
    for (int i = 0; i < currentSize; ++i) {
        if (sensors[i].parameter == index) {
            sensors[i].recurrence = recurrence;
            break; // Exit loop once recurrence is set
        }
    }
}

void SensorManager::initAll() {
    for (int i = 0; i < currentSize; ++i) {
        sensors[i].sensor->init();
        DEBUG("Sensor init status: %d\n", sensors[i].sensor->initiated);
    }
}

void SensorManager::testAll() {
    for (int i = 0; i < currentSize; ++i) {
        sensors[i].sensor->test();
        DEBUG("Sensor test status: %d\n", sensors[i].sensor->testSucces);
    }
}

void SensorManager::readAll() {
    for (int i = 0; i < currentSize; ++i) {
        sensors[i].sensor->read(sensors[i].parameter);
    }
}

float SensorManager::getValue(int index) {
    for (int i =  0; i < currentSize; i++)
    {
        if (index == sensors[i].parameter){
            sensors[i].value = sensors[i].sensor->readValue(sensors[i].parameter);
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