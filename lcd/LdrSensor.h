#ifndef LDR_SENSOR_H
#define LDR_SENSOR_H
#include <Arduino.h>
class LdrSensor {
private:
  int pin;
  int minVal, maxVal;
public:
  LdrSensor(int analogPin);
  int readRaw();
  int readPercent();
  bool isDark(int threshold=70);
};
#endif