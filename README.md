# Smart Solar Li-Po Charging System (STM8S + STM8duino)

Open-source reference design for a microcontroller-controlled solar charger:

- Solar panel input: `5V / 200mA` (about `1W`)
- Battery: `1S 3.7V Li-Po`, charge limit `4.2V`
- Controller: `STM8S103F3P6`
- Firmware environment: Arduino IDE with STM8duino core
- Programming: ST-LINK V2 over SWIM

This repository includes firmware, circuit design, PCB design files, BOM, and bring-up/testing docs.

## About

STM8-based open-source smart solar charger for `5V/200mA` panels and `1S Li-Po` batteries, with PWM buck control, Li-Po safety state machine, and lightweight MPPT.

## Tags

`stm8` `stm8duino` `solar-charger` `mppt` `lipo-charger` `buck-converter` `embedded` `open-hardware` `st-link`

## Features

- Solar voltage monitoring (`ADC`)
- Battery voltage monitoring (`ADC`)
- Optional current monitoring (`0.1 ohm` shunt + amplifier)
- PWM buck-stage control
- Li-Po safety state machine
- Optional perturb-and-observe MPPT loop

## Charging Modes

| Battery Voltage | Mode | Behavior |
|---|---|---|
| `< 3.2V` | Precharge | Low current target (`~40mA`) |
| `3.2V to 4.1V` | Bulk/Normal | Higher current target (`~160mA`) |
| `4.1V to 4.2V` | CV | Hold near `4.2V` |
| `>= 4.2V` and low current | Done | PWM off |
| `>= 4.25V` | Fault | PWM off (protection) |

## Repository Layout

```text
.
|-- arduino_lipo_charger.ino                      # Main firmware sketch
|-- firmware/stm8_solar_mppt_charger/
|   `-- stm8_solar_mppt_charger.ino              # Same firmware (folderized)
|-- hardware/
|   |-- bom/
|   |   |-- bom.csv
|   |   `-- bom.md
|   |-- kicad/
|   |   |-- smart_solar_lipo_charger.kicad_sch
|   |   `-- smart_solar_lipo_charger.kicad_pcb
|   |-- schematic/
|   |   |-- smart_solar_lipo_charger_schematic.md
|   |   |-- smart_solar_lipo_charger_netlist.csv
|   |   `-- smart_solar_lipo_charger_schematic.svg
|   |-- pcb/
|   |   |-- smart_solar_lipo_charger_pcb.md
|   |   |-- component_placement.csv
|   |   `-- smart_solar_lipo_charger_pcb.svg
|   `-- manufacturing/gerbers/
|       `-- README.md
|-- Docs/
|   |-- architecture.md
|   |-- firmware.md
|   |-- hardware.md
|   |-- mppt.md
|   |-- programming_stlink.md
|   |-- calibration.md
|   `-- test_plan.md
|-- LICENSE
`-- LICENSE-HARDWARE.md
```

## Hardware Summary

- Inductor: `22uH`
- Input capacitor: `220uF`
- Output capacitor: `470uF`
- Schottky diode: `SS14`
- Main switch MOSFET: `AO3400` (or equivalent logic-level MOSFET)
- Current shunt: `0.1 ohm`
- Voltage sense dividers for solar and battery ADC channels

Detailed notes:

- Schematic: [`hardware/schematic/smart_solar_lipo_charger_schematic.md`](hardware/schematic/smart_solar_lipo_charger_schematic.md)
- PCB: [`hardware/pcb/smart_solar_lipo_charger_pcb.md`](hardware/pcb/smart_solar_lipo_charger_pcb.md)

## Firmware Build/Flash (Arduino IDE 2.x)

1. Install Arduino IDE 2.x.
2. Open `File -> Preferences`.
3. Add board manager URL:
   `https://raw.githubusercontent.com/tenbaht/sduino/master/package_sduino_stm8_index.json`
4. Install STM8 core from Boards Manager.
5. Open `arduino_lipo_charger.ino`.
6. Select board family containing `STM8S103F3P6`.
7. Select programmer `ST-LINK V2`.
8. Use `Sketch -> Upload Using Programmer`.

ST-LINK wiring:

- `SWIM` -> `SWIM`
- `GND` -> `GND`
- `NRST` -> `NRST` (recommended)
- `3V3/5V` -> target VCC as required

Full guide: [`Docs/programming_stlink.md`](Docs/programming_stlink.md)

## Safety Notes

- This is a development reference design, not a certified charger.
- Verify ADC scaling and calibration before connecting real batteries.
- Use a protected Li-Po cell or external protection circuit.
- Validate with a dummy load before field deployment.

## License

- Firmware/code: MIT (see [`LICENSE`](LICENSE))
- Hardware documentation and design files: CERN-OHL-S-2.0 (see [`LICENSE-HARDWARE.md`](LICENSE-HARDWARE.md))
