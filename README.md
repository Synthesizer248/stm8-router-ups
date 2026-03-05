# STM8 Router UPS (Arduino IDE + ST-Link V2)

This repo now uses Arduino IDE workflow for STM8S boards.

## Firmware File

- Main sketch: `arduino_lipo_charger.ino`

## Features

- Mains adapter detection
- Battery voltage monitoring with ADC divider
- Boost enable control for backup mode
- Low-battery cutoff with hysteresis
- Status LED and buzzer alert

## Hardware Assumption

- MCU: STM8S103F3 (or compatible STM8S board)
- Programmer: ST-Link V2
- Power stage: 2S Li-ion + 2S BMS + 8.4V charger + 12V boost

## Arduino IDE Setup

1. Install Arduino IDE 2.x.
2. Open `File -> Preferences`.
3. Add this Boards Manager URL:
   `https://raw.githubusercontent.com/tenbaht/sduino/master/package_sduino_stm8_index.json`
4. Open `Tools -> Board -> Boards Manager`, install STM8 core package.
5. Open sketch file `arduino_lipo_charger.ino`.
6. In `Tools`, select your STM8 board (example: `STM8S103F3P6`).
7. Select programmer as ST-Link V2 (name may appear as `STLink`, `STLinkV2`, or similar).
8. Use `Sketch -> Upload Using Programmer` (Ctrl+Shift+U).

## ST-Link Wiring

- ST-Link `SWIM` -> STM8 `SWIM`
- ST-Link `GND` -> STM8 `GND`
- ST-Link `3V3/5V` -> target VCC (as required by board)
- Optional: ST-Link `RST` -> STM8 `NRST`

## Pin Mapping In Sketch

- `PB4`: mains detect input
- `PD3`: boost enable / Q3 gate driver control
- `PD4`: buzzer output
- `PC5`: status LED output
- `A2` or `PD2`: battery ADC input

If your board mapping differs, edit pin constants at the top of `arduino_lipo_charger.ino`.

## Threshold Tuning

Edit values in sketch:

- `BATT_CUTOFF_MV` (default `6400`)
- `BATT_RECOVER_MV` (default `7000`)
- `LOOP_DELAY_MS`
