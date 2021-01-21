// The purpose of this sketch is to find how to create a BLE service that a Garmin FR935 can connect to.
#include "BLEHelper.hh"

BLEHelper bleHelper;
unsigned int heartRate = 50;
int change = 1;
uint8_t batteryLevel = 50;

void setup()
{
    Serial.begin(115200);
    Serial.println("Start");
    bleHelper.setup();
}

void loop()
{
    bleHelper.notifyHeartRate(heartRate);
    bleHelper.notifyBatteryLevel(batteryLevel);

    if (change<0) 
    {
      heartRate--;
      batteryLevel--;
    }
    else{
      heartRate++;
      batteryLevel++;
    }
    
    delay(700);

    if (heartRate>59)
      change=-1;

    if (heartRate<51)
      change=1;
}
