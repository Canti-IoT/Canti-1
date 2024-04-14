#include <math.hpp>
#ifndef PIO_UNIT_TESTING
#include <debug.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <esp_sleep.h>

#include "BLEServerManager.hpp"
#include <ParameterIndexService.hpp>
#include <ParameterValueService.hpp>

BLEServerManager *pServerManager = nullptr;




void setup()
{
  DEBUGINIT(115200);
  // Create the BLE Device
  BLEDevice::init("ESP32");

  // Initialize the BLE server manager
  pServerManager = new BLEServerManager();

  // Start advertising
  pServerManager->startAdvertising();

  //   // Set wake-up time to 60 seconds (in microseconds)
  // esp_sleep_enable_timer_wakeup(60 * 1000000);
  // delay(1000);
  // DEBUG("Entering Light Sleep mode...");
  // esp_light_sleep_start();
  // DEBUG("Exited Light Sleep mode");
}

void loop()
{
  delay(1000);    
  DEBUG(".\n");
  pServerManager->loopCycle();
}

#endif
