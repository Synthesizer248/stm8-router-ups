# BOM And Pin Map

## 1) Core Modules

- STM8S103F3 development board (1x)
- 2S Li-ion BMS board, continuous current >= router load (1x)
- 2S charger module (8.4V CC/CV), input 12V (1x)
- Boost converter 6V-8.4V to 12V, >=2A recommended (1x)
- Schottky diodes 3A+ (2x) or ideal-diode power-path modules (2x)
- 18650 cells (2x, same model/capacity/age)
- Cell holder for 2S pack or welded pack (1x)

## 2) Sensing/Control Components

- Resistor 100k 1% (battery divider top) (1x)
- Resistor 33k 1% (battery divider bottom) (1x)
- Optocoupler PC817 or equivalent (optional, mains detect) (1x)
- Resistors for optocoupler LED/transistor side (2x to 3x)
- Active buzzer 3.3V/5V or passive buzzer + driver transistor (1x)
- LED + 330R resistor (status) (1x each)

## 3) Protection/Power Parts

- Inline fuse holder + fuse (battery positive) (1x)
- Terminal blocks / screw connectors (as required)
- Wires rated for expected current

## 4) STM8 Pin Mapping Table

| Function | STM8 Pin | Direction | Connected To |
|---|---|---|---|
| Mains present | PB4 | Input | Optocoupler/transistor output |
| Boost enable | PD3 | Output | Boost converter `EN` |
| Buzzer | PD4 | Output | Buzzer input/driver |
| Status LED | PC5 | Output | LED + resistor |
| Battery ADC | PD2 / AIN2 | Input (ADC) | Divider midpoint (100k/33k) |
| Ground | GND | - | Common ground |

## 5) Suggested Ratings (for typical 12V router)

- Router load assumption: up to 1A at 12V
- Adapter: 12V, 2A or higher
- Boost converter: 12V, 2A output capability
- Charger current: 0.8A to 1A (depends on cell spec/thermal)

## 6) Pre-Power Checklist

- Confirm common ground across all modules.
- Confirm battery polarity and BMS wiring.
- Confirm divider output at ADC pin is < MCU analog max.
- Confirm OR-ing diodes orientation.
- Confirm boost output is regulated to router requirement.
- Test no-load then dummy-load before router.
