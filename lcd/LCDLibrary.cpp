#include "LCDLibrary.h"

MyLCDLibrary::MyLCDLibrary(uint8_t lcd_addr, uint8_t lcd_cols, uint8_t lcd_rows)
  : lcd(lcd_addr, lcd_cols, lcd_rows) {}

void MyLCDLibrary::begin() {
  lcd.init();
  lcd.backlight();
}

void MyLCDLibrary::printMessage(const String &message) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(message);
}
