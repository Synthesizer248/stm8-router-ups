# Router UPS Circuit (With MOSFET + STM8S Module)

## KiCad-Style Diagram

![STM8 Router UPS KiCad-style schematic](./circuit_kicad_style.svg)

## 1) Full Circuit Topology

```text
                           +---------------------------> Router (+)
                           |                UPS_12V_OUT
12V Adapter (+) ---+-----> Q1 P-MOS (ideal-diode OR) -----------+
                   |                                              |
                   +-----> 2S Charger 8.4V (CC/CV)               |
                                |                                 |
                                +-------> BMS P+                  |
Battery 2S pack <---- BMS B+/B-         BMS P- ------------------+---- GND
                                |
                                +--> Fuse --> Q3 P-MOS switch --> Boost IN+
                                                   (STM8 control)   Boost GND -> GND
Boost OUT 12V --------------------------------> Q2 P-MOS (ideal-diode OR) --+
                                                                              |
                                                                              +--> UPS_12V_OUT

UPS_12V_OUT -> 5V/3.3V regulator -> STM8S VCC
GND ------------------------------------------> STM8S GND
```

MOSFET roles:
- `Q1`: adapter path ideal-diode MOSFET (prevents reverse current).
- `Q2`: boost path ideal-diode MOSFET (prevents backfeed into boost).
- `Q3`: battery-to-boost high-side switch MOSFET controlled by STM8.

## 2) STM8S103 Module Block

```text
                 +----------------------+
                 |      STM8S103        |
Adapter detect ->| PB4              VCC |<- 5V/3.3V regulator
Battery ADC ---->| PD2/AIN2         GND |-> Common GND
Boost control -->| PD3                  |
Buzzer drive ----| PD4                  |
Status LED ------| PC5                  |
                 +----------------------+
```

Control connections:
- `PD3` -> gate-driver transistor/resistor network for `Q3` and/or boost `EN`.
- `PD4` -> buzzer (direct small buzzer or via N-MOS driver).
- `PC5` -> LED + resistor.

## 3) Battery Sense Divider (PD2/AIN2)

- `Rtop = 100k`: `BMS P+` to `PD2`
- `Rbot = 33k`: `PD2` to GND
- Optional `100nF` from `PD2` to GND for filtering

At 8.4V battery, ADC input is about 2.08V.

## 4) Adapter-Present Detect (PB4)

- Use optocoupler or divider+transistor from adapter 12V to logic-level PB4.
- Ensure PB4 never exceeds MCU input voltage rating.

Simple divider:
- `100k` from adapter +12V to PB4
- `33k` from PB4 to GND

## 5) Firmware Behavior (Matches This Diagram)

- Adapter present: `Q3` OFF / boost OFF, router fed via `Q1`.
- Adapter missing: enable boost path (`Q3` ON and boost EN ON) if battery above cutoff.
- Battery low: disable boost path and alarm; recover only above hysteresis threshold.

Thresholds in `inc/config.h`:
- `BATT_CUTOFF_MV = 6400`
- `BATT_RECOVER_MV = 7000`

## 6) Wiring Safety Checklist

- Verify BMS mapping: `B+/B-` to cells, `P+/P-` to charger/load.
- Add battery fuse before boost input.
- Confirm MOSFET orientation (`source`, `drain`, `body diode` direction).
- Set boost output to router voltage before connecting router.
- Validate switchover with a dummy load first.
