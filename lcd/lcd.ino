
#include "LdrSensor.h"
#include "LCDLibrary.h"

const int led1 = 13;
const int led2 = 14;
const int led3 = 26;
const int ldrPin = 34;

LdrSensor ldr(ldrPin);
MyLCDLibrary myLCD(0x27, 16, 2);  

void setup() {
  Serial.begin(115200);

  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);

  myLCD.begin();
  myLCD.printMessage("System Ready");
  delay(1000);
}

void loop() {
  int lightPercent = ldr.readPercent();

  Serial.print("Light Level: ");
  Serial.print(lightPercent);
  Serial.println("%");

  String message = "Light: " + String(lightPercent) + "%\n";

  if (lightPercent < 20) {
    digitalWrite(led1, LOW);
    digitalWrite(led2, LOW);
    digitalWrite(led3, LOW);
    message += "Bright";
  } else if (lightPercent < 50) {
    digitalWrite(led1, HIGH);
    digitalWrite(led2, LOW);
    digitalWrite(led3, LOW);
    message += "Slightly Dark";
  } else if (lightPercent < 80) {
    digitalWrite(led1, HIGH);
    digitalWrite(led2, HIGH);
    digitalWrite(led3, LOW);
    message += "Dim";
  } else {
    digitalWrite(led1, HIGH);
    digitalWrite(led2, HIGH);
    digitalWrite(led3, HIGH);
    message += "Very Dark";
  }

  myLCD.printMessage(message);

  delay(500);
}












