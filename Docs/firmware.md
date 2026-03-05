# Firmware Notes

Main sketch:

- `arduino_lipo_charger.ino`
- `firmware/stm8_solar_mppt_charger/stm8_solar_mppt_charger.ino`

## Pin Map (default)

| Function | STM8 Pin (default) | Notes |
|---|---|---|
| PWM gate drive | `PC6` (fallback `PD3`) | Buck MOSFET gate |
| Status LED | `PC5` | Mode indication |
| Solar ADC | `A0` (fallback `PB0`) | Divider midpoint |
| Battery ADC | `A1` (fallback `PB1`) | Divider midpoint |
| Current ADC | `A2` (fallback `PB2`) | Shunt amplifier output |

## Control Loops

- `CONTROL_INTERVAL_MS = 100`
- `MPPT_INTERVAL_MS = 600`

PWM duty range:

- `PWM_MIN_DUTY = 0`
- `PWM_MAX_DUTY = 220`

## Charging Parameters

- `BATT_PRECHARGE_MV = 3200`
- `BATT_CV_ENTRY_MV = 4100`
- `BATT_FULL_MV = 4200`
- `BATT_OVERVOLTAGE_MV = 4250`
- `PRECHARGE_TARGET_CURRENT_MA = 40`
- `BULK_TARGET_CURRENT_MA = 160`
- `TERMINATION_CURRENT_MA = 20`

## Optional Features

- Current sensing: set `ENABLE_CURRENT_SENSE` (`1` or `0`)
- UART logs: set `ENABLE_SERIAL_DEBUG` (`1` or `0`)

## Safety Behaviors

- Charger disables PWM if panel voltage is too low.
- Charger disables PWM in `DONE` and `FAULT`.
- Over-voltage forces `FAULT`.

## Calibration Inputs

If your ADC reference or divider values differ, update constants:

- `ADC_REF_MV`
- `SOLAR_DIV_RTOP_OHM`, `SOLAR_DIV_RBOT_OHM`
- `BATT_DIV_RTOP_OHM`, `BATT_DIV_RBOT_OHM`
- `SHUNT_MILLIOHM`, `CURRENT_SENSE_GAIN`
