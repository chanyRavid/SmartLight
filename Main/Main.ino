#include <Arduino.h>
#include <Wire.h>
#include "PirSensor.h"
#include "LdrSensor.h"
#include "LCDLibrary.h"
#include <Adafruit_NeoPixel.h>
#include <ThreeWire.h>
#include <RtcDS1302.h>
#include <WiFi.h>
#include <WebServer.h>
#include "time.h"

const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 2 * 3600;
const int daylightOffset_sec = 3600;


#define LED_PIN 26
#define NUM_LEDS 12

Adafruit_NeoPixel ring(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

const int PIN_POT = 33;
const int PIN_BTN = 15;
#define RST_PIN 12
#define DAT_PIN 23
#define CLK_PIN 18

const int PIR_PIN = 27;
const int PIR_LED = 12;
const int LDR_PIN = 35;

const unsigned long KEEP_ON_MS = 30UL * 1000;  

PirSensor pir(PIR_PIN, PIR_LED);
LdrSensor ldr(LDR_PIN);
MyLCDLibrary lcd(0x27, 16, 2);
ThreeWire myWire(DAT_PIN, CLK_PIN, RST_PIN);
RtcDS1302<ThreeWire> Rtc(myWire);
const int TURN_OFF_HOUR = 23;
const int TURN_OFF_MINUTE = 30;  
const int TURN_ON_HOUR = 6;      
const int TURN_ON_MINUTE = 0;    
int dynamicTurnOffHour = TURN_OFF_HOUR;
int dynamicTurnOffMinute = TURN_OFF_MINUTE;

const char* ssid = "BTWIFI_1279";
const char* password = "sscz090619";

WebServer server(80);

bool isRemoteOn = false;         
uint8_t remoteBrightness = 128;  


uint8_t colorIndex = 0;            
int lastBtnState = HIGH;           
unsigned long lastMotionTime = 0;  

void writeRgb(uint8_t r, uint8_t g, uint8_t b) {
  for (int i = 0; i < NUM_LEDS; i++) {
    ring.setPixelColor(i, ring.Color(r, g, b));
  }
  ring.show();
}

void setColorByIndex(uint8_t idx, uint8_t brightness) {
  switch (idx) {
    case 0: writeRgb(brightness, brightness, brightness); break;
    case 1: writeRgb(brightness, 0, 0); break;
    case 2: writeRgb(0, brightness, 0); break;
    case 3: writeRgb(0, 0, brightness); break;
    case 4: writeRgb(brightness, brightness, 0); break;
    case 5: writeRgb(0, brightness, brightness); break;
    case 6: writeRgb(brightness, 0, brightness); break;
  }
}

void syncTimeFromNTP() {
  Serial.println("מסנכרן זמן מהאינטרנט...");

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  struct tm timeinfo;
  int attempts = 0;
  while (!getLocalTime(&timeinfo) && attempts < 10) {
    Serial.println("ממתין לסנכרון זמן...");
    delay(1000);
    attempts++;
  }

  if (attempts < 10) {
    // עדכון השעון עם הזמן הנכון
    RtcDateTime networkTime = RtcDateTime(
      timeinfo.tm_year + 1900, 
      timeinfo.tm_mon + 1,      
      timeinfo.tm_mday,        
      timeinfo.tm_hour,        
      timeinfo.tm_min,         
      timeinfo.tm_sec           
    );

    Rtc.SetDateTime(networkTime);

    Serial.println("זמן סונכרן בהצלחה!");
    Serial.printf("זמן נוכחי: %02d/%02d/%d %02d:%02d:%02d\n",
                  timeinfo.tm_mday, timeinfo.tm_mon + 1, timeinfo.tm_year + 1900,
                  timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
  } else {
    Serial.println("כשל בסנכרון זמן - משתמש בזמן ברירת מחדל");
  }
}

//פונקציות לשליטה באפליקציה

void handleToggle() {
  isRemoteOn = !isRemoteOn;
  if (isRemoteOn)
    setColorByIndex(colorIndex, remoteBrightness);
  else
    writeRgb(0, 0, 0);
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "text/plain", "Toggled");
}

void handleColor() {
  colorIndex = (colorIndex + 1) % 7;
  if (isRemoteOn)
    setColorByIndex(colorIndex, remoteBrightness);
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "text/plain", "Color Changed");
}

