#pragma once

#include <Arduino.h>

// Waveshare ESP32-S3-Touch-LCD-1.28 pinout
inline constexpr int PIN_I2C_SDA = 6;
inline constexpr int PIN_I2C_SCL = 7;

inline constexpr int PIN_LCD_BL = 2;
inline constexpr int PIN_LCD_DC = 8;
inline constexpr int PIN_LCD_CS = 9;
inline constexpr int PIN_LCD_SCLK = 10;
inline constexpr int PIN_LCD_MOSI = 11;
inline constexpr int PIN_LCD_MISO = 12;
inline constexpr int PIN_LCD_RST = 14;

inline constexpr int PIN_TOUCH_INT = 5; //* shared with output solder headers! This is the input
inline constexpr int PIN_TOUCH_RST = 13;

inline constexpr int PIN_IMU_INT1 = 4; //* shared with output solder headers! This is the input
inline constexpr int PIN_IMU_INT2 = 3;

inline constexpr int PIN_BATT_ADC = 1;

inline constexpr int PIN_OUT_MOSFET1 = 4;  //* shared with input interrupts for IMU This is the output
inline constexpr int PIN_OUT_MOSFET2 = 5;   //* shared with input interrupts for touchscreen This is the output

//GPIO Connector --------
/* 
 https://files.waveshare.com/wiki/ESP32-S3-Touch-LCD-1.28/ESP32-S3-Touch-LCD-1.28-Sch.pdf
 pin 1  GND
 pin 2  VSYS  (VSYS is the system supply voltage that is toggled between battery ~3to4v ish and usb 5v.)
 pin 3  RUN/RESET
 pin 4  GPIO 0 BOOT
 pin 5  GND
 pin 6  3v3
*/
inline constexpr int PIN_GPIOCON_15 = 15; // pin 7
inline constexpr int PIN_GPIOCON_16 = 16; // pin 8
inline constexpr int PIN_GPIOCON_17 = 17; // pin 9
inline constexpr int PIN_GPIOCON_18 = 18; // pin 10
inline constexpr int PIN_GPIOCON_21 = 21; // pin 11
inline constexpr int PIN_GPIOCON_33 = 33; // pin 12

//-----------chip addresses
inline constexpr uint8_t CST816_ADDR = 0x15;
inline constexpr uint8_t QMI8658_ADDRS[] = {0x6A, 0x6B};

//---------- useful consts.
inline constexpr int LCD_CX = 120;
inline constexpr int LCD_CY = 120;
inline constexpr int OUTER_R = 119;
inline constexpr int INNER_R = 116; // 3-pixel ring: r=117..119


