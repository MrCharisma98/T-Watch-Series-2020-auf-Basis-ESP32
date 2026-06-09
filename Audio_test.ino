#define LILYGO_WATCH_2020_V1

#include <LilyGoWatch.h>
#include <driver/i2s.h>
#include <math.h>

TTGOClass *ttgo;


#define I2S_BCLK 26
#define I2S_LRC  25
#define I2S_DOUT 33

#define SAMPLE_RATE 16000
#define AMPLITUDE   1500   

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

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("=== Leiser Lautsprecher Test ===");

  ttgo = TTGOClass::getWatch();
  ttgo->begin();
  ttgo->openBL();

  ttgo->enableLDO3();   
  delay(200);

  setupI2S();
}

void loop() {
  playTone(1000, 150);  
  delay(1500);
}