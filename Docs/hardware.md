# Hardware Design Package

## Schematic

- Design notes: `hardware/schematic/smart_solar_lipo_charger_schematic.md`
- Net list: `hardware/schematic/smart_solar_lipo_charger_netlist.csv`
- Diagram: `hardware/schematic/smart_solar_lipo_charger_schematic.svg`
- KiCad starter: `hardware/kicad/smart_solar_lipo_charger.kicad_sch`

## PCB

- PCB notes: `hardware/pcb/smart_solar_lipo_charger_pcb.md`
- Placement table: `hardware/pcb/component_placement.csv`
- Layout image: `hardware/pcb/smart_solar_lipo_charger_pcb.svg`
- KiCad starter: `hardware/kicad/smart_solar_lipo_charger.kicad_pcb`

## Manufacturing

- Gerber export guide: `hardware/manufacturing/gerbers/README.md`

## BOM

- CSV BOM: `hardware/bom/bom.csv`
- Human-readable BOM: `hardware/bom/bom.md`

## Power Stage Constraints

- Keep current loop area (`VIN -> MOSFET -> DIODE -> GND`) tight.
- Place `220uF` input capacitor near MOSFET.
- Place `470uF` output capacitor near battery connector.
- Route shunt Kelvin traces separately from high current traces.
