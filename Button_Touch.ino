#include <Arduino.h>
#define LILYGO_WATCH_2020_V1
#include <LilyGoWatch.h>

TTGOClass *ttgo;

void setup() {
  Serial.begin(115200);
  delay(500);

  ttgo = TTGOClass::getWatch();
  ttgo->begin();

  
  ttgo->tft->fillScreen(TFT_BLACK);
  ttgo->tft->setTextColor(TFT_WHITE, TFT_BLACK);
  ttgo->tft->setTextSize(2);
  ttgo->tft->setCursor(10, 10);
  ttgo->tft->println("INPUT TEST");
  ttgo->tft->setCursor(10, 40);
  ttgo->tft->println("Button + Touch");

  
  ttgo->power->enableIRQ(AXP202_PEK_SHORTPRESS_IRQ, true);
  ttgo->power->clearIRQ();
}

void loop() {
  
  ttgo->power->readIRQ();
  if (ttgo->power->isPEKShortPressIRQ()) {
    Serial.println("BUTTON PRESSED");

    ttgo->tft->fillRect(0, 80, 240, 30, TFT_BLACK);
    ttgo->tft->setCursor(10, 80);
    ttgo->tft->println("BUTTON OK");

    ttgo->power->clearIRQ();
    delay(300);
  }

  
  int16_t x, y;
  if (ttgo->getTouch(x, y)) {
    Serial.printf("TOUCH: X=%d Y=%d\n", x, y);
    ttgo->tft->fillCircle(x, y, 3, TFT_GREEN);
    delay(30);
  }
}
