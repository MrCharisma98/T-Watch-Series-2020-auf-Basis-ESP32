#include <Arduino.h>

#define LILYGO_WATCH_2020_V1
#include <LilyGoWatch.h>

TTGOClass *ttgo;


int voltageToPercent(float v) {
  
  if (v <= 3.00f) return 0;
  if (v >= 4.20f) return 100;

  
  
  if (v < 3.30f) return (int)((v - 3.00f) / (0.30f) * 10.0f);

  
  if (v < 3.60f) return 10 + (int)((v - 3.30f) / (0.30f) * 40.0f);

  
  if (v < 3.90f) return 50 + (int)((v - 3.60f) / (0.30f) * 40.0f);

  
  return 90 + (int)((v - 3.90f) / (0.30f) * 10.0f);
}

void show(float vbat, float vbus, bool charging, int pct) {

  Serial.printf("VBAT=%.2f V  VBUS=%.2f V  CHG=%s  PCT=%d%%\n",vbat, vbus, charging ? "YES" : "NO", pct);

  
  ttgo->tft->fillScreen(TFT_BLACK);
  ttgo->tft->setTextColor(TFT_WHITE, TFT_BLACK);
  ttgo->tft->setTextSize(2);

  ttgo->tft->setCursor(10, 20);
  ttgo->tft->println("AKKU STATUS");

  ttgo->tft->setCursor(10, 60);
  ttgo->tft->printf("VBAT: %.2f V", vbat);

  ttgo->tft->setCursor(10, 90);
  ttgo->tft->printf("PCT : %d%%", pct);

  ttgo->tft->setCursor(10, 120);
  ttgo->tft->printf("VBUS: %.2f V", vbus);

  ttgo->tft->setCursor(10, 150);
  ttgo->tft->printf("CHG : %s", charging ? "YES" : "NO");
}

void setup() {
  Serial.begin(115200);
  delay(500);

  ttgo = TTGOClass::getWatch();
  ttgo->begin();

  ttgo->tft->init();
  ttgo->tft->setRotation(1);
}

void loop() {
  
  float vbat = ttgo->power->getBattVoltage() / 1000.0f;
  float vbus = ttgo->power->getVbusVoltage() / 1000.0f;

  
  bool charging = ttgo->power->isChargeing();

  int pct = voltageToPercent(vbat);

  show(vbat, vbus, charging, pct);

  delay(2000);
}

