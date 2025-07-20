#include "Arduino.h"
#include "esp32-hal-adc.h"
#include "LdrSensor.h"
LdrSensor::LdrSensor(int analogPin){
  pin=analogPin;
  minVal=0;
  maxVal=4095;
}
int LdrSensor::readRaw(){
  return analogRead(pin);
}
int LdrSensor::readPercent(){
  int raw=readRaw();
  int percent=map(raw, minVal, maxVal, 100, 0);
  return constrain(percent, 0, 100);
}
bool LdrSensor::isDark(int threshold){
  return readPercent()>threshold;
}