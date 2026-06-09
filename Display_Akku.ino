#include <Arduino.h>
#include <Wire.h>

#define LILYGO_WATCH_2020_V1
#include <LilyGoWatch.h>


const uint8_t AXP202_ADDR = 0x35;

#define BL_PIN 12
#define BL_CH  0

TTGOClass *ttgo;


uint8_t axpRead(uint8_t reg) {
  Wire.beginTransmission(AXP202_ADDR);
  Wire.write(reg);
  Wire.endTransmission(false);


  Wire.requestFrom((uint8_t)AXP202_ADDR, (uint8_t)1);

  return Wire.available() ? Wire.read() : 0xFF;
}

void axpWrite(uint8_t reg, uint8_t val) {
  Wire.beginTransmission(AXP202_ADDR);
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
  ledcWrite(BL_CH, 200); 

  
  ttgo->tft->setRotation(1);
  ttgo->tft->fillScreen(TFT_BLACK);
  ttgo->tft->setTextSize(2);
  ttgo->tft->setTextColor(TFT_WHITE, TFT_BLACK);

  ttgo->tft->setCursor(10, 10);
  ttgo->tft->println("Systemintegration");
  ttgo->tft->setCursor(10, 28);
  ttgo->tft->println("Akku / Laden");
}


void loop() {
  
  float vbat = ttgo->power->getBattVoltage() / 1000.0;
  float vbus = ttgo->power->getVbusVoltage() / 1000.0;

  bool charging = ttgo->power->isChargeing();
  int pct = ttgo->power->getBattPercentage();

  
  Serial.print("VBAT=");
  Serial.print(vbat, 2);
  Serial.print(" V  VBUS=");
  Serial.print(vbus, 2);
  Serial.print(" V  CHG=");
  Serial.print(charging ? "YES" : "NO");
  Serial.print("  PCT=");
  Serial.print(pct);
  Serial.println(" %");

  

  ttgo->tft->fillRect(0, 55, 240, 120, TFT_BLACK);
  ttgo->tft->setCursor(10, 55);

  ttgo->tft->printf("VBAT: %.2f V\n", vbat);
  ttgo->tft->printf("VBUS: %.2f V\n", vbus);
  ttgo->tft->printf("CHG : %s\n", charging ? "YES" : "NO");
  ttgo->tft->printf("PCT : %d %%\n", pct);

  delay(1000);
}
