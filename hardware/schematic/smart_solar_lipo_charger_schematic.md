# Smart Solar Li-Po Charger Schematic

## Topology

Input (`5V solar`) feeds an MCU-controlled buck charging stage:

- `J1` solar input
- `D1` reverse protection
- `C1` input bulk capacitor
- `Q1` PWM switch (AO3400)
- `L1` 22uH inductor
- `D2` SS14 freewheel diode
- `C2` output capacitor
- `J2` battery output

STM8S103 measures:

- Solar input voltage
- Battery voltage
- Optional charge current

## Functional Nets

- `SOLAR_IN`: panel positive input
- `VIN_FILT`: post-protection filtered input
- `SW_NODE`: switched buck node
- `BAT_OUT`: battery charging node
- `SHUNT_LOW`: shunt low-side sense node
- `ADC_SOLAR`: solar divider midpoint
- `ADC_BATT`: battery divider midpoint
- `ADC_CURR`: current sense amplifier output
- `PWM_GATE`: MOSFET gate drive from STM8

## Divider Values

Solar ADC divider:

- `R1 = 100k` (top)
- `R2 = 33k` (bottom)

Battery ADC divider:

- `R3 = 100k` (top)
- `R4 = 100k` (bottom)

## Current Sense

- `R5 = 0.1 ohm` in low side return
- optional amplifier `U2` (INA180A2 class) to map low shunt voltage into ADC range

## Programming Header

`J3` exposes:

- `SWIM`
- `NRST`
- `VCC`
- `GND`

## Diagram

See:

- `hardware/schematic/smart_solar_lipo_charger_schematic.svg`
- `hardware/schematic/smart_solar_lipo_charger_netlist.csv`
