#include <math.hpp>
#ifndef PIO_UNIT_TESTING
#include <debug.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <esp_sleep.h>
#include <Wire.h>

#include "BLEServerManager.hpp"
#include <ParameterIndexService.hpp>
#include <ParameterValueService.hpp>
#include <SensorManager.hpp>
#include <RTCSingleton.hpp>

BLEServerManager *pServerManager = nullptr;
SensorManager *sensorManager = nullptr;

#define SDA_PIN 8
#define SCL_PIN 9

#define GMT_OFFSET (3600 * 3)
// ESP32Time RTCSingleton::rtc(GMT_OFFSET);

void setup()
{
  // DEBUGINIT(115200);
  USBSerial.begin(115200);
  while (!USBSerial)
  {
    delay(20);
  }
  DEBUG("Setup started\n");

  Wire.setPins(SDA_PIN, SCL_PIN);

  // Create the BLE Device
  BLEDevice::init("ESP32");

  // Initialize the BLE server manager
  pServerManager = new BLEServerManager();

  // Start advertising
  pServerManager->startAdvertising();

  RTCSingleton::rtc.offset = GMT_OFFSET;

  sensorManager = &SensorManager::getInstance();

  // if (RTCSingleton::rtc.getYear() != 2024)
  // {
    RTCSingleton::rtc.setTime(BUILD_TIMESTAMP+30);
  // }
  DEBUG("%d %d %d", RTCSingleton::rtc.getYear(), RTCSingleton::rtc.getMonth(), RTCSingleton::rtc.getDay());
  //   // Set wake-up time to 60 seconds (in microseconds)
  // esp_sleep_enable_timer_wakeup(60 * 1000000);
  // delay(1000);
  // DEBUG("Entering Light Sleep mode...");
  // esp_light_sleep_start();
  // DEBUG("Exited Light Sleep mode");
}

uint64_t last_read_main = 0;

void loop()
{
  delay(500);
  DEBUG("%d:%d:%d:%d\n", RTCSingleton::rtc.getHour(), RTCSingleton::rtc.getMinute(), RTCSingleton::rtc.getSecond(), RTCSingleton::rtc.getMillis());
  pServerManager->loopCycle();
  sensorManager->loop();
  if (last_read_main == 0 || last_read_main + 15000 < millis())
  {
    DEBUG("Read started\n");
    float temperature = sensorManager->getValue(TEMPERATURE);
    float humidity = sensorManager->getValue(HUMIDITY);
    float pressure = sensorManager->getValue(PRESSURE);
    float altitude = sensorManager->getValue(ALTITUDE);
    float iaq = sensorManager->getValue(VOCS);
    float lux = sensorManager->getValue(ILLUMINATION);
    float db = sensorManager->getValue(NOISE);
    DEBUG("temp: %f\n", temperature);
    DEBUG("hum: %f\n", humidity);
    DEBUG("pre: %f\n", pressure);
    DEBUG("alt: %f\n", altitude);
    DEBUG("iaq: %f\n", iaq);
    DEBUG("lux: %f\n", lux);
    DEBUG("dbA: %f\n", db);
    last_read_main = millis();
    DEBUG("Read finished\n");
  }
}

#endif
