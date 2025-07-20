//חיישן תזוזה
// #include "PirSensor.h"

// PirSensor sensor(27, 14); // PIR מחובר ל-D2, נורה ל-D13

// void setup() {
//   Serial.begin(9600);
//   sensor.begin();
// }

// void loop() {
//   sensor.checkAndLight();
// }

#include "LdrSensor.h"
#include "PirSensor.h"

const int led1 = 13;
const int led2 = 14;
const int led3 = 26;
const int ldrPin = 34;
const int pirPin = 27; // שימי את חיישן התנועה בכניסה דיגיטלית פנויה

LdrSensor ldr(ldrPin);
PirSensor pir(pirPin, -1); // לא נשתמש ב-ledPin מתוך PirSensor

void setup() {
  Serial.begin(115200);
  
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  
  pir.begin();
}

void loop() {
  int lightPercent = ldr.readPercent();  // 0% = אור מלא, 100% = חושך
  int motionDetected = digitalRead(pirPin); // נקרא ישירות כדי לשלוט אנחנו

  Serial.print("Light: ");
  Serial.print(lightPercent);
  Serial.print("%, Motion: ");
  Serial.println(motionDetected == HIGH ? "Yes" : "No");

  if (lightPercent > 20 && motionDetected == HIGH) {
    // חושך + תזוזה — להדליק בהתאם לרמת החושך
    if (lightPercent < 50) {
      digitalWrite(led1, HIGH);
      digitalWrite(led2, LOW);
      digitalWrite(led3, LOW);
    }
    else if (lightPercent < 80) {
      digitalWrite(led1, HIGH);
      digitalWrite(led2, HIGH);
      digitalWrite(led3, LOW);
    }
    else {
      digitalWrite(led1, HIGH);
      digitalWrite(led2, HIGH);
      digitalWrite(led3, HIGH);
    }
  } else {
    // אין תזוזה או שיש אור – כבה הכל
    digitalWrite(led1, LOW);
    digitalWrite(led2, LOW);
    digitalWrite(led3, LOW);
  }

  delay(300);
}

