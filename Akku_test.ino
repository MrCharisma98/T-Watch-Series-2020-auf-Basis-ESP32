#include <Arduino.h>
#include <Wire.h>

#define LILYGO_WATCH_2020_V3
#include <LilyGoWatch.h>

#define AXP202_ADDR 0x35

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

uint16_t read12bit(uint8_t highReg, uint8_t lowReg) {
  return ((uint16_t)axpRead(highReg) << 4) | (axpRead(lowReg) & 0x0F);
}

float readVBAT_lowlevel() {           
  return read12bit(0x78, 0x79) * 1.1f / 1000.0f;
}

float readVBUS_lowlevel() {           
  return read12bit(0x5A, 0x5B) * 1.7f / 1000.0f;
}

void setup() {
  Serial.begin(115200);
  delay(800);

  Serial.println("\nAXP DIAG: start");

  ttgo = TTGOClass::getWatch();
  ttgo->begin();

  Wire.begin(21, 22);
  Wire.setClock(400000);

  
  axpWrite(0x82, 0xFF);
  delay(200);

  
  float vbus = readVBUS_lowlevel();
  float vbat = readVBAT_lowlevel();

  uint8_t s0 = axpRead(0x00);
  uint8_t s1 = axpRead(0x01);

  Serial.println("LOW-LEVEL:");
  Serial.printf("VBUS = %.2f V\n", vbus);
  Serial.printf("VBAT = %.2f V\n", vbat);
  Serial.printf("STATUS 0x00 = 0x%02X\n", s0);
  Serial.printf("STATUS 0x01 = 0x%02X\n", s1);

  
  Serial.println("\nLIBRARY:");
  Serial.printf("VBUS(getVbusVoltage) = %.2f V\n", ttgo->power->getVbusVoltage() / 1000.0);
  Serial.printf("VBAT(getBattVoltage) = %.2f V\n", ttgo->power->getBattVoltage() / 1000.0);

  
  Serial.printf("USB plugged (isVbusPlugIn) = %s\n", ttgo->power->isVbusPlugInIRQ() ? "YES" : "NO");
  Serial.printf("Charging (isCharging) = %s\n", ttgo->power->isChargeing() ? "YES" : "NO");

  Serial.println("\nAXP DIAG: done");
}

void loop() {}
