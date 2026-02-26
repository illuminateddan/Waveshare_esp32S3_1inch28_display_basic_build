#pragma once

#include <Arduino.h>

struct ImuSample {
  bool valid = false;
  float ax = 0;
  float ay = 0;
  float az = 0;
  float gx = 0;
  float gy = 0;
  float gz = 0;
};

struct QmiFeatureConfig {
  float stepDeltaG = 0.18f;
  uint32_t stepMinIntervalMs = 260;

  bool womEnabled = true;
  float womThresholdG = 0.08f;

  bool tapEnabled = true;
  float tapThresholdG = 0.45f;
  uint32_t tapQuietMs = 160;

  bool anyMotionEnabled = true;
  float anyMotionThresholdG = 0.06f;

  bool noMotionEnabled = true;
  float noMotionThresholdG = 0.03f;
  uint32_t noMotionDurationMs = 1500;

  bool significantMotionEnabled = true;
  float significantMotionThresholdG = 0.35f;
  float significantGyroThresholdDps = 80.0f;
};

struct QmiFeatureState {
  uint32_t stepCount = 0;
  bool wakeOnMotion = false;
  bool tapDetected = false;
  bool anyMotion = false;
  bool noMotion = false;
  bool significantMotion = false;

  uint32_t lastStepMs = 0;
  uint32_t noMotionStartMs = 0;
  uint32_t tapCandidateMs = 0;
  bool tapCandidate = false;
};

namespace qmi {

bool imuInit();
ImuSample imuRead();
uint8_t qmiGetImuAddress();

void qmiPedometerConfigure(float stepDeltaG, uint32_t minStepIntervalMs);
uint32_t qmiPedometerReadStepCount();
void qmiPedometerResetStepCount();

void qmiWakeOnMotionConfigure(bool enabled, float womThresholdG);
void qmiTapConfigure(bool enabled, float tapThresholdG, uint32_t tapQuietMs);
void qmiAnyMotionConfigure(bool enabled, float thresholdG);
void qmiNoMotionConfigure(bool enabled, float thresholdG, uint32_t noMotionDurationMs);
void qmiSignificantMotionConfigure(bool enabled, float accThresholdG, float gyroThresholdDps);

void qmiProcessMotion(const ImuSample& s, uint32_t nowMs);
const QmiFeatureState& qmiGetFeatureState();

} // namespace qmi
