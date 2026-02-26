# waveshare_esp321in28_lovyangfx_demo

LovyanGFX + custom CST816S + QMI8658 demo for Waveshare ESP32-S3 Touch LCD 1.28.

## File layout
- `waveshare_esp321in28_lovyangfx_demo.ino`: app loop, touch read, serial telemetry, feature wiring.
- `BoardPins.h`: all board pin/address constants.
- `AppTypes.h`: shared touch data struct.
- `ScreenUI.h/.cpp`: all LCD/screen functions and LovyanGFX panel setup.
- `ImuQmi.h/.cpp`: all IMU/QMI8658 functions + motion feature layer.

## Concise function annotations
### Screen (`ScreenUI`)
- `screenInit()` — initialize GC9A01 panel and rotation.
- `screenDrawStaticFrame()` — draw the circular 3px outer ring frame.
- `screenDrawCenterText(...)` — render touch/IMU/step/motion flags text in center.

### IMU core (`ImuQmi`)
- `qmi::imuInit()` — probe/init QMI8658 (0x6A/0x6B) and enable accel+gyro.
- `qmi::imuRead()` — burst-read accel+gyro XYZ and convert to g/dps.
- `qmi::qmiGetImuAddress()` — return active detected IMU I2C address.

### Pedometer / motion APIs (`ImuQmi`)
- `qmi::qmiPedometerConfigure(...)` — set step sensitivity + refractory timing.
- `qmi::qmiPedometerReadStepCount()` — read current software step counter.
- `qmi::qmiPedometerResetStepCount()` — reset step counter.
- `qmi::qmiWakeOnMotionConfigure(...)` — set WoM enable + threshold.
- `qmi::qmiTapConfigure(...)` — set tap detection enable + threshold/timing.
- `qmi::qmiAnyMotionConfigure(...)` — set any-motion enable + threshold.
- `qmi::qmiNoMotionConfigure(...)` — set no-motion enable + threshold/duration.
- `qmi::qmiSignificantMotionConfigure(...)` — set significant-motion accel/gyro thresholds.
- `qmi::qmiProcessMotion(...)` — evaluate one IMU sample across all motion engines.
- `qmi::qmiGetFeatureState()` — read latest motion-event flags.

## Dependency
- Install `LovyanGFX` library in Arduino IDE / PlatformIO.

## Pins
- LCD: DC=8, CS=9, SCLK=10, MOSI=11, MISO=12, RST=14, BL=2
- I2C: SDA=6, SCL=7
- Touch: INT=5, RST=13


## Build troubleshooting
- If you previously compiled an older revision of this sketch and see `multiple definition` linker errors, do a clean rebuild (close/reopen IDE or delete the Arduino temporary build folder) and ensure only one copy of `waveshare_esp321in28_lovyangfx_demo.ino` exists in the sketch folder.
