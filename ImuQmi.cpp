#include "ImuQmi.h"

#include <Wire.h>
#include <math.h>

#include "BoardPins.h"

static QmiFeatureConfig qmiCfg;
static QmiFeatureState qmiState;
static uint8_t imuAddr = 0;

static bool i2cWrite8(uint8_t addr, uint8_t reg, uint8_t val) {
  Wire.beginTransmission(addr);
  Wire.write(reg);
  Wire.write(val);
  return Wire.endTransmission() == 0;
}

static bool i2cRead(uint8_t addr, uint8_t reg, uint8_t* data, uint8_t len) {
  Wire.beginTransmission(addr);
  Wire.write(reg);
  if (Wire.endTransmission(false) != 0) return false;
  if (Wire.requestFrom(addr, len) != len) return false;
  for (uint8_t i = 0; i < len; ++i) data[i] = Wire.read();
  return true;
}

namespace qmi {

bool imuInit() {
  imuAddr = 0;
  for (uint8_t addr : QMI8658_ADDRS) {
    uint8_t who = 0;
    if (i2cRead(addr, 0x00, &who, 1) && who == 0x05) {
      imuAddr = addr;
      break;
    }
  }
  if (!imuAddr) return false;

  if (!i2cWrite8(imuAddr, 0x02, 0x60)) return false;
  if (!i2cWrite8(imuAddr, 0x03, 0x25)) return false;
  if (!i2cWrite8(imuAddr, 0x04, 0x45)) return false;
  if (!i2cWrite8(imuAddr, 0x06, 0x00)) return false;
  if (!i2cWrite8(imuAddr, 0x08, 0x03)) return false;

  uint8_t c7 = 0;
  return i2cRead(imuAddr, 0x08, &c7, 1) && ((c7 & 0x03) == 0x03);
}

ImuSample imuRead() {
  ImuSample s;
  if (!imuAddr) return s;

  uint8_t raw[12] = {0};
  if (!i2cRead(imuAddr, 0x35, raw, 12)) return s;

  int16_t axRaw = (int16_t)((raw[1] << 8) | raw[0]);
  int16_t ayRaw = (int16_t)((raw[3] << 8) | raw[2]);
  int16_t azRaw = (int16_t)((raw[5] << 8) | raw[4]);
  int16_t gxRaw = (int16_t)((raw[7] << 8) | raw[6]);
  int16_t gyRaw = (int16_t)((raw[9] << 8) | raw[8]);
  int16_t gzRaw = (int16_t)((raw[11] << 8) | raw[10]);

  s.ax = axRaw / 4096.0f;
  s.ay = ayRaw / 4096.0f;
  s.az = azRaw / 4096.0f;
  s.gx = gxRaw / 64.0f;
  s.gy = gyRaw / 64.0f;
  s.gz = gzRaw / 64.0f;
  s.valid = true;
  return s;
}

uint8_t qmiGetImuAddress() {
  return imuAddr;
}

void qmiPedometerConfigure(float stepDeltaG, uint32_t minStepIntervalMs) {
  qmiCfg.stepDeltaG = stepDeltaG;
  qmiCfg.stepMinIntervalMs = minStepIntervalMs;
}

uint32_t qmiPedometerReadStepCount() {
  return qmiState.stepCount;
}

void qmiPedometerResetStepCount() {
  qmiState.stepCount = 0;
  qmiState.lastStepMs = 0;
}

void qmiWakeOnMotionConfigure(bool enabled, float womThresholdG) {
  qmiCfg.womEnabled = enabled;
  qmiCfg.womThresholdG = womThresholdG;
}

void qmiTapConfigure(bool enabled, float tapThresholdG, uint32_t tapQuietMs) {
  qmiCfg.tapEnabled = enabled;
  qmiCfg.tapThresholdG = tapThresholdG;
  qmiCfg.tapQuietMs = tapQuietMs;
}

void qmiAnyMotionConfigure(bool enabled, float thresholdG) {
  qmiCfg.anyMotionEnabled = enabled;
  qmiCfg.anyMotionThresholdG = thresholdG;
}

void qmiNoMotionConfigure(bool enabled, float thresholdG, uint32_t noMotionDurationMs) {
  qmiCfg.noMotionEnabled = enabled;
  qmiCfg.noMotionThresholdG = thresholdG;
  qmiCfg.noMotionDurationMs = noMotionDurationMs;
}

void qmiSignificantMotionConfigure(bool enabled, float accThresholdG, float gyroThresholdDps) {
  qmiCfg.significantMotionEnabled = enabled;
  qmiCfg.significantMotionThresholdG = accThresholdG;
  qmiCfg.significantGyroThresholdDps = gyroThresholdDps;
}

void qmiProcessMotion(const ImuSample& s, uint32_t nowMs) {
  qmiState.wakeOnMotion = false;
  qmiState.tapDetected = false;
  qmiState.anyMotion = false;
  qmiState.significantMotion = false;

  if (!s.valid) return;

  const float accMag = sqrtf(s.ax * s.ax + s.ay * s.ay + s.az * s.az);
  const float delta = fabsf(accMag - 1.0f);
  const float gyroMag = sqrtf(s.gx * s.gx + s.gy * s.gy + s.gz * s.gz);

  if (delta > qmiCfg.stepDeltaG && (nowMs - qmiState.lastStepMs) > qmiCfg.stepMinIntervalMs) {
    qmiState.stepCount++;
    qmiState.lastStepMs = nowMs;
  }

  if (qmiCfg.womEnabled && delta > qmiCfg.womThresholdG) {
    qmiState.wakeOnMotion = true;
  }

  if (qmiCfg.tapEnabled) {
    if (!qmiState.tapCandidate && delta > qmiCfg.tapThresholdG) {
      qmiState.tapCandidate = true;
      qmiState.tapCandidateMs = nowMs;
    }
    if (qmiState.tapCandidate) {
      if (delta < (qmiCfg.tapThresholdG * 0.45f) && (nowMs - qmiState.tapCandidateMs) <= qmiCfg.tapQuietMs) {
        qmiState.tapDetected = true;
        qmiState.tapCandidate = false;
      } else if ((nowMs - qmiState.tapCandidateMs) > qmiCfg.tapQuietMs) {
        qmiState.tapCandidate = false;
      }
    }
  }

  if (qmiCfg.anyMotionEnabled && delta > qmiCfg.anyMotionThresholdG) {
    qmiState.anyMotion = true;
  }

  if (qmiCfg.noMotionEnabled) {
    if (delta < qmiCfg.noMotionThresholdG) {
      if (qmiState.noMotionStartMs == 0) qmiState.noMotionStartMs = nowMs;
      qmiState.noMotion = (nowMs - qmiState.noMotionStartMs) >= qmiCfg.noMotionDurationMs;
    } else {
      qmiState.noMotionStartMs = 0;
      qmiState.noMotion = false;
    }
  }

  if (qmiCfg.significantMotionEnabled &&
      (delta > qmiCfg.significantMotionThresholdG || gyroMag > qmiCfg.significantGyroThresholdDps)) {
    qmiState.significantMotion = true;
  }
}

const QmiFeatureState& qmiGetFeatureState() {
  return qmiState;
}

} // namespace qmi