void handleBrightness() {
  if (server.hasArg("plain")) {
    int newBrightness = server.arg("plain").toInt();
    remoteBrightness = constrain(newBrightness, 0, 255);
    if (isRemoteOn)
      setColorByIndex(colorIndex, remoteBrightness);
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(200, "text/plain", "Brightness Updated");
  } else {
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(400, "text/plain", "No value");
  }
}
void handleSetTime() {
  if (server.hasArg("hour") && server.hasArg("minute")) {
    int hour = server.arg("hour").toInt();
    int minute = server.arg("minute").toInt();

    // שמירה על התאריך הנוכחי ועדכון רק השעה והדקה
    RtcDateTime now = Rtc.GetDateTime();
    RtcDateTime newTime = RtcDateTime(now.Year(), now.Month(), now.Day(), hour, minute, 0);
    Rtc.SetDateTime(newTime);

    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(200, "text/plain", "Time Updated");
  } else {
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(400, "text/plain", "Missing parameters");
  }
}

//כיבוי אוטומטי בשעה מסוימת
void handleSetTurnOffTime() {
  if (server.hasArg("plain")) {
    String timeStr = server.arg("plain");

    int colonIndex = timeStr.indexOf(':');
    if (colonIndex > 0) {
      dynamicTurnOffHour = timeStr.substring(0, colonIndex).toInt();
      dynamicTurnOffMinute = timeStr.substring(colonIndex + 1).toInt();
      Serial.printf("זמן כיבוי עודכן ל: %02d:%02d\n", dynamicTurnOffHour, dynamicTurnOffMinute);

      server.sendHeader("Access-Control-Allow-Origin", "*");
      server.send(200, "text/plain", "Turn off time updated");
    } else {
      server.sendHeader("Access-Control-Allow-Origin", "*");
      server.send(400, "text/plain", "Invalid format");
    }
  } else {
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(400, "text/plain", "No time provided");
  }
}

void handleSyncTime() {
  syncTimeFromNTP();

  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "text/plain", "Time synced from internet");
}

bool useCustomColor = false;
uint8_t customR = 255, customG = 255, customB = 255;

void handleSetRGB() {
  Serial.println("RGB request received");
  
  if (server.hasArg("r") && server.hasArg("g") && server.hasArg("b")) {
    int r = server.arg("r").toInt();
    int g = server.arg("g").toInt();
    int b = server.arg("b").toInt();
    
    Serial.printf("RGB values: R=%d, G=%d, B=%d\n", r, g, b);
    
    customR = constrain(r, 0, 255);
    customG = constrain(g, 0, 255);
    customB = constrain(b, 0, 255);
    
    useCustomColor = true;
    isRemoteOn = true;
    
    writeRgb(customR, customG, customB);
    
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(200, "text/plain", "RGB Color Set");
  } else {
    Serial.println("Missing parameters");
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(400, "text/plain", "Missing RGB parameters");
  }
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("WiFi connected. IP: ");
  Serial.println(WiFi.localIP());

  pinMode(PIN_BTN, INPUT_PULLUP);
  pinMode(PIN_POT, INPUT);

  pir.begin();

  Wire.begin(21, 22);
  lcd.begin();
  lcd.printMessage("System ready");


  Rtc.Begin();

  if (Rtc.GetIsWriteProtected()) {
    Rtc.SetIsWriteProtected(false);
  }

  if (!Rtc.GetIsRunning()) {
    Rtc.SetIsRunning(true);
  }

  syncTimeFromNTP();

  ring.begin();
  ring.show();
  server.on("/toggle", HTTP_POST, handleToggle);
  server.on("/color", HTTP_POST, handleColor);
  server.on("/brightness", HTTP_POST, handleBrightness);
  server.on("/settime", HTTP_POST, handleSetTime);
  server.on("/turnoff", HTTP_POST, handleSetTurnOffTime);
  server.on("/synctime", HTTP_POST, handleSyncTime);
  server.on("/setrgb", HTTP_POST, handleSetRGB);
  server.begin();
}

