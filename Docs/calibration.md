# Calibration Procedure

## Equipment

- Adjustable bench supply (`0V-5V`)
- DMM
- Oscilloscope (recommended)
- Electronic load or resistor load

## 1) Voltage ADC Calibration

1. Disconnect battery and panel.
2. Feed known voltages into the solar divider input.
3. Read ADC-reported solar voltage.
4. Adjust:
- `ADC_REF_MV`
- `SOLAR_DIV_RTOP_OHM`, `SOLAR_DIV_RBOT_OHM`

Repeat for battery divider and:

- `BATT_DIV_RTOP_OHM`, `BATT_DIV_RBOT_OHM`

## 2) Current Calibration (if enabled)

1. Force a known charging current through shunt.
2. Compare measured current vs firmware reading.
3. Tune:
- `SHUNT_MILLIOHM`
- `CURRENT_SENSE_GAIN`

## 3) MPPT Step Tuning

Tune:

- `MPPT_STEP_DUTY`
- `MPPT_INTERVAL_MS`
- `MPPT_DEADBAND_MW`

Use stable sunlight or a programmable source to avoid false conclusions.

## 4) Charge Threshold Validation

Confirm transitions occur near:

- `3.2V` precharge exit
- `4.1V` CV entry
- `4.2V` done target
- `4.25V` over-voltage fault
