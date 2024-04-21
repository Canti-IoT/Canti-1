#include "ConfigurationService.hpp"
#include <BLE2902.h>

class ConfigCharacteristicCallback : public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic);
    void onRead(BLECharacteristic *pCharacteristic);
};

void ConfigCharacteristicCallback::onWrite(BLECharacteristic *pCharacteristic) {
    // Handle write event
}

void ConfigCharacteristicCallback::onRead(BLECharacteristic *pCharacteristic) {
    // Handle read event
}

ConfigurationService::ConfigurationService(BLEServer *pServer) {
    // Initialize the service
    pService = pServer->createService(CONFIGURATION_SERVICE_UUID);

    // Create a characteristic
    pCharacteristic = pService->createCharacteristic(
        CONFIG_CHARACTERISTIC_UUID,
        BLECharacteristic::PROPERTY_READ |
        BLECharacteristic::PROPERTY_WRITE |
        BLECharacteristic::PROPERTY_NOTIFY |
        BLECharacteristic::PROPERTY_INDICATE
    );

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

void ConfigurationService::noitify(){
    pCharacteristic->notify();
}