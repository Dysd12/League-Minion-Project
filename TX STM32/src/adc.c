/**
 * Daniel Yeo
 * 2026/9/1
 */

#include <stm32l432xx.h>
#include "adc.h"
#include "gpio.h"


// Note that we don't handle the special ADC
// internal channels (temperature sensor and voltage reference).
static int get_adc_channel[24] = { 
    5,  6,  7,  8,  // PA0,  PA1,  PA2,  PA3,
    9,  10, 11, 12, // PA4,  PA5,  PA6,  PA7,
    0,  0,  0,  0,  // PA8,  PA9,  PA10, PA11,
    12, 13, 14, 15, // PA12, PA13, PA14, PA15,
    15, 16, 0,  0,  // PB0,  PB1,  PB2,  PB3,
    0,  0,  0,  0   // PB4,  PB5,  PB6,  PB7
};

void adc1_init(void) 
{
    // According to 16.4.10, you should only write the ADC RCC bits if the ADC
    // is disabled. It's disabled out of reset, so this code is likely unneeded.
    ADC1->CR &= ~ADC_CR_ADEN;

    RCC->AHB2ENR |= RCC_AHB2ENR_ADCEN;

    RCC->AHB2RSTR |= RCC_AHB2RSTR_ADCRST; // Go into reset.
    for (volatile int delay = 0; delay < 5; delay++); // delay
    RCC->AHB2RSTR &= ~RCC_AHB2RSTR_ADCRST; // Come out of reset.
    for (volatile int delay = 0; delay < 5; delay++);

    // Reset leaves us in deep-power-down mode, so exit it now.
    if ((ADC1->CR & ADC_CR_DEEPPWD) == ADC_CR_DEEPPWD) {
        ADC1->CR &= ~ADC_CR_DEEPPWD;
    }
    // Enable the ADC internal voltage regulator
    ADC1->CR |= ADC_CR_ADVREGEN;    
    for (volatile int delay = 0; delay < 1600; delay++);
}

void adc_config_single(Nucleo_pin pin) 
{
    int channel = get_adc_channel[pin];

    // If the ADC is off, turn it on before continuing!
    if (!(RCC->AHB2ENR & RCC_AHB2ENR_ADCEN) || ADC1->CR & ADC_CR_DEEPPWD) {
        adc_init();
    }
    // Configure GPIO pin for analog input
    gpio_config_analog(pin, 0);

    ADC1->CR &= ~(ADC_CR_ADEN);
    ADC1_COMMON->CCR = 0;
    ADC1_COMMON->CCR |= 0x1 << ADC_CCR_CKMODE_Pos;

    ADC1->CFGR &= ~ADC_CFGR_RES;
    ADC1->CFGR |= 0x2 << ADC_CFGR_RES_Pos; // 8-bit resolution
    ADC1->SQR1 &= ADC_SQR1_L;
    ADC1->SQR1 |= 0x0 << ADC_SQR1_L_Pos; // 0 = 1 conversion

    ADC1->SQR1 |= channel << ADC_SQR1_SQ1_Pos;

    if(channel < 10){
        ADC1->SMPR1 &= ~(0x2 << channel*3);
    } else{
        ADC1->SMPR2 &= ~(0x2 << (channel-10)*3);
    }
    ADC1->CFGR &= ~ADC_CFGR_CONT;
    ADC1->CFGR &= ~(0x3 << ADC_CFGR_EXTEN_Pos);
    ADC1->ISR &= ~ADC_ISR_ADRDY;
    ADC1->CR |= ADC_CR_ADEN;
    while (!(ADC1->ISR & ADC_ISR_ADRDY));
}

unsigned int adc_read_single() 
{
    // Start a conversion
    ADC1->CR |= ADC_CR_ADSTART;
    // Wait for the conversion to finish
    while (!(ADC1->ISR & ADC_ISR_EOC));
    // Return the result
    return ADC1->DR;
}
