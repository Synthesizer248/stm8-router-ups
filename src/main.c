#include <stdint.h>
#include "config.h"

// STM8S register map (minimal subset)
#define REG8(addr)          (*(volatile uint8_t *)(addr))

// Clock
#define CLK_CKDIVR          REG8(0x50C6)

// GPIO
#define PB_DDR              REG8(0x5007)
#define PB_CR1              REG8(0x5008)
#define PB_IDR              REG8(0x5006)

#define PC_ODR              REG8(0x500A)
#define PC_DDR              REG8(0x500C)
#define PC_CR1              REG8(0x500D)

#define PD_ODR              REG8(0x500F)
#define PD_DDR              REG8(0x5011)
#define PD_CR1              REG8(0x5012)
#define PD_CR2              REG8(0x5013)

// ADC1
#define ADC1_CSR            REG8(0x5400)
#define ADC1_CR1            REG8(0x5401)
#define ADC1_CR2            REG8(0x5402)
#define ADC1_CR3            REG8(0x5403)
#define ADC1_DRH            REG8(0x5404)
#define ADC1_DRL            REG8(0x5405)

#define ADC1_CR1_ADON       (1U << 0)
#define ADC1_CR1_CONT       (1U << 1)
#define ADC1_CR1_SPSEL_FCPU18 (0x04U << 4)

#define ADC1_CSR_EOC        (1U << 7)
#define ADC1_CSR_CH_MASK    0x0FU

#define ADC1_CR2_ALIGN      (1U << 3)

// Pin bits
#define PIN4                (1U << 4)
#define PIN5                (1U << 5)
#define PIN3                (1U << 3)

// Wiring
// PB4: mains-present input
// PD3: boost enable output
// PD4: buzzer output
// PC5: LED output
// AIN2/PD2: battery ADC channel = 2
#define ADC_BATT_CHANNEL    2U

static void delay_cycles(volatile uint32_t n) {
    while (n--) {
        __asm
            nop
        __endasm;
    }
}

static void delay_ms(uint16_t ms) {
    // Approximate for 16MHz clock; tune if needed.
    while (ms--) {
        delay_cycles(1600U);
    }
}

static void clock_init(void) {
    // HSI /1 => 16 MHz
    CLK_CKDIVR = 0x00;
}

static void gpio_init(void) {
    // PB4 input floating
    PB_DDR &= (uint8_t)~PIN4;
    PB_CR1 &= (uint8_t)~PIN4;

    // PC5 push-pull output
    PC_DDR |= PIN5;
    PC_CR1 |= PIN5;

    // PD3, PD4 push-pull outputs
    PD_DDR |= (PIN3 | PIN4);
    PD_CR1 |= (PIN3 | PIN4);
    PD_CR2 &= (uint8_t)~(PIN3 | PIN4);

    // Initial low
    PC_ODR &= (uint8_t)~PIN5;
    PD_ODR &= (uint8_t)~(PIN3 | PIN4);
}

static void adc_init(void) {
    ADC1_CR1 = 0x00;
    ADC1_CR2 = 0x00;
    ADC1_CR3 = 0x00;

    // Single conversion, fADC = fCPU/18, right align
    ADC1_CR1 = ADC1_CR1_SPSEL_FCPU18;
    ADC1_CR2 &= (uint8_t)~ADC1_CR2_ALIGN;

    // Enable ADC
    ADC1_CR1 |= ADC1_CR1_ADON;
}

static uint16_t adc_read(uint8_t channel) {
    uint16_t result;

    ADC1_CSR = (ADC1_CSR & (uint8_t)~ADC1_CSR_CH_MASK) | (channel & ADC1_CSR_CH_MASK);

    // Start conversion: ADON set again
    ADC1_CR1 |= ADC1_CR1_ADON;

    while ((ADC1_CSR & ADC1_CSR_EOC) == 0U) {
        // wait
    }

    // Read data registers clears EOC
    result = ((uint16_t)ADC1_DRH << 2) | (uint16_t)(ADC1_DRL & 0x03U);
    return result;
}

static uint16_t batt_mv_from_adc(uint16_t adc_counts) {
    // Vadc = counts * ADC_REF_MV / ADC_MAX_COUNTS
    // Vin = Vadc * (Rtop + Rbot) / Rbot
    uint32_t mv = (uint32_t)adc_counts * ADC_REF_MV;
    mv /= ADC_MAX_COUNTS;
    mv = mv * (DIVIDER_RTOP_OHM + DIVIDER_RBOT_OHM) / DIVIDER_RBOT_OHM;
    return (uint16_t)mv;
}

static uint8_t mains_present(void) {
    return (PB_IDR & PIN4) ? 1U : 0U;
}

static void boost_set(uint8_t on) {
    if (on) {
        PD_ODR |= PIN3;
    } else {
        PD_ODR &= (uint8_t)~PIN3;
    }
}

static void led_set(uint8_t on) {
    if (on) {
        PC_ODR |= PIN5;
    } else {
        PC_ODR &= (uint8_t)~PIN5;
    }
}

static void buzzer_beep(uint16_t ms) {
    PD_ODR |= PIN4;
    delay_ms(ms);
    PD_ODR &= (uint8_t)~PIN4;
}

int main(void) {
    uint8_t low_lock = 0U;

    clock_init();
    gpio_init();
    adc_init();

    while (1) {
        uint16_t batt_counts = adc_read(ADC_BATT_CHANNEL);
        uint16_t batt_mv = batt_mv_from_adc(batt_counts);
        uint8_t mains = mains_present();

        if (mains) {
            // Adapter present: adapter powers router
            boost_set(0U);
            led_set(1U);

            // Clear lock only after battery recovered
            if (batt_mv >= BATT_RECOVER_MV) {
                low_lock = 0U;
            }
        } else {
            led_set(0U);

            if (!low_lock) {
                if (batt_mv <= BATT_CUTOFF_MV) {
                    low_lock = 1U;
                    boost_set(0U);
                    buzzer_beep(120U);
                } else {
                    boost_set(1U);
                }
            } else {
                boost_set(0U);
            }
        }

        delay_ms(MAIN_LOOP_DELAY_MS);
    }
}
