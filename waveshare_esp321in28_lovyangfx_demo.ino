#include <Arduino.h>
#include <Wire.h>
#include <Preferences.h>

#include "BoardPins.h"
#include "AppTypes.h"
#include "ImuQmi.h"
#include "ScreenUI.h"

Preferences prefs;
uint32_t bootCount = 0;
uint32_t lastUpdateMs = 0;

bool i2cRead(uint8_t addr, uint8_t reg, uint8_t* data, uint8_t len) {
  Wire.beginTransmission(addr);
  Wire.write(reg);
  if (Wire.endTransmission(false) != 0) return false;
  if (Wire.requestFrom(addr, len) != len) return false;
  for (uint8_t i = 0; i < len; ++i) data[i] = Wire.read();
  return true;
}

bool touchInit() {
  pinMode(PIN_TOUCH_RST, OUTPUT);
  pinMode(PIN_TOUCH_INT, INPUT_PULLUP);
  digitalWrite(PIN_TOUCH_RST, LOW);
  delay(5);
  digitalWrite(PIN_TOUCH_RST, HIGH);
  delay(50);

  uint8_t fw = 0;
  return i2cRead(CST816_ADDR, 0x15, &fw, 1);
}

TouchPoint touchRead() {
  TouchPoint t;
  uint8_t raw[6] = {0};
  if (!i2cRead(CST816_ADDR, 0x01, raw, 6)) return t;

  t.valid = true;
  t.gesture = raw[0];
  t.touching = raw[1] > 0;
  t.x = ((raw[2] & 0x0F) << 8) | raw[3];
  t.y = ((raw[4] & 0x0F) << 8) | raw[5];
  return t;
}

void setup() {
  Serial.begin(115200);
  delay(200);

  pinMode(PIN_LCD_BL, OUTPUT);
  digitalWrite(PIN_LCD_BL, HIGH);

  Wire.begin(PIN_I2C_SDA, PIN_I2C_SCL, 400000);

  prefs.begin("ws-lgfx", false);
  bootCount = prefs.getUInt("boots", 0) + 1;
  prefs.putUInt("boots", bootCount);

  screenInit();
  screenDrawStaticFrame();

  const bool tOk = touchInit();
  const bool iOk = qmi::imuInit();

  // IMU feature layer configuration examples
  qmi::qmiPedometerConfigure(0.18f, 260); //float stepDeltaG, uint32_t minStepIntervalMs
  qmi::qmiWakeOnMotionConfigure(true, 0.08f); //bool enabled, float womThresholdG
  qmi::qmiTapConfigure(true, 0.45f, 160); //bool enabled, float tapThresholdG, uint32_t tapQuietMs
  qmi::qmiAnyMotionConfigure(true, 0.06f); //bool enabled, float thresholdG
  qmi::qmiNoMotionConfigure(true, 0.03f, 1500); //bool enabled, float thresholdG, uint32_t noMotionDurationMs
  qmi::qmiSignificantMotionConfigure(true, 0.35f, 80.0f); //bool enabled, float accThresholdG, float gyroThresholdDps

  Serial.println("=== Waveshare ESP32-S3 LovyanGFX demo ===");
  Serial.printf("Touch: %s\n", tOk ? "OK" : "FAIL");
  Serial.printf("IMU: %s", iOk ? "OK" : "FAIL");
  if (iOk) Serial.printf(" @0x%02X", qmi::qmiGetImuAddress());
  Serial.printf(" Boot:%u\n", bootCount);

  screenDrawSplashScreen();
  delay(2000);
}

void loop() {
  if (millis() - lastUpdateMs < 100) {
    delay(5);
    return;
  }
  lastUpdateMs = millis();

  TouchPoint t = touchRead();
  ImuSample imu = qmi::imuRead();

  qmi::qmiProcessMotion(imu, millis());
  const QmiFeatureState& state = qmi::qmiGetFeatureState();
  const uint32_t steps = qmi::qmiPedometerReadStepCount();


  screenDrawCenterText(t, imu, steps, state);

// font cheat sheet tmp
/*
  lcd.setTextColor(TFT_WHITE, TFT_BLACK);
  lcd.setFont(&fonts::Font0); //default BITMAP fonts (fast): Font0..2,4,6,7, to Font8 exposed by default in lovyangfx
  //OR
  lcd.setFont(&fonts::lgfxJapanGothic_24); // proportion, fixed size, smooth, slower.
  // Other fonts sizes: 8,12,16,20,24,28,32,36,40,
   lgfxJapanMincho_xx
   lgfxJapanMinchoP_xx
   lgfxJapanGothic_xx
   lgfxJapanGothicP_xx
  and other gfxFonts in limited sizes:
  Orbitron_Light_24 & 32
  Roboto_Thin_24
  Satisfy_24
  Yellowtail_32
  DejaVu9 & 12,18,24,40...
  
  lcd.setTextSize(1); // 1x font scaling (multiplies bitmap size)
  lcd.setTextDatum(textdatum_t::middle_center); 
*/


  Serial.print("TP="); Serial.print(t.touching ? 1 : 0);
  Serial.print(" X="); Serial.print(t.x);
  Serial.print(" Y="); Serial.print(t.y);
  Serial.print(" G="); Serial.print(t.gesture);

  if (imu.valid) {
    Serial.print("\tACC[g]=");
    Serial.print(String(imu.ax, 3)); Serial.print(",");
    Serial.print(String(imu.ay, 3)); Serial.print(",");
    Serial.print(String(imu.az, 3));
    Serial.print("\tGYR[dps]=");
    Serial.print(String(imu.gx, 2)); Serial.print(",");
    Serial.print(String(imu.gy, 2)); Serial.print(",");
    Serial.print(String(imu.gz, 2));
  } else {
    Serial.print(" IMU_READ_FAIL");
  }

  Serial.print("\tSTEP="); Serial.print(steps);
  Serial.print(" EVT[");
  Serial.print(state.wakeOnMotion ? "W" : "-");
  Serial.print(state.tapDetected ? "T" : "-");
  Serial.print(state.anyMotion ? "A" : "-");
  Serial.print(state.noMotion ? "N" : "-");
  Serial.print(state.significantMotion ? "S" : "-");
  Serial.println("]");
}
