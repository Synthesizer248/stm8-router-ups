# Gerber Export Instructions

This folder is reserved for generated manufacturing outputs.

## Recommended Export Set

- `F.Cu`
- `B.Cu`
- `F.SilkS`
- `B.SilkS`
- `F.Mask`
- `B.Mask`
- `Edge.Cuts`
- Drill files (`.drl`)

## Fabrication Defaults

- `2-layer FR4`
- `1.6mm` thickness
- `1 oz` copper
- lead-free HASL or ENIG finish

## Pre-Release Checklist

- Confirm polarity marks on diode and battery connector.
- Confirm SWIM header pin order.
- Run design rule check in your CAD tool before generating Gerbers.
