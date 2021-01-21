#ifndef BLEHelper_h
#define BLEHelper_h

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include "arduino.h"

class BLEHelper : public BLEServerCallbacks
{
public:
  BLEHelper();
  void setup();
  void notifyHeartRate(unsigned int heartRate);
  void notifyBatteryLevel(uint8_t level);
  void onConnect(BLEServer *pServer);
  void onDisconnect(BLEServer *pServer);

private:
  BLECharacteristic *_measurementCharacteristics = NULL;
  BLECharacteristic *_sensorLocationCharacteristics = NULL;
  BLECharacteristic *_controlPointCharacteristics = NULL;
  BLECharacteristic *_batteryLevelCharacteristics = NULL;
  void startAdvertising(BLEServer *pServer);
};

#endif
