#include <Arduino.h>
#include <Wire.h>
#define LILYGO_WATCH_2020_V1
#include <LilyGoWatch.h>



#define AXP202_ADDR 0x35
#define BL_PIN 12
#define BL_CH  0

TTGOClass *ttgo;


uint8_t axpRead(uint8_t reg) {
  Wire.beginTransmission((uint8_t)AXP202_ADDR);
  Wire.write(reg);
  Wire.endTransmission(false);
  Wire.requestFrom((uint8_t)AXP202_ADDR, (uint8_t)1);
  return Wire.available() ? Wire.read() : 0xFF;
}

void axpWrite(uint8_t reg, uint8_t val) {
  Wire.beginTransmission((uint8_t)AXP202_ADDR);
  Wire.write(reg);
  Wire.write(val);
  Wire.endTransmission();
}

void enablePower() {
  uint8_t pwr = axpRead(0x12);
  pwr |= (1 << 4); 
  pwr |= (1 << 3); 
  pwr |= (1 << 2); 
  axpWrite(0x12, pwr);
}


void setup() {
  Serial.begin(115200);
  delay(500);

  
  ttgo = TTGOClass::getWatch();
  ttgo->begin();

  
  Wire.begin(21, 22);
  Wire.setClock(400000);
  enablePower();

  
  ledcSetup(BL_CH, 5000, 8);
  ledcAttachPin(BL_PIN, BL_CH);
  ledcWrite(BL_CH, 255); 

  
  ttgo->tft->init();
  ttgo->tft->setRotation(1);
  ttgo->tft->fillScreen(TFT_WHITE);
  ttgo->tft->setTextColor(TFT_BLACK);
  ttgo->tft->setCursor(20, 20);
  ttgo->tft->setTextSize(2);
  ttgo->tft->println("Display OK");
}

void loop() {}
