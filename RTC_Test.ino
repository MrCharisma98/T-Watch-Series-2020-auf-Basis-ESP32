#include <Arduino.h>

#define LILYGO_WATCH_2020_V1
#include <LilyGoWatch.h>

TTGOClass *ttgo;


int monthFromString(const char *m) {
  if (!strncmp(m, "Jan", 3)) return 1;
  if (!strncmp(m, "Feb", 3)) return 2;
  if (!strncmp(m, "Mar", 3)) return 3;
  if (!strncmp(m, "Apr", 3)) return 4;
  if (!strncmp(m, "May", 3)) return 5;
  if (!strncmp(m, "Jun", 3)) return 6;
  if (!strncmp(m, "Jul", 3)) return 7;
  if (!strncmp(m, "Aug", 3)) return 8;
  if (!strncmp(m, "Sep", 3)) return 9;
  if (!strncmp(m, "Oct", 3)) return 10;
  if (!strncmp(m, "Nov", 3)) return 11;
  if (!strncmp(m, "Dec", 3)) return 12;
  return 1;
}

RTC_Date buildRtcFromCompileTime() {

  const char *d = __DATE__;
  const char *t = __TIME__;

  RTC_Date r;
  r.month = monthFromString(d);


  char dayStr[3] = {0};
  dayStr[0] = d[4];
  dayStr[1] = d[5];
  r.day = atoi(dayStr);

  
  char yearStr[5] = {0};
  yearStr[0] = d[7];
  yearStr[1] = d[8];
  yearStr[2] = d[9];
  yearStr[3] = d[10];
  r.year = atoi(yearStr);

  
  r.hour   = (t[0] - '0') * 10 + (t[1] - '0');
  r.minute = (t[3] - '0') * 10 + (t[4] - '0');
  r.second = (t[6] - '0') * 10 + (t[7] - '0');

  return r;
}

bool rtcLooksInvalid(const RTC_Date &now) {
  
  if (now.year < 2024 || now.year > 2099) return true;
  if (now.month < 1 || now.month > 12) return true;
  if (now.day < 1 || now.day > 31) return true;
  if (now.hour > 23 || now.minute > 59 || now.second > 59) return true;
  return false;
}

void drawRtcScreen(const RTC_Date &now) {
  ttgo->tft->fillScreen(TFT_BLACK);
  ttgo->tft->setTextColor(TFT_WHITE, TFT_BLACK);

  ttgo->tft->setTextSize(2);
  ttgo->tft->setCursor(10, 20);
  ttgo->tft->println("RTC TIME");

  ttgo->tft->setCursor(10, 70);
  ttgo->tft->printf("%02d.%02d.%04d", now.day, now.month, now.year);

  ttgo->tft->setCursor(10, 110);
  ttgo->tft->printf("%02d:%02d:%02d", now.hour, now.minute, now.second);

  ttgo->tft->setTextSize(1);
  ttgo->tft->setCursor(10, 210);
  ttgo->tft->println("PEK short press: set to compile time");
  ttgo->tft->setTextSize(2);
}

void setup() {
  Serial.begin(115200);

  ttgo = TTGOClass::getWatch();
  ttgo->begin();
  ttgo->openBL();


  ttgo->rtc->check();


  ttgo->power->clearIRQ();


  RTC_Date now = ttgo->rtc->getDateTime();
  if (rtcLooksInvalid(now)) {
    RTC_Date compiled = buildRtcFromCompileTime();
    ttgo->rtc->setDateTime(compiled);
    now = compiled;

    Serial.println("RTC war unplausibel -> gesetzt auf Compile-Zeit:");
    Serial.printf("%02d.%02d.%04d %02d:%02d:%02d\n",now.day, now.month, now.year, now.hour, now.minute, now.second);
  }

  drawRtcScreen(now);
}

void loop() {

  ttgo->power->readIRQ();
  if (ttgo->power->isPEKShortPressIRQ()) {
    ttgo->power->clearIRQ();

    RTC_Date compiled = buildRtcFromCompileTime();
    ttgo->rtc->setDateTime(compiled);

    Serial.println("PEK: RTC gesetzt auf Compile-Zeit:");
    Serial.printf("%02d.%02d.%04d %02d:%02d:%02d\n",
                  compiled.day, compiled.month, compiled.year,
                  compiled.hour, compiled.minute, compiled.second);

    drawRtcScreen(compiled);
    delay(250);
    return;
  }
  ttgo->power->clearIRQ();

  static unsigned long last = 0;
  if (millis() - last >= 1000) {
    last = millis();

    RTC_Date now = ttgo->rtc->getDateTime();
    Serial.printf("%02d.%02d.%04d %02d:%02d:%02d\n",
                  now.day, now.month, now.year, now.hour, now.minute, now.second);

    drawRtcScreen(now);
  }
}
