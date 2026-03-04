# STM8S Router UPS (VS Code + SDCC)

This is a minimal STM8S103F3 project for a router UPS controller.

## Features
- Detect mains adapter presence (digital input)
- Measure battery voltage (ADC)
- Enable/disable boost converter (GPIO)
- Low-battery cutoff protection with hysteresis
- Status LED + buzzer alert

## Target
- MCU: STM8S103F3P6
- Toolchain: SDCC
- Flasher: stm8flash (with ST-Link)

## Pin Map
- `PB4` : Mains present input (HIGH = adapter present)
- `PD3` : Boost enable output
- `PD4` : Buzzer output
- `PC5` : Status LED output
- `AIN2 (PD2)` : Battery voltage sense (via 100k/33k divider)

## Hardware Notes
- Use a 2S Li-ion pack with a proper 2S BMS.
- Use a dedicated 8.4V CC/CV charger module.
- Use a 12V boost converter for backup rail.
- Divider for battery sense: 100k (top) + 33k (bottom).

## VS Code Usage
- `Terminal -> Run Task -> Build (SDCC)`
- `Terminal -> Run Task -> Flash (ST-Link)`
- `Terminal -> Run Task -> Build + Flash`

## CLI Usage (PowerShell)
```powershell
./scripts_build.ps1
./scripts_flash.ps1
```

## Optional Makefile Usage
```powershell
make
make flash
make clean
```

## Adjustments
Edit thresholds in `inc/config.h`:
- `BATT_CUTOFF_MV`
- `BATT_RECOVER_MV`
- `MAIN_LOOP_DELAY_MS`

If your board differs (STM8S003 etc.), update `MCU` in `Makefile` and flash target accordingly.
