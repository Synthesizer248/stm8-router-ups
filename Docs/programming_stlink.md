# STM8 Programming Guide (ST-LINK V2 + SWIM)

## Required Tools

- Arduino IDE 2.x
- ST-LINK V2
- STM8duino core (`sduino`)

## Install STM8duino Core

1. Open Arduino IDE -> `File -> Preferences`.
2. Add:
   `https://raw.githubusercontent.com/tenbaht/sduino/master/package_sduino_stm8_index.json`
3. Install STM8 core from Boards Manager.

## Target Selection

- Board: STM8S103 class board matching `STM8S103F3P6`
- Programmer: ST-LINK V2 / STLink

## Wiring

- ST-LINK `SWIM` -> STM8 `SWIM`
- ST-LINK `GND` -> STM8 `GND`
- ST-LINK `NRST` -> STM8 `NRST` (recommended)
- ST-LINK power pin -> target VCC only if needed by your setup

## Flashing

1. Open sketch `arduino_lipo_charger.ino`.
2. Verify board and programmer settings.
3. Use `Upload Using Programmer`.

## First Boot Checks

- LED mode indication changes with light/battery conditions.
- PWM output appears on gate pin (scope recommended).
- ADC channels report sane values in debug mode.
