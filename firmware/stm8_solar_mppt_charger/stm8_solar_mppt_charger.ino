#include <Arduino.h>

// ------------------------ Pin Mapping ------------------------
// Adjust these if your STM8S103F3P6 board uses a different mapping.
#if defined(PC6)
const uint8_t PIN_PWM_GATE = PC6;      // Preferred TIM1 PWM-capable pin
#elif defined(PD3)
const uint8_t PIN_PWM_GATE = PD3;
#else
const uint8_t PIN_PWM_GATE = 3;
#endif

#if defined(PC5)
const uint8_t PIN_STATUS_LED = PC5;
#elif defined(LED_BUILTIN)
const uint8_t PIN_STATUS_LED = LED_BUILTIN;
#else
const uint8_t PIN_STATUS_LED = 13;
#endif

#if defined(A0)
const uint8_t PIN_SOLAR_ADC = A0;
#elif defined(PB0)
const uint8_t PIN_SOLAR_ADC = PB0;
#else
const uint8_t PIN_SOLAR_ADC = 0;
#endif

#if defined(A1)
const uint8_t PIN_BATT_ADC = A1;
#elif defined(PB1)
const uint8_t PIN_BATT_ADC = PB1;
#else
const uint8_t PIN_BATT_ADC = 1;
#endif

#if defined(A2)
const uint8_t PIN_CURRENT_ADC = A2;
#elif defined(PB2)
const uint8_t PIN_CURRENT_ADC = PB2;
#else
const uint8_t PIN_CURRENT_ADC = 2;
#endif

// ------------------------ Compile Options ------------------------
#ifndef ENABLE_CURRENT_SENSE
#define ENABLE_CURRENT_SENSE 1
#endif

#ifndef ENABLE_SERIAL_DEBUG
#define ENABLE_SERIAL_DEBUG 0
#endif

// ------------------------ ADC Scaling ------------------------
const uint16_t ADC_REF_MV = 3300;
const uint16_t ADC_MAX_COUNTS = 1023;

// Solar divider (Rtop from solar+ to ADC, Rbot from ADC to GND)
const uint32_t SOLAR_DIV_RTOP_OHM = 100000;
const uint32_t SOLAR_DIV_RBOT_OHM = 33000;

// Battery divider
const uint32_t BATT_DIV_RTOP_OHM = 100000;
const uint32_t BATT_DIV_RBOT_OHM = 100000;

// Current sensing: shunt + amplifier
const uint16_t SHUNT_MILLIOHM = 100;     // 0.1 ohm
const uint8_t CURRENT_SENSE_GAIN = 20;   // e.g., INA180A2/A3 class gain stage

// ------------------------ Charger Limits ------------------------
const uint16_t PANEL_CURRENT_LIMIT_MA = 200;   // 5V / 200mA panel

const uint16_t BATT_PRECHARGE_MV = 3200;
const uint16_t BATT_CV_ENTRY_MV = 4100;
const uint16_t BATT_FULL_MV = 4200;
const uint16_t BATT_OVERVOLTAGE_MV = 4250;
const uint16_t BATT_RECHARGE_MV = 4150;

const uint16_t PRECHARGE_TARGET_CURRENT_MA = 40;
const uint16_t BULK_TARGET_CURRENT_MA = 160;
const uint16_t TERMINATION_CURRENT_MA = 20;

const uint16_t SOLAR_START_MV = 3600;
const uint16_t SOLAR_COLLAPSE_MV = 3300;

// ------------------------ PWM / Timing ------------------------
const uint8_t PWM_MIN_DUTY = 0;
const uint8_t PWM_MAX_DUTY = 220;  // Keep headroom to avoid hard switching at 100%
const uint8_t PWM_START_DUTY = 10;

const uint16_t CONTROL_INTERVAL_MS = 100;
const uint16_t MPPT_INTERVAL_MS = 600;
const uint16_t MPPT_DEADBAND_MW = 10;
const int8_t MPPT_STEP_DUTY = 2;

const uint16_t CURRENT_HYSTERESIS_MA = 5;
const uint16_t VOLTAGE_HYSTERESIS_MV = 10;

#if ENABLE_SERIAL_DEBUG
const uint16_t SERIAL_LOG_INTERVAL_MS = 1000;
#endif

enum ChargeMode {
  MODE_IDLE = 0,
  MODE_PRECHARGE,
  MODE_BULK,
  MODE_CV,
  MODE_DONE,
  MODE_FAULT
};

struct Telemetry {
  uint16_t solarMv;
  uint16_t battMv;
  uint16_t chargeMa;
  uint16_t powerMw;
};

ChargeMode gMode = MODE_IDLE;
uint8_t gPwmDuty = 0;
int8_t gMpptDirection = 1;
uint16_t gPrevPowerMw = 0;

uint32_t gLastControlMs = 0;
uint32_t gLastMpptMs = 0;
#if ENABLE_SERIAL_DEBUG
uint32_t gLastLogMs = 0;
#endif

