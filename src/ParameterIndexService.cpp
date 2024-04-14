#include "ParameterIndexService.hpp"
#include <BLE2902.h>

class IndexCharacteristicCallback : public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic);
    void onRead(BLECharacteristic *pCharacteristic);
};

void IndexCharacteristicCallback::onWrite(BLECharacteristic *pCharacteristic) {
    // Handle write event
}

void IndexCharacteristicCallback::onRead(BLECharacteristic *pCharacteristic) {
    // Handle read event
}

ParameterIndexService::ParameterIndexService(BLEServer *pServer) {
    // Initialize the service
    pService = pServer->createService(PARAMETER_INDEX_SERVICE_UUID);

    // Create a characteristic
    pCharacteristic = pService->createCharacteristic(
        INDEX_CHARACTERISTIC_UUID,
        BLECharacteristic::PROPERTY_READ |
        BLECharacteristic::PROPERTY_WRITE |
        BLECharacteristic::PROPERTY_NOTIFY |
        BLECharacteristic::PROPERTY_INDICATE
    );

    // Set characteristic callbacks
    pCharacteristic->setCallbacks(new IndexCharacteristicCallback());

   // Create Client Characteristic Configuration Descriptor (CCC) for characteristic
    pCharacteristic->addDescriptor(new BLE2902());

    // Create Characteristic User Description Descriptor for characteristic
    BLEDescriptor pDescriptorUserDescription(BLEUUID((uint16_t)0x2901));
    pDescriptorUserDescription.setValue("index");
    pCharacteristic->addDescriptor(&pDescriptorUserDescription);

    pService->start();
}

void ParameterIndexService::noitify(){
    pCharacteristic->notify();
}