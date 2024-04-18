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

BLEServerManager *pServerManager = nullptr;
SensorManager *sensorManager = nullptr;

#define SDA_PIN 8
#define SCL_PIN 9


void setup()
{
  // DEBUGINIT(115200);
  USBSerial.begin(115200);
  while (!USBSerial) {}

  Wire.setPins(SDA_PIN, SCL_PIN);

  // Create the BLE Device
  BLEDevice::init("ESP32");

  // Initialize the BLE server manager
  pServerManager = new BLEServerManager();

  // Start advertising
  pServerManager->startAdvertising();

  sensorManager = new SensorManager();
  sensorManager->initAll();
  sensorManager->testAll();
  //   // Set wake-up time to 60 seconds (in microseconds)
  // esp_sleep_enable_timer_wakeup(60 * 1000000);
  // delay(1000);
  // DEBUG("Entering Light Sleep mode...");
  // esp_light_sleep_start();
  // DEBUG("Exited Light Sleep mode");
}

uint64_t last_read = 0;

void loop()
{
  delay(1000);    
  DEBUG(".\n");
  pServerManager->loopCycle();

  DEBUG("%u", millis()); 
  if (last_read == 0 || last_read + 15000 < millis())
  {
    DEBUG("Read started\n");
    sensorManager->initAll();
    sensorManager->readAll();
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
    last_read = millis();
    DEBUG("Read finished\n");
  }
}

#endif
