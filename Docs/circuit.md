# Router UPS Circuit (STM8S + 2S Li-ion)

## 1) Power Architecture

```text
                    +------------------------ Router 12V OUT (+)
                    | 
12V Adapter (+) ---+----> D1 (Schottky/ideal diode) -------+
                    |                                       |
                    +----> 2S Charger (8.4V CC/CV) ----+    |
                                                       |    |
2S Battery Pack (+) -> 2S BMS -> Boost 12V -> D2 -----+----+

All grounds are common:
Adapter (-), Charger GND, BMS GND, Boost GND, STM8 GND, Router GND
```

- `D1` and `D2` perform power-path OR-ing so the higher source feeds output.
- Prefer ideal-diode modules for lower voltage drop than Schottky.

## 2) STM8S Connections

Assumed MCU: STM8S103F3P6 dev board.

- `PB4`  : Mains present detect input (HIGH when adapter present)
- `PD3`  : Boost enable output (to converter `EN` pin)
- `PD4`  : Buzzer output (optional, via transistor if needed)
- `PC5`  : Status LED output
- `PD2/AIN2` : Battery voltage ADC input via divider

## 3) Battery Voltage Divider (for PD2/AIN2)

Use:
- `Rtop = 100k` from battery+ to `PD2`
- `Rbot = 33k` from `PD2` to GND

This scales max 8.4V battery into ADC-safe range (~2.08V).

## 4) Mains Presence Detection

### Option A (recommended): optocoupler
- Drive optocoupler LED from adapter side with resistor.
- Transistor output pulls `PB4` high/low with pull-up/down logic.
- Gives isolation from adapter noise.

### Option B: transistor divider
- Simple NPN + resistor network from adapter 12V to logic level for `PB4`.
- Ensure `PB4` never exceeds 3.3V/5V logic level of your dev board.

## 5) Firmware Behavior (already implemented)

- If mains present: boost OFF, LED ON
- If mains missing and battery above cutoff: boost ON
- If battery below cutoff: boost OFF + buzzer alert
- Recovery hysteresis prevents rapid toggling

Thresholds are in `inc/config.h`:
- `BATT_CUTOFF_MV` (default 6400mV)
- `BATT_RECOVER_MV` (default 7000mV)

## 6) Safety Notes

- Use matched 18650 cells in good condition.
- 2S BMS is mandatory.
- Add fuse on battery positive line.
- Verify router current draw and size adapter/boost with margin.
- Test cutoff/recovery thresholds with a bench supply before live use.
