#include "ParameterValueService.hpp"
#include <BLE2902.h>

class ValueCharacteristicCallback : public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic);
    void onRead(BLECharacteristic *pCharacteristic);
};

void ValueCharacteristicCallback::onWrite(BLECharacteristic *pCharacteristic) {
    // Handle write event
}

void ValueCharacteristicCallback::onRead(BLECharacteristic *pCharacteristic) {
    // Handle read event
}

ParameterValueService::ParameterValueService(BLEServer *pServer) {
    // Initialize the service
    pService = pServer->createService(PARAMETER_VALUE_SERVICE_UUID);

    // Create a characteristic
    pCharacteristic = pService->createCharacteristic(
        VALUE_CHARACTERISTIC_UUID,
        BLECharacteristic::PROPERTY_READ |
        BLECharacteristic::PROPERTY_WRITE |
        BLECharacteristic::PROPERTY_NOTIFY |
        BLECharacteristic::PROPERTY_INDICATE
    );

    // Set characteristic callbacks
    pCharacteristic->setCallbacks(new ValueCharacteristicCallback());

   // Create Client Characteristic Configuration Descriptor (CCC) for characteristic
    pCharacteristic->addDescriptor(new BLE2902());

    // Create Characteristic User Description Descriptor for characteristic
    BLEDescriptor pDescriptorUserDescription(BLEUUID((uint16_t)0x2901));
    pDescriptorUserDescription.setValue("value");
    pCharacteristic->addDescriptor(&pDescriptorUserDescription);

    pService->start();
}

void ParameterValueService::noitify(){
    pCharacteristic->notify();
}