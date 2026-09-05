#include <stm32l432xx.h>
#include "timer.h"
#include "gpio.h"


// Mapping of GPIO pin to timer channel
const static int get_tim1_channel[24] = {
    0,  0,  0,  0,  // PA0,  PA1,  PA2,  PA3,
    0,  0,  0, -1,  // PA4,  PA5,  PA6,  PA7,
    1,  2,  3,  4,  // PA8,  PA9,  PA10, PA11,
    0,  0,  0,  0,  // PA12, PA13, PA14, PA15,
   -2, -3,  0,  0,  // PB0,  PB1,  PB2,  PB3,
    0,  0,  0,  0   // PB4,  PB5,  PB6,  PB7
};

uint32_t freq_clk_psc = 4000000;

void tim1_init(uint32_t freq_hz) {
    RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;

    if (freq_hz > freq_clk_psc) {
        freq_hz = freq_clk_psc;
    }
    // f_clk_psc = 4 MHz
    uint32_t temp1 = freq_clk_psc / 65536 + 1; // 62
    uint16_t temp2 = temp1 / freq_hz + 1; // 1
    TIM1->PSC = temp2 - 1; // 0

    // 4 MHz / 1000 = 4 kHz
    uint32_t temp3 = freq_clk_psc / temp2; // 4 MHz
    uint16_t temp4 = temp3 / freq_hz; // 4000
    TIM1->ARR = temp4 - 1; // 3999          

    TIM1->BDTR |= TIM_BDTR_MOE;
    TIM1->CR1  |= TIM_CR1_CEN;
}

bool tim_config_pwm(TIM_TypeDef *TIMx, Nucleo_pin pin, unsigned int duty_cycle) {
    int channel = get_tim1_channel[pin];
    if (channel < 1 || channel > 4) {
        return false;
    }
    // Configure the CCMRx and CCER registers for the specified channel
    if (channel == 1) {
        // OC1Ref is not affected by some signal?
        TIM1->CCMR1 &= ~TIM_CCMR1_OC1CE; 
        // PWM Mode 1: OC1Ref is active as long as TIMx_CNT < TIMx_CCR1
        TIM1->CCMR1 &= ~TIM_CCMR1_OC1M;
        TIM1->CCMR1 |=  TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1;
        // Enable preload register on CCR1
        TIM1->CCMR1 |=  TIM_CCMR1_OC1PE;
        // Fast mode disabled
        TIM1->CCMR1 &= ~TIM_CCMR1_OC1FE;
        // CC1 channel is Output Compare 
        TIM1->CCMR1 &= ~TIM_CCMR1_CC1S;

        // Active high polarity for CHN1
        TIM1->CCER &= ~TIM_CCER_CC1NP;
        // Disable complementary output for channel 1
        TIM1->CCER &= ~TIM_CCER_CC1NE;
        // Active high polarity
        TIM1->CCER &= ~TIM_CCER_CC1P;
        // Compare mode enabled for channel 1
        TIM1->CCER |=  TIM_CCER_CC1E;

    } else if (channel == 2) {
        TIM1->CCMR1 &= ~(TIM_CCMR1_OC2CE); // OC2Ref is not affected by some signal?

        TIM1->CCMR1 &= ~(TIM_CCMR1_OC2M);
        TIM1->CCMR1 |=  (TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2M_1);

        TIM1->CCMR1 &= ~(TIM_CCMR1_OC2PE);
        TIM1->CCMR1 |=  (TIM_CCMR1_OC2PE);

        TIM1->CCMR1 &= ~(TIM_CCMR1_OC2FE);

        TIM1->CCMR1 &= ~(TIM_CCMR1_CC2S);
        
    } else if (channel == 3) {
        TIM1->CCMR2 &= ~(TIM_CCMR2_OC3CE); // OC3Ref is not affected by some signal?

        TIM1->CCMR2 &= ~(TIM_CCMR2_OC3M);
        TIM1->CCMR2 |=  (TIM_CCMR2_OC3M_2 | TIM_CCMR2_OC3M_1);

        TIM1->CCMR2 &= ~(TIM_CCMR2_OC3PE);
        TIM1->CCMR2 |=  (TIM_CCMR2_OC3PE);

        TIM1->CCMR2 &= ~(TIM_CCMR2_OC3FE);

        TIM1->CCMR2 &= ~(TIM_CCMR2_CC3S);

    } else if (channel == 4) {
        TIM1->CCMR2 &= ~(TIM_CCMR2_OC4CE); // OC4Ref is not affected by some signal?

        TIM1->CCMR2 &= ~(TIM_CCMR2_OC4M);
        TIM1->CCMR2 |=  (TIM_CCMR2_OC4M_2 | TIM_CCMR2_OC4M_1);

        TIM1->CCMR2 &= ~(TIM_CCMR2_OC4PE);
        TIM1->CCMR2 |=  (TIM_CCMR2_OC4PE);

        TIM1->CCMR2 &= ~(TIM_CCMR2_OC4FE);

        TIM1->CCMR2 &= ~(TIM_CCMR2_CC4S);

        // Configuring the CCER register
        // Active high polarity for CHN4
        TIM1->CCER &= ~(TIM_CCER_CC4NP);
        // Disable complementary output for channel 4 
        //TIM1->CCER &= ~(TIM_CCER_CC4NE); (DNE)
        // Active high polarity
        TIM1->CCER &= ~(TIM_CCER_CC4P);
        // Compare mode enabled for channel 4
        TIM1->CCER &= ~(TIM_CCER_CC4E);
        TIM1->CCER |=  (TIM_CCER_CC4E);
    }
    
    // Calculating the CCR value based on the duty cycle percentage and the ARR value
    uint16_t arr_val = TIM1->ARR + 1;
    uint16_t ccr_val = (arr_val * duty_cycle / 100) - 1;

    if      (duty_cycle < 1)   ccr_val = 0;
    else if (duty_cycle > 100) ccr_val = arr_val - 1;

    if      (channel == 1) TIM1->CCR1 = ccr_val;
    else if (channel == 2) TIM1->CCR2 = ccr_val;
    else if (channel == 3) TIM1->CCR3 = ccr_val;
    else if (channel == 4) TIM1->CCR4 = ccr_val;

    gpio_config_alternate_function(pin, 1);

    return true;
}
