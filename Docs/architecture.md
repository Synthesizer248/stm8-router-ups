# System Architecture

## Goal

Charge a `1S Li-Po (3.7V nominal, 4.2V max)` from a `5V/200mA` solar panel with:

- safe Li-Po charging behavior
- PWM-controlled buck stage
- optional MPPT for better harvested energy

## High-Level Blocks

1. Solar front-end:
- input connector, reverse protection, bulk capacitor

2. Buck power stage:
- PWM switch (`AO3400`)
- inductor (`22uH`)
- freewheel diode (`SS14`)
- output capacitor (`470uF`)

3. Sensing:
- solar voltage divider -> STM8 ADC
- battery voltage divider -> STM8 ADC
- optional current shunt (`0.1 ohm`) + gain stage -> STM8 ADC

4. Controller:
- STM8S103F3P6 running STM8duino firmware
- charging state machine + MPPT logic

5. Programming:
- ST-LINK V2 via SWIM

## Control Structure

- Fast loop (`100ms`):
  - sample ADC channels
  - decide charge mode
  - adjust PWM duty
- Slow loop (`600ms`):
  - perturb-and-observe MPPT update

## Charge State Machine

- `IDLE`: no usable solar input
- `PRECHARGE`: battery below `3.2V`, current-limited low current
- `BULK`: normal current charging until ~`4.1V`
- `CV`: control around `4.2V`
- `DONE`: terminate when full and current falls low
- `FAULT`: over-voltage protection

## Limits Used in Firmware

- Precharge threshold: `3.2V`
- CV entry: `4.1V`
- Full voltage: `4.2V`
- Over-voltage cut: `4.25V`
- Re-charge threshold: `4.15V`
