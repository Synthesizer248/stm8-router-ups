# PCB Design Notes

## Board Targets

- Board size target: `50mm x 40mm`
- Layers: `2`
- Copper: `1 oz`
- Board thickness: `1.6mm`

## Placement Strategy

1. Keep power path short:
- `J1 -> D1 -> Q1/L1/D2 -> C2 -> J2`

2. Place high di/dt elements tight:
- `Q1`, `D2`, `L1`, `C1`

3. Keep MCU and analog sensing away from switching node:
- place `U1`, dividers, and current amplifier on quieter side

4. Route shunt Kelvin traces:
- separate high-current shunt path from ADC sense pair

## Trace Rules

- High-current path: `>= 1.0mm` preferred
- Signal traces: `0.25mm`
- Ground pour on both layers with stitching vias

## Switching Node Guidance

- Minimize copper area on `SW_NODE` to reduce EMI.
- Keep ADC lines away from `SW_NODE`.

## Connectors

- `J1`: solar panel input (`+/-`)
- `J2`: battery output (`+/-`)
- `J3`: SWIM header (`SWIM`, `NRST`, `VCC`, `GND`)

## Files

- Placement CSV: `hardware/pcb/component_placement.csv`
- Layout image: `hardware/pcb/smart_solar_lipo_charger_pcb.svg`
