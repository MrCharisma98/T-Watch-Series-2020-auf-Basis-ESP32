#define LILYGO_WATCH_2020_V1

#include <LilyGoWatch.h>
#include <driver/i2s.h>
#include <math.h>
#include "hello_sample.h"
#include "thetimeis_sample.h"

#include <WiFi.h>
#include "time.h"

const char* ssid = "WLAN_NAME";
const char* password = "WLAN_PASSWORT";

const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 3600;
const int daylightOffset_sec = 3600;

TTGOClass *ttgo;

#define I2S_BCLK 26
#define I2S_LRC  25
#define I2S_DOUT 33

#define SAMPLE_RATE 16000
#define AMPLITUDE   1500

void setupWiFiTime() {
  Serial.println("Verbinde WLAN...");
  WiFi.begin(ssid, password);

  int tries = 0;
  while (WiFi.status() != WL_CONNECTED && tries < 30) {
    delay(500);
    Serial.print(".");
    tries++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWLAN verbunden");
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  } else {
    Serial.println("\nWLAN Fehler");
  }
}

void setupI2S() {
  i2s_driver_uninstall(I2S_NUM_0);

  i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
    .sample_rate = SAMPLE_RATE,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
    .communication_format = I2S_COMM_FORMAT_STAND_I2S,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 8,
    .dma_buf_len = 128,
    .use_apll = false,
    .tx_desc_auto_clear = true,
    .fixed_mclk = 0
  };

  i2s_pin_config_t pin_config = {
    .bck_io_num = I2S_BCLK,
    .ws_io_num = I2S_LRC,
    .data_out_num = I2S_DOUT,
    .data_in_num = I2S_PIN_NO_CHANGE
  };

  i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
  i2s_set_pin(I2S_NUM_0, &pin_config);
  i2s_zero_dma_buffer(I2S_NUM_0);
}

void playTone(int freq, int durationMs) {
  int16_t buffer[256 * 2];
  float phase = 0;
  float step = 2 * PI * freq / SAMPLE_RATE;
  int samples = SAMPLE_RATE * durationMs / 1000;

  for (int i = 0; i < samples; i += 256) {
    for (int j = 0; j < 256; j++) {
      int16_t s = sin(phase) * AMPLITUDE;
      phase += step;

      buffer[j * 2] = s;
      buffer[j * 2 + 1] = s;
    }

    size_t bytesWritten;
    i2s_write(I2S_NUM_0, buffer, sizeof(buffer), &bytesWritten, portMAX_DELAY);
  }
}

void playHelloRaw() {
  Serial.print("Array size: ");
  Serial.println(sizeof(hello_wav));

  const int headerSize = 44;
  int16_t buffer[256 * 2];

  for (unsigned int i = headerSize; i + 1 < sizeof(hello_wav); ) {
    for (int j = 0; j < 256; j++) {
      if (i + 1 < sizeof(hello_wav)) {
        int16_t s = (int16_t)(pgm_read_byte(&hello_wav[i]) |
                             (pgm_read_byte(&hello_wav[i + 1]) << 8));
        i += 2;

        s = s * 4;   

        buffer[j * 2] = s;
        buffer[j * 2 + 1] = s;
      } else {
        buffer[j * 2] = 0;
        buffer[j * 2 + 1] = 0;
      }
    }

    size_t bytesWritten;
    i2s_write(I2S_NUM_0, buffer, sizeof(buffer), &bytesWritten, portMAX_DELAY);
  }

  Serial.println("Hello fertig");
}

void playTimeRaw() {
  const int headerSize = 44;
  int16_t buffer[256 * 2];

  for (unsigned int i = headerSize; i + 1 < sizeof(thetimeis_wav); ) {
    for (int j = 0; j < 256; j++) {
      if (i + 1 < sizeof(thetimeis_wav)) {
        int16_t s = (int16_t)(pgm_read_byte(&thetimeis_wav[i]) |
                             (pgm_read_byte(&thetimeis_wav[i + 1]) << 8));
        i += 2;

        s = s * 4;

        buffer[j * 2] = s;
        buffer[j * 2 + 1] = s;
      } else {
        buffer[j * 2] = 0;
        buffer[j * 2 + 1] = 0;
      }
    }

    size_t bytesWritten;
    i2s_write(I2S_NUM_0, buffer, sizeof(buffer), &bytesWritten, portMAX_DELAY);
  }

  Serial.println("Time fertig");
}

void showCurrentTime() {
  struct tm timeinfo;

  if (!getLocalTime(&timeinfo)) {
    Serial.println("Zeit konnte nicht gelesen werden");
    ttgo->tft->fillScreen(TFT_BLACK);
    ttgo->tft->setCursor(20, 60);
    ttgo->tft->setTextSize(2);
    ttgo->tft->println("Keine Zeit");
    return;
  }

  char timeString[10];
  strftime(timeString, sizeof(timeString), "%H:%M", &timeinfo);

  Serial.print("Aktuelle Zeit: ");
  Serial.println(timeString);

  ttgo->tft->fillScreen(TFT_BLACK);
  ttgo->tft->setTextColor(TFT_WHITE, TFT_BLACK);
  ttgo->tft->setTextSize(2);
  ttgo->tft->setCursor(20, 40);
  ttgo->tft->println("THE TIME IS");

  ttgo->tft->setTextSize(4);
  ttgo->tft->setCursor(35, 100);
  ttgo->tft->println(timeString);
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("=== Hello Sample Test ===");

  ttgo = TTGOClass::getWatch();
  ttgo->begin();
  ttgo->openBL();

  ttgo->enableLDO3();
  delay(200);

  setupI2S();
  setupWiFiTime();

  ttgo->tft->fillScreen(TFT_BLACK);
  ttgo->tft->setTextColor(TFT_WHITE, TFT_BLACK);
  ttgo->tft->setTextSize(2);
  ttgo->tft->setCursor(20, 40);
  ttgo->tft->println("HELLO SAMPLE");

  playTone(1000, 150);   
  delay(700);

  playHelloRaw();
  delay(1000);
  showCurrentTime();
  playTimeRaw();      
}

int mode = 0;

void loop() {
  ttgo->power->readIRQ();

  if (ttgo->power->isPEKShortPressIRQ()) {
    mode++;

    if (mode > 1) {
      mode = 0;
    }

    if (mode == 0) {
      Serial.println("Button: HELLO WORLD");

      ttgo->tft->fillScreen(TFT_BLACK);
      ttgo->tft->setCursor(20, 60);
      ttgo->tft->setTextSize(3);
      ttgo->tft->println("HELLO");
      ttgo->tft->setCursor(20, 110);
      ttgo->tft->println("WORLD");

      playHelloRaw();
    }

    if (mode == 1) {
      Serial.println("Button: THE TIME IS");
      showCurrentTime();
      playTimeRaw();
    }

    ttgo->power->clearIRQ();
    delay(400);
  }

  delay(20);
}
