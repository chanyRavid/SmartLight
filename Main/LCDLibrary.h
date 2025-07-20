#ifndef LCD_LIBRARY_H
#define LCD_LIBRARY_H

#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

class MyLCDLibrary {
  public:
    MyLCDLibrary(uint8_t lcd_addr, uint8_t lcd_cols, uint8_t lcd_rows);
    void begin();
    void printMessage(const String &message);
  private:
    LiquidCrystal_I2C lcd;
};

#endif
