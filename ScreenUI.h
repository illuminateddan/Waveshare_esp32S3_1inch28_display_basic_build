#pragma once

#include <LovyanGFX.hpp>

#include "AppTypes.h"
#include "ImuQmi.h"

class LGFX : public lgfx::LGFX_Device {
  lgfx::Panel_GC9A01 _panel_instance;
  lgfx::Bus_SPI _bus_instance;

public:
  LGFX();
};

void screenInit();
void screenDrawStaticFrame();
void screenDrawCenterText(const TouchPoint& t, const ImuSample& imu, uint32_t stepCount, const QmiFeatureState& state);
void screenDrawSplashScreen();
