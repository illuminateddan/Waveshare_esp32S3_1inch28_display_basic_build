#include "ScreenUI.h"

#include "BoardPins.h"

LGFX lcd;

LGFX::LGFX() {
  auto bus_cfg = _bus_instance.config();
  bus_cfg.spi_host = SPI2_HOST;
  bus_cfg.spi_mode = 0;
  bus_cfg.freq_write = 40000000;
  bus_cfg.freq_read = 16000000;
  bus_cfg.spi_3wire = false;
  bus_cfg.use_lock = true;
  bus_cfg.dma_channel = SPI_DMA_CH_AUTO;
  bus_cfg.pin_sclk = PIN_LCD_SCLK;
  bus_cfg.pin_mosi = PIN_LCD_MOSI;
  bus_cfg.pin_miso = PIN_LCD_MISO;
  bus_cfg.pin_dc = PIN_LCD_DC;
  _bus_instance.config(bus_cfg);
  _panel_instance.setBus(&_bus_instance);

  auto panel_cfg = _panel_instance.config();
  panel_cfg.pin_cs = PIN_LCD_CS;
  panel_cfg.pin_rst = PIN_LCD_RST;
  panel_cfg.pin_busy = -1;
  panel_cfg.memory_width = 240;
  panel_cfg.memory_height = 240;
  panel_cfg.panel_width = 240;
  panel_cfg.panel_height = 240;
  panel_cfg.offset_x = 0;
  panel_cfg.offset_y = 0;
  panel_cfg.offset_rotation = 0;
  panel_cfg.readable = false;
  panel_cfg.invert = true;
  panel_cfg.rgb_order = false;
  panel_cfg.dlen_16bit = false;
  panel_cfg.bus_shared = false;
  _panel_instance.config(panel_cfg);

  setPanel(&_panel_instance);
}

void screenInit() {
  lcd.init();
  lcd.setRotation(0);
}

void screenDrawStaticFrame() {
  lcd.fillScreen(TFT_BLACK);
  lcd.fillCircle(LCD_CX, LCD_CY, OUTER_R, TFT_CYAN);
  lcd.fillCircle(LCD_CX, LCD_CY, INNER_R, TFT_BLACK);
}

void screenDrawCenterText(const TouchPoint& t, const ImuSample& imu, uint32_t stepCount, const QmiFeatureState& state) {
  lcd.fillCircle(LCD_CX, LCD_CY, 102, TFT_BLACK);
  //lcd.fillCircle(LCD_CX, LCD_CY, OUTER_R, TFT_CYAN);
  lcd.fillCircle(LCD_CX, LCD_CY, INNER_R, TFT_BLACK);

  lcd.setTextColor(TFT_WHITE, TFT_BLACK);
  lcd.setFont(&fonts::Font2); //default BITMAP fonts (fast): Font0, Font2,4,6,7,8
  lcd.setTextSize(1); // 1x font scaling (multiplies bitmap size)
  lcd.setTextDatum(textdatum_t::middle_center); 

  int y = 78;
  const int lineStep = 14;

  lcd.drawString(String("TP:") + (t.touching ? "1" : "0") + " X:" + t.x + " Y:" + t.y, LCD_CX, y); y += lineStep;
  lcd.drawString(String("GST:") + t.gesture, LCD_CX, y); y += lineStep;

  if (imu.valid) {
    lcd.drawString(String("AX ") + String(imu.ax, 2) + " AY " + String(imu.ay, 2), LCD_CX, y); y += lineStep;
    lcd.drawString(String("AZ ") + String(imu.az, 2), LCD_CX, y); y += lineStep;
    lcd.drawString(String("GX ") + String(imu.gx, 1) + " GY " + String(imu.gy, 1), LCD_CX, y); y += lineStep;
    lcd.drawString(String("GZ ") + String(imu.gz, 1), LCD_CX, y); y += lineStep;
  } else {
    lcd.drawString("IMU READ FAIL", LCD_CX, y); y += lineStep;
  }

  lcd.drawString(String("STEP ") + stepCount, LCD_CX, y); y += lineStep;

  String flags = "M:";
  flags += state.wakeOnMotion ? "W" : "-";
  flags += state.tapDetected ? "T" : "-";
  flags += state.anyMotion ? "A" : "-";
  flags += state.noMotion ? "N" : "-";
  flags += state.significantMotion ? "S" : "-";
  lcd.drawString(flags, LCD_CX, y);
}


void screenDrawSplashScreen() {
  lcd.fillScreen(TFT_BLACK);
  lcd.fillCircle(LCD_CX, LCD_CY, 80, TFT_CYAN);
  lcd.fillCircle(LCD_CX, LCD_CY, 70, TFT_BLACK);
  lcd.fillCircle(LCD_CX, LCD_CY, 60, TFT_ORANGE);
  lcd.fillCircle(LCD_CX, LCD_CY, 50, TFT_BLACK);
  lcd.setFont(&fonts:: Orbitron_Light_24);
  lcd.setTextDatum(middle_center);
  lcd.drawString("I did a thing...", LCD_CX, 120);
}
