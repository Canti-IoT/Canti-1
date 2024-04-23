#include <math.hpp>
#ifndef PIO_UNIT_TESTING
#include <debug.h>
#include <BLEDevice.h>
#include <esp_sleep.h>
#include <Wire.h>

#include "bluetooth/BLEServerManager.hpp"
#include <SensorManager.hpp>
#include <RTCSingleton.hpp>
#include <buzzer.hpp>

#define SDA_PIN 8
#define SCL_PIN 9
#define BUZZER_PIN 2

BLEServerManager *pServerManager = nullptr;
SensorManager *sensorManager = nullptr;



Buzzer b(BUZZER_PIN);

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
  BLEDevice::init("Canti 1");
  // Initialize the BLE server manager
  pServerManager = new BLEServerManager();
  // Start advertising
  pServerManager->startAdvertising();

  RTCSingleton::rtc.offset = GMT_OFFSET;
  RTCSingleton::rtc.setTime(BUILD_TIMESTAMP+30);

  sensorManager = &SensorManager::getInstance();

  DEBUG("%d %d %d", RTCSingleton::rtc.getYear(), RTCSingleton::rtc.getMonth(), RTCSingleton::rtc.getDay());
  //   // Set wake-up time to 60 seconds (in microseconds)
  // esp_sleep_enable_timer_wakeup(60 * 1000000);
  // delay(1000);
  // DEBUG("Entering Light Sleep mode...");
  // esp_light_sleep_start();
  // DEBUG("Exited Light Sleep mode");
  b.init();
  b.enable();
}

uint64_t last_read_main = 0;

void loop()
{
  if(RTCSingleton::rtc.getSecond() % 5 == 0) {
  TIMESTAMP();
  DEBUG("\n");
  }
  delay(500);
  pServerManager->loopCycle();
  sensorManager->loop();
  if (last_read_main == 0 || last_read_main + 15000 < millis())
  {
    TIMESTAMP();
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
    TIMESTAMP();
    DEBUG("Read finished\n");
  }
  b.playTune1();
  b.disable();
}

#endif
