#ifndef CONFIG_H
#define CONFIG_H

#define ADC_MAX_COUNTS      1023UL
#define ADC_REF_MV          3300UL

// Divider: Rtop = 100k, Rbottom = 33k
#define DIVIDER_RTOP_OHM    100000UL
#define DIVIDER_RBOT_OHM    33000UL

// 2S battery thresholds
#define BATT_CUTOFF_MV      6400U
#define BATT_RECOVER_MV     7000U

#define MAIN_LOOP_DELAY_MS  100U

#endif
