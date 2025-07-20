#include "LdrSensor.h"

const int led1 = 13;
const int led2 = 14;
const int led3 = 26;
const int ldrPin = 34;

LdrSensor ldr(ldrPin);

void setup() {
  Serial.begin(115200);
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
}

void loop() {
  int lightPercent = ldr.readPercent();  // 0% = אור מלא, 100% = חושך

  Serial.print("Light Level: ");
  Serial.print(lightPercent);
  Serial.println("%");

  if (lightPercent < 20) {
    // מואר – לא להדליק כלום
    digitalWrite(led1, LOW);
    digitalWrite(led2, LOW);
    digitalWrite(led3, LOW);
  }
  else if (lightPercent < 50) {
    // טיפה חושך – לד 1
    digitalWrite(led1, HIGH);
    digitalWrite(led2, LOW);
    digitalWrite(led3, LOW);
  }
  else if (lightPercent < 80) {
    // בינוני – 2 לדים
    digitalWrite(led1, HIGH);
    digitalWrite(led2, HIGH);
    digitalWrite(led3, LOW);
  }
  else {
    // ממש חשוך – 3 לדים
    digitalWrite(led1, HIGH);
    digitalWrite(led2, HIGH);
    digitalWrite(led3, HIGH);
  }

  delay(500);
}

// //חיישן תנועה
// #define PIR_PIN 27  // הפין שבו מחובר ה-OUT של החיישן

// int previousState = LOW;  // נשמור את המצב הקודם

// void setup() {
//   Serial.begin(115200);
//   pinMode(PIR_PIN, INPUT);
// }

// void loop() {
//   int currentState = digitalRead(PIR_PIN);

//   if (currentState != previousState) {  // אם היה שינוי במצב
//     if (currentState == HIGH) {
//       Serial.println("תנועה זוהתה!");
//     } else {
//       Serial.println("לא זוהתה תנועה.");
//     }
//     previousState = currentState;  // עדכון המצב הקודם
//   }

//   delay(100);  // בדיקה כל 100 מילישניות
// }




//מסך
// #include <Wire.h>

// void setup() {
//   Serial.begin(115200);
//   delay(1000);
//   Serial.println("סריקת I2C עם הגדרת פינים");

//   Wire.begin(12, 22);  // <<<<< חשוב! SDA=12, SCL=22

//   byte found = 0;
//   for (byte address = 1; address < 127; address++) {
//     Wire.beginTransmission(address);
//     if (Wire.endTransmission() == 0) {
//       Serial.print("נמצא התקן בכתובת 0x");
//       Serial.println(address, HEX);
//       found++;
//     }
//   }

//   if (found == 0) {
//     Serial.println("לא נמצאו התקני I2C 😢");
//   } else {
//     Serial.println("הסריקה הסתיימה ✔️");
//   }
// }

// void loop() {}