static uint16_t readAdcAverage(const uint8_t pin, const uint8_t samples) {
  uint32_t sum = 0;
  for (uint8_t i = 0; i < samples; i++) {
    sum += analogRead(pin);
    delay(2);
  }
  return (uint16_t)(sum / samples);
}

static uint16_t adcToMilliVolts(const uint16_t adcCounts) {
  return (uint16_t)(((uint32_t)adcCounts * ADC_REF_MV) / ADC_MAX_COUNTS);
}

static uint16_t scaleDividerMilliVolts(const uint16_t vadcMv, const uint32_t rTop, const uint32_t rBot) {
  return (uint16_t)(((uint32_t)vadcMv * (rTop + rBot)) / rBot);
}

static uint16_t readSolarMilliVolts() {
  const uint16_t adc = readAdcAverage(PIN_SOLAR_ADC, 8);
  const uint16_t vadc = adcToMilliVolts(adc);
  return scaleDividerMilliVolts(vadc, SOLAR_DIV_RTOP_OHM, SOLAR_DIV_RBOT_OHM);
}

static uint16_t readBatteryMilliVolts() {
  const uint16_t adc = readAdcAverage(PIN_BATT_ADC, 8);
  const uint16_t vadc = adcToMilliVolts(adc);
  return scaleDividerMilliVolts(vadc, BATT_DIV_RTOP_OHM, BATT_DIV_RBOT_OHM);
}

static uint16_t readChargeMilliAmps() {
#if ENABLE_CURRENT_SENSE
  const uint16_t adc = readAdcAverage(PIN_CURRENT_ADC, 8);
  const uint16_t vsenseMv = adcToMilliVolts(adc);
  const uint32_t ima = ((uint32_t)vsenseMv * 1000UL) / ((uint32_t)SHUNT_MILLIOHM * CURRENT_SENSE_GAIN);
  return (uint16_t)ima;
#else
  return (uint16_t)(((uint32_t)PANEL_CURRENT_LIMIT_MA * gPwmDuty) / PWM_MAX_DUTY);
#endif
}

static void setPwmDuty(const int16_t newDuty) {
  int16_t clipped = newDuty;
  if (clipped < PWM_MIN_DUTY) {
    clipped = PWM_MIN_DUTY;
  }
  if (clipped > PWM_MAX_DUTY) {
    clipped = PWM_MAX_DUTY;
  }
  gPwmDuty = (uint8_t)clipped;
  analogWrite(PIN_PWM_GATE, gPwmDuty);
}

static const char* modeName(const ChargeMode mode) {
  switch (mode) {
    case MODE_IDLE: return "IDLE";
    case MODE_PRECHARGE: return "PRECHARGE";
    case MODE_BULK: return "BULK";
    case MODE_CV: return "CV";
    case MODE_DONE: return "DONE";
    case MODE_FAULT: return "FAULT";
    default: return "UNKNOWN";
  }
}

static Telemetry sampleTelemetry() {
  Telemetry t;
  t.solarMv = readSolarMilliVolts();
  t.battMv = readBatteryMilliVolts();
  t.chargeMa = readChargeMilliAmps();
  t.powerMw = (uint16_t)(((uint32_t)t.solarMv * t.chargeMa) / 1000UL);
  return t;
}

static void updateMode(const Telemetry& t) {
  if (t.solarMv < SOLAR_START_MV) {
    gMode = MODE_IDLE;
    return;
  }

  if (t.battMv >= BATT_OVERVOLTAGE_MV) {
    gMode = MODE_FAULT;
    return;
  }

  if (gMode == MODE_DONE && t.battMv <= BATT_RECHARGE_MV) {
    gMode = MODE_BULK;
    return;
  }

  if (t.battMv < BATT_PRECHARGE_MV) {
    gMode = MODE_PRECHARGE;
    return;
  }

  if (t.battMv < BATT_CV_ENTRY_MV) {
    gMode = MODE_BULK;
    return;
  }

  if (t.battMv < BATT_FULL_MV) {
    gMode = MODE_CV;
    return;
  }

#if ENABLE_CURRENT_SENSE
  if (t.chargeMa <= TERMINATION_CURRENT_MA) {
    gMode = MODE_DONE;
  } else {
    gMode = MODE_CV;
  }
#else
  gMode = MODE_DONE;
#endif
}

static void applyCurrentLimit(const Telemetry& t, const uint16_t targetMa) {
#if ENABLE_CURRENT_SENSE
  if (t.chargeMa > targetMa + CURRENT_HYSTERESIS_MA) {
    setPwmDuty((int16_t)gPwmDuty - 2);
  } else if (t.chargeMa + CURRENT_HYSTERESIS_MA < targetMa && t.solarMv > (SOLAR_COLLAPSE_MV + 120)) {
    setPwmDuty((int16_t)gPwmDuty + 1);
  }
#else
  (void)t;
  (void)targetMa;
#endif
}

static void runMppt(const Telemetry& t, const uint32_t nowMs) {
  if ((uint32_t)(nowMs - gLastMpptMs) < MPPT_INTERVAL_MS) {
    return;
  }
  gLastMpptMs = nowMs;

  const uint16_t power = t.powerMw;
  if (power + MPPT_DEADBAND_MW < gPrevPowerMw) {
    gMpptDirection = (int8_t)(-gMpptDirection);
  }

  const int16_t stepped = (int16_t)gPwmDuty + ((int16_t)gMpptDirection * MPPT_STEP_DUTY);
  setPwmDuty(stepped);
  gPrevPowerMw = power;
}

