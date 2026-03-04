#include <Arduino.h>

// Pin map for STM8S103 (adjust if your board variant maps differently)
const uint8_t PIN_MAINS_PRESENT = PB4;  // HIGH when adapter is present
const uint8_t PIN_BOOST_EN = PD3;       // To boost EN and/or Q3 gate driver
const uint8_t PIN_BUZZER = PD4;         // Optional buzzer output
const uint8_t PIN_STATUS_LED = PC5;     // Status LED output

#if defined(A2)
const uint8_t PIN_BATT_ADC = A2;        // Preferred analog alias
#else
const uint8_t PIN_BATT_ADC = PD2;       // AIN2 fallback
#endif

// Battery thresholds (2S Li-ion)
const uint16_t BATT_CUTOFF_MV = 6400;   // Cut load around 3.2V/cell
const uint16_t BATT_RECOVER_MV = 7000;  // Hysteresis to re-enable output

const uint16_t LOOP_DELAY_MS = 100;

// ADC and divider constants
const uint32_t ADC_REF_MV = 3300;       // Set to your board ADC reference
const uint32_t ADC_MAX_COUNTS = 1023;   // 10-bit ADC
const uint32_t DIV_RTOP_OHM = 100000;   // 100k from battery+ to ADC pin
const uint32_t DIV_RBOT_OHM = 33000;    // 33k from ADC pin to GND

bool lowBatteryLock = false;

static void boostSet(bool on) {
  digitalWrite(PIN_BOOST_EN, on ? HIGH : LOW);
}

static void ledSet(bool on) {
  digitalWrite(PIN_STATUS_LED, on ? HIGH : LOW);
}

static void buzzerBeep(uint16_t ms) {
  digitalWrite(PIN_BUZZER, HIGH);
  delay(ms);
  digitalWrite(PIN_BUZZER, LOW);
}

static bool mainsPresent() {
  return digitalRead(PIN_MAINS_PRESENT) == HIGH;
}

static uint16_t readBatteryMilliVolts() {
  // Simple averaging for ADC noise reduction
  uint32_t sum = 0;
  for (uint8_t i = 0; i < 8; i++) {
    sum += analogRead(PIN_BATT_ADC);
    delay(2);
  }

  const uint32_t adcCounts = sum / 8UL;
  const uint32_t vadcMv = (adcCounts * ADC_REF_MV) / ADC_MAX_COUNTS;
  const uint32_t vbattMv = (vadcMv * (DIV_RTOP_OHM + DIV_RBOT_OHM)) / DIV_RBOT_OHM;
  return (uint16_t)vbattMv;
}

void setup() {
  pinMode(PIN_MAINS_PRESENT, INPUT);
  pinMode(PIN_BOOST_EN, OUTPUT);
  pinMode(PIN_BUZZER, OUTPUT);
  pinMode(PIN_STATUS_LED, OUTPUT);
  pinMode(PIN_BATT_ADC, INPUT);

  boostSet(false);
  digitalWrite(PIN_BUZZER, LOW);
  ledSet(false);
}

void loop() {
  const bool mains = mainsPresent();
  const uint16_t battMv = readBatteryMilliVolts();

  if (mains) {
    // Adapter is present; feed load from adapter path
    boostSet(false);
    ledSet(true);

    // Unlock only after battery has recovered
    if (battMv >= BATT_RECOVER_MV) {
      lowBatteryLock = false;
    }
  } else {
    // Mains down; try to run from battery via boost
    ledSet(false);

    if (!lowBatteryLock) {
      if (battMv <= BATT_CUTOFF_MV) {
        lowBatteryLock = true;
        boostSet(false);
        buzzerBeep(120);
      } else {
        boostSet(true);
      }
    } else {
      boostSet(false);
    }
  }

  delay(LOOP_DELAY_MS);
}
