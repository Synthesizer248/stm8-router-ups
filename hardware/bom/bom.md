# Bill of Materials

Primary BOM is in CSV:

- `hardware/bom/bom.csv`

## Required Core Parts

- `STM8S103F3P6`
- `AO3400` (or equivalent logic-level MOSFET)
- `22uH` inductor
- `SS14` Schottky diode
- `220uF` input capacitor
- `470uF` output capacitor
- `0.1 ohm` shunt resistor (optional current sensing path)

## Notes

- Current sensing amplifier (`INA180A2`) is optional but recommended for real MPPT behavior.
- Use low-ESR capacitors for stable switching behavior.
- Select inductor saturation current above peak panel current with margin.