static void controlPrechargeOrBulk(const Telemetry& t, const uint16_t targetCurrentMa, const uint32_t nowMs) {
  if (gPwmDuty == 0) {
    setPwmDuty(PWM_START_DUTY);
  }

  runMppt(t, nowMs);

  if (t.solarMv < SOLAR_COLLAPSE_MV) {
    setPwmDuty((int16_t)gPwmDuty - 4);
  }

  if (t.chargeMa > PANEL_CURRENT_LIMIT_MA) {
    setPwmDuty((int16_t)gPwmDuty - 4);
  }

  applyCurrentLimit(t, targetCurrentMa);
}

static void controlCv(const Telemetry& t) {
  if (t.battMv > BATT_FULL_MV) {
    setPwmDuty((int16_t)gPwmDuty - 3);
  } else if (t.battMv + VOLTAGE_HYSTERESIS_MV < BATT_FULL_MV && t.solarMv > (SOLAR_COLLAPSE_MV + 100)) {
    setPwmDuty((int16_t)gPwmDuty + 1);
  }

  if (t.solarMv < SOLAR_COLLAPSE_MV) {
    setPwmDuty((int16_t)gPwmDuty - 3);
  }

#if ENABLE_CURRENT_SENSE
  if (t.chargeMa > BULK_TARGET_CURRENT_MA) {
    setPwmDuty((int16_t)gPwmDuty - 2);
  }
#endif
}

static void applyControl(const Telemetry& t, const uint32_t nowMs) {
  switch (gMode) {
    case MODE_IDLE:
      setPwmDuty(0);
      break;

    case MODE_PRECHARGE:
      controlPrechargeOrBulk(t, PRECHARGE_TARGET_CURRENT_MA, nowMs);
      break;

    case MODE_BULK:
      controlPrechargeOrBulk(t, BULK_TARGET_CURRENT_MA, nowMs);
      break;

    case MODE_CV:
      controlCv(t);
      break;

    case MODE_DONE:
      setPwmDuty(0);
      break;

    case MODE_FAULT:
    default:
      setPwmDuty(0);
      break;
  }
}

static void updateStatusLed(const uint32_t nowMs) {
  uint16_t periodMs = 1000;
  uint16_t onMs = 50;

  switch (gMode) {
    case MODE_IDLE:
      periodMs = 1000;
      onMs = 40;
      break;
    case MODE_PRECHARGE:
      periodMs = 1000;
      onMs = 500;
      break;
    case MODE_BULK:
      digitalWrite(PIN_STATUS_LED, HIGH);
      return;
    case MODE_CV:
      periodMs = 250;
      onMs = 120;
      break;
    case MODE_DONE:
      periodMs = 2000;
      onMs = 100;
      break;
    case MODE_FAULT:
    default:
      periodMs = 160;
      onMs = 80;
      break;
  }

  const uint16_t phase = (uint16_t)(nowMs % periodMs);
  digitalWrite(PIN_STATUS_LED, (phase < onMs) ? HIGH : LOW);
}

#if ENABLE_SERIAL_DEBUG
static void logTelemetry(const Telemetry& t, const uint32_t nowMs) {
  if ((uint32_t)(nowMs - gLastLogMs) < SERIAL_LOG_INTERVAL_MS) {
    return;
  }
  gLastLogMs = nowMs;

  Serial.print("mode=");
  Serial.print(modeName(gMode));
  Serial.print(" pwm=");
  Serial.print(gPwmDuty);
  Serial.print(" solar_mV=");
  Serial.print(t.solarMv);
  Serial.print(" batt_mV=");
  Serial.print(t.battMv);
  Serial.print(" charge_mA=");
  Serial.print(t.chargeMa);
  Serial.print(" power_mW=");
  Serial.println(t.powerMw);
}
#endif

void setup() {
  pinMode(PIN_PWM_GATE, OUTPUT);
  pinMode(PIN_STATUS_LED, OUTPUT);
  pinMode(PIN_SOLAR_ADC, INPUT);
  pinMode(PIN_BATT_ADC, INPUT);
#if ENABLE_CURRENT_SENSE
  pinMode(PIN_CURRENT_ADC, INPUT);
#endif

  setPwmDuty(0);
  digitalWrite(PIN_STATUS_LED, LOW);

#if ENABLE_SERIAL_DEBUG
  Serial.begin(115200);
#endif
}

void loop() {
  const uint32_t now = millis();
  if ((uint32_t)(now - gLastControlMs) < CONTROL_INTERVAL_MS) {
    updateStatusLed(now);
    return;
  }
  gLastControlMs = now;

  Telemetry t = sampleTelemetry();
  updateMode(t);
  applyControl(t, now);
  updateStatusLed(now);

#if ENABLE_SERIAL_DEBUG
  logTelemetry(t, now);
#endif
}
