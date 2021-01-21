#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>

#include "arduino.h"
#include "BLEHelper.hh"

#define hrmServiceId BLEUUID((uint16_t)0x180D)
#define deviceInformationServiceId BLEUUID((uint16_t)0x180A)
#define batteryLevelServiceId BLEUUID((uint16_t)0x180F)

BLEHelper::BLEHelper()
{
}

void BLEHelper::setup()
{
    BLEDevice::init("HRM1");
    BLEServer *pServer = BLEDevice::createServer();
    pServer->setCallbacks(this);

    BLEService *hrmService = pServer->createService(hrmServiceId);

    _measurementCharacteristics = hrmService->createCharacteristic(
        BLEUUID((uint16_t)0x2A37), BLECharacteristic::PROPERTY_NOTIFY);

    // Client Characteristic Configuration
    _measurementCharacteristics->addDescriptor(new BLE2902());

    // Body Sensor Location
    /*
    0x00 Other
    0x01 Chest
    0x02 Wrist
    0x03 Finger
    0x04 Hand
    0x05 Ear Lobe
    0x06 Foot
    0x07–0xFF Reserved for Future Use*/
    byte sensorLocationValue[1] = { 0x01 }; // Chest
    BLECharacteristic *sensorLocation = hrmService->createCharacteristic(BLEUUID((uint16_t)0x2A38), BLECharacteristic::PROPERTY_READ);
    sensorLocation->setValue(sensorLocationValue, 1);

    // Characteristic User Description
    BLEDescriptor userDescription(BLEUUID((uint16_t)0x2901));
    userDescription.setValue("Demo 50-60 bpm");
    _measurementCharacteristics->addDescriptor(&userDescription);

    // Device Information Service - Did not help by it self.
    BLEService *deviceInformationService = pServer->createService(deviceInformationServiceId);
    BLECharacteristic *firmware = deviceInformationService->createCharacteristic(BLEUUID((uint16_t)0x2A26), BLECharacteristic::PROPERTY_READ);
    firmware->setValue("1.2.3");
    BLECharacteristic *hardware = deviceInformationService->createCharacteristic(BLEUUID((uint16_t)0x2A27), BLECharacteristic::PROPERTY_READ);
    hardware->setValue("4.5.6");
    BLECharacteristic *manufacturer = deviceInformationService->createCharacteristic(BLEUUID((uint16_t)0x2A29), BLECharacteristic::PROPERTY_READ);
    manufacturer->setValue("ACME");

    BLEService *batteryLevelService = pServer->createService(batteryLevelServiceId);
    _batteryLevelCharacteristics = batteryLevelService->createCharacteristic(BLEUUID((uint16_t)0x2A19), BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);
    _batteryLevelCharacteristics->addDescriptor(new BLE2902());

    deviceInformationService->start();
    hrmService->start();
    batteryLevelService->start();
    this->startAdvertising(pServer);
}

void BLEHelper::onConnect(BLEServer *pServer)
{
    Serial.println("onConnect");
};

void BLEHelper::onDisconnect(BLEServer *pServer)
{
    Serial.println("onDisconnect");
}

void BLEHelper::startAdvertising(BLEServer *pServer)
{
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();

    pAdvertising->addServiceUUID(hrmServiceId);

    // If the following two lines are left out, nrfConnect will show duplicated services.
    pAdvertising->setScanResponse(false);
    pAdvertising->setMinPreferred(0x0); // set value to 0x00 to not advertise this parameter

    BLEDevice::startAdvertising();
}

void BLEHelper::notifyHeartRate(unsigned int heartRate)
{
    /*
    Field #1 - Flags (byte)
        Bit 0   - Heart Rate Value Format
                    0 = uint8
                    1 = uint16
        Bit 1-2 - Sensor Contact Status
                    0 - Sensor Contact feature is not supported in the current connection
                    1 - Sensor Contact feature is not supported in the current connection
                    2 - Sensor Contact feature is supported, but contact is not detected
                    3 - Sensor Contact feature is supported and contact is detected
        Bit 3   - Energy Expended Status
                    0 = Energy Expended field is not present
                    1 = Energy Expended field is present. Units: kilo Joules
        Bit 3   - RR-Interval bit
                    0 = RR-Interval values are not present.
                    1 = One or more RR-Interval values are present.
        Bit 5-7 - Reserved
    Field #2 - Heart Rate Measurement Value (uint8)
    Field #3 - Heart Rate Measurement Value (uint16)
    Field #4 - Energy Expended (uint16)
    Field #5 - RR-Interval (uint16)
    */

    // Flags = Format uint16
    byte cscMeasurementFlags = 0b00000001;

    // Flags + Heart rate (uint16) = 3 bytes
    byte value[3] = { cscMeasurementFlags, 0, 0 };

    value[1] = heartRate & 0xFF;
    value[2] = (heartRate >> 8) & 0xFF;

    _measurementCharacteristics->setValue(value, 3);
    _measurementCharacteristics->notify();
}

void BLEHelper::notifyBatteryLevel(uint8_t level)
{
    byte value[1] = { level };
    _batteryLevelCharacteristics->setValue(value, 1);
    _batteryLevelCharacteristics->notify();
}
