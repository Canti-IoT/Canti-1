#include "SensorManager.hpp"
#include <sensor/Bme680Adapter.hpp>
#include <sensor/Veml7700Adapter.hpp>
#include <sensor/AdcMicrophoneAdapter.hpp>
#include "sensor/SHT40Adapter.hpp"
#include <debug.h>

RTC_DATA_ATTR float value[MAX_SENSORS];
RTC_DATA_ATTR float last_read[MAX_SENSORS];
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

void SensorManager::addSensor(ParameterType parameter, int recurrence, AbstractSensor* sensor) {
    if (currentSize < MAX_SENSORS) {
        sensors[currentSize].parameter = parameter; 
        sensors[currentSize].sensor = sensor;
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

void SensorManager::loop()
{
    
}

float SensorManager::getValue(int index) {
    for (int i =  0; i < currentSize; i++)
    {
        if (index == sensors[i].parameter){
            value[i] = sensors[i].sensor->readValue(sensors[i].parameter);
            last_read[i] = 1;
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