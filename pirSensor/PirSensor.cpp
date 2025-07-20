#include "PirSensor.h"

PirSensor::PirSensor(int pir, int led) {
  pirPin = pir;
  ledPin = led;
}

void PirSensor::begin() {
  pinMode(pirPin, INPUT);
  pinMode(ledPin, OUTPUT);
}

void PirSensor::checkAndLight() {
  int motionDetected = digitalRead(pirPin);
  if (motionDetected == HIGH) {
    digitalWrite(ledPin, HIGH);
  } else {
    digitalWrite(ledPin, LOW);
  }
}
