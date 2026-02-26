#pragma once

#include <Arduino.h>

// Waveshare ESP32-S3-Touch-LCD-1.28 pinout
inline constexpr int PIN_LCD_BL = 2;
inline constexpr int PIN_LCD_DC = 8;
inline constexpr int PIN_LCD_CS = 9;
inline constexpr int PIN_LCD_SCLK = 10;
inline constexpr int PIN_LCD_MOSI = 11;
inline constexpr int PIN_LCD_MISO = 12;
inline constexpr int PIN_LCD_RST = 14;

inline constexpr int PIN_I2C_SDA = 6;
inline constexpr int PIN_I2C_SCL = 7;
inline constexpr int PIN_TOUCH_INT = 5;
inline constexpr int PIN_TOUCH_RST = 13;

inline constexpr int LCD_CX = 120;
inline constexpr int LCD_CY = 120;
inline constexpr int OUTER_R = 119;
inline constexpr int INNER_R = 116; // 3-pixel ring: r=117..119

inline constexpr uint8_t CST816_ADDR = 0x15;
inline constexpr uint8_t QMI8658_ADDRS[] = {0x6A, 0x6B};
