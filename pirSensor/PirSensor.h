#ifndef PIR_SENSOR_H
#define PIR_SENSOR_H

#include <Arduino.h>

class PirSensor {
  private:
    int pirPin;
    int ledPin;

  public:
    PirSensor(int pir, int led);
    void begin();
    void checkAndLight();
};

#endif
