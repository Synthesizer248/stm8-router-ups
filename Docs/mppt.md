# MPPT Algorithm (Perturb and Observe)

Firmware uses a simple perturb-and-observe method suitable for low-power panels.

## Inputs

- Solar voltage (`Vsolar`)
- Charge current (`Ichg`)
- Computed power (`P = Vsolar * Ichg`)

If current sensing is disabled, current is estimated from PWM duty for fallback control.

## Update Rate

- MPPT update period: `600ms`

## Logic

1. Measure current solar power.
2. Compare with previous power.
3. If power dropped beyond deadband, reverse duty perturb direction.
4. Step PWM duty (`+/- MPPT_STEP_DUTY`).
5. Apply safety clamps:
- panel collapse limit
- battery over-voltage handling
- current limits

## Why This Works Here

- Panel power is only about `1W`.
- Controller resources are small (`STM8S103F3P6`).
- P&O gives acceptable energy tracking with minimal firmware overhead.

## Known Limitations

- Can oscillate around local operating point.
- Performance reduces under fast irradiance changes.
- Better accuracy requires stable current sensing and calibrated ADC channels.