void loop() {
  server.handleClient();
  bool motion = digitalRead(PIR_PIN) == HIGH;
  if (motion) lastMotionTime = millis();
  int lightPercent = ldr.readPercent();
  bool isDark = ldr.isDark();


  int potRaw = analogRead(PIN_POT);
  uint8_t pwmLevel = map(potRaw, 0, 4095, 0, 255);
  int btnState = digitalRead(PIN_BTN);
  if (btnState == LOW && lastBtnState == HIGH) {
    delay(40);
    if (digitalRead(PIN_BTN) == LOW)
      colorIndex = (colorIndex + 1) % 7;
  }

  lastBtnState = btnState;
  RtcDateTime now = Rtc.GetDateTime();
  int currentHour = now.Hour();
  int currentMinute = now.Minute();
  int currentSecond = now.Second();
  bool shouldTurnOff = (currentHour == dynamicTurnOffHour && currentMinute == dynamicTurnOffMinute);

  if (shouldTurnOff) {
    writeRgb(0, 0, 0);
    if (isRemoteOn) {
      isRemoteOn = false;  
    }
  }
  // שליטה מרחוק פועלת תמיד (אלא אם זה זמן כיבוי)
  // else if (isRemoteOn) {
  //   setColorByIndex(colorIndex, remoteBrightness);
  // }
  else if (isRemoteOn) {
  if (useCustomColor) {
    writeRgb(customR, customG, customB);
  } else {
    setColorByIndex(colorIndex, remoteBrightness);
  }
}

  else {
    bool keepOn = ((millis() - lastMotionTime) <= KEEP_ON_MS) && isDark;

    if (keepOn) {
      // חישוב עוצמת אור מותאמת
      uint8_t adaptiveBrightness;

      if (lightPercent >= 80) {
        adaptiveBrightness = map(lightPercent, 80, 100, 60, 30);
      } else if (lightPercent >= 60) {
        adaptiveBrightness = map(lightPercent, 60, 80, 100, 60);
      } else if (lightPercent >= 40) {
        adaptiveBrightness = map(lightPercent, 40, 60, 150, 100);
      } else if (lightPercent >= 20) {
        adaptiveBrightness = map(lightPercent, 20, 40, 200, 150);
      } else {
        adaptiveBrightness = map(lightPercent, 0, 20, 255, 200);
      }

      adaptiveBrightness = constrain(adaptiveBrightness, 30, 255);

      // בחירה בין שליטה ידנית לאוטומטית
      if (potRaw > 100) {
        setColorByIndex(colorIndex, pwmLevel);
      } else {
        setColorByIndex(colorIndex, adaptiveBrightness);
      }
    } else {
      writeRgb(0, 0, 0);
    }
  }


  static unsigned long lastLcdUpdate = 0;
  if (millis() - lastLcdUpdate > 1000) {
    // הצגת זמן ואור
    String timeStr = "";
    if (currentHour < 10) timeStr += "0";
    timeStr += String(currentHour) + ":";
    if (currentMinute < 10) timeStr += "0";
    timeStr += String(currentMinute);

    String line1 = timeStr + " Light:" + String(lightPercent) + "%";
    String line2 = "PWM:" + String(pwmLevel) + " Color:" + String(colorIndex);
    lcd.printMessage(line1 + "\n" + line2);
    lastLcdUpdate = millis();
  }

  delay(25);
}
