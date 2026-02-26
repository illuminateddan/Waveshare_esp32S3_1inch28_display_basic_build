#pragma once

#include <Arduino.h>

struct TouchPoint {
  bool valid = false;
  bool touching = false;
  int x = 0;
  int y = 0;
  uint8_t gesture = 0;
};
