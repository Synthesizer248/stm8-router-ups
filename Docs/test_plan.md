# Validation Test Plan

## Safety

- Use protected Li-Po or a battery emulator.
- Keep a thermal camera or probe available for first power-up.

## Test Matrix

1. No panel, battery connected:
- expected mode: `IDLE`
- PWM off

2. Panel present, battery below `3.2V`:
- expected mode: `PRECHARGE`
- low charge current

3. Panel present, battery `3.2V-4.1V`:
- expected mode: `BULK`
- higher current charge

4. Panel present, battery near `4.2V`:
- expected mode: `CV` then `DONE`

5. Battery over-voltage injection:
- expected mode: `FAULT`
- PWM forced off

6. Irradiance step changes:
- verify MPPT converges and does not collapse panel excessively

## Measurements to Capture

- `Vsolar`, `Vbatt`, `Ichg`, PWM duty, temperature
- transition points and recovery behavior

## Pass Criteria

- No battery over-voltage under normal operation
- Correct mode transitions at threshold bands
- Stable behavior for at least 30 minutes continuous run
