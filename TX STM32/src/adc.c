#include <stm32l432xx.h>
#include "adc.h"
#include "gpio.h"



// g_ADC_channel[pin] returns the integer mapping which regular ADC channel is
// mapped to 'pin' (which should be of type STM32_Pin). We take advantage of
// enum being equivalent to int. Note that we don't handle the special ADC
// internal channels (temperature sensor and voltage reference).
static int g_ADC_channel[24] = { 
    5,  6,  7,  8,  // PA0,  PA1,  PA2,  PA3,
    9,  10, 11, 12, // PA4,  PA5,  PA6,  PA7,
    0,  0,  0,  0,  // PA8,  PA9,  PA10, PA11,
    12, 13, 14, 15, // PA12, PA13, PA14, PA15,
    15, 16, 0,  0,  // PB0,  PB1,  PB2,  PB3,
    0,  0,  0,  0   // PB4,  PB5,  PB6,  PB7
};


// Performs initial steps to turn on the ADC: Resets the ADC, takes it out of
// deep-power-down mode, enables the internal voltage regulator, and waits for
// the voltage to stabilize.  This is the first function called from
// adc_config_single(); it should not need to be called directly from user code.
void adc_init(void) {
    // According to 16.4.10, you should only write the ADC RCC bits if the ADC
    // is disabled. It's disabled out of reset, so this code is likely unneeded.
    ADC1->CR &= ~ADC_CR_ADEN;

    // Enable the clock of ADC
    // RCC_AHB2ENR is the AHB2 peripheral-clock-enable register. 
    // ADCEN enables the clock to all ADCs.
    RCC->AHB2ENR  |= RCC_AHB2ENR_ADCEN;

    // RCC_AHB2RSTR is the AHB2 peripheral-reset register.
    // Put the ADC into reset, wait, and take it out, using RCC_AHB2RSTR.ADCRST
    RCC->AHB2RSTR    |=  RCC_AHB2RSTR_ADCRST; // Go into reset.
    for (volatile int i=0; i<5; i++) {}       // Wait till we're really in reset.
    RCC->AHB2RSTR    &= ~RCC_AHB2RSTR_ADCRST; // Come out of reset.
    for (volatile int i=0; i<5; i++) {}       // Wait till we're really out of reset

    // Reset leaves us in deep-power-down mode, so exit it now.
    if ((ADC1->CR & ADC_CR_DEEPPWD) == ADC_CR_DEEPPWD){
        ADC1->CR &= ~ADC_CR_DEEPPWD;
    }

    // Enable the ADC internal voltage regulator
    ADC1->CR |= ADC_CR_ADVREGEN;    

    // Wait for ADC voltage regulator start-up time (RM0394 16.4.6). This is
    // T_ADCVREG_STUP, which the L432 datasheet (p115) lists as 20us.
    // With an 80MHz clock, that's 20*80 cycles. If our clock is slower, then
    // we wait a bit too long... no big deal since we do this once at initialization.
    for(volatile int wait_time = 1600; wait_time > 0; wait_time--) {}
}

// Configure the ADC for a single reading on one channel.  Call
// adc_read_single() after this function to perform the read.  This is the
// simplest way to use the ADC, but also the lowest performance.
//   pin: A Nucleo pin ID (D2, A4, etc.)
// Returns STM32_Error_INVALID_CONFIG if the pin is not ADC-capable; otherwise
// returns STM32_Error_OK.
void adc_config_single(Nucleo_pin pin) {
    // Map the pin to the ADC channel
    int channel = g_ADC_channel[pin];
    if(channel < 0){
        //return STM32_Error_INVALID_CONFIG;
    }

    // If the ADC is off, turn it on before continuing!
    if(!(RCC->AHB2ENR & RCC_AHB2ENR_ADCEN) || ADC1->CR & ADC_CR_DEEPPWD){
        adc_init();
    }

    // Configure GPIO pin for analog input
    GPIO_config_analog(pin);
    //gpio_config_pullup(pin, PULL_OFF);

    // Disable ADC so we can configure settings (Section 16.4.9)
    ADC1->CR &= ~(ADC_CR_ADEN);

    // I/O analog switches voltage booster -- If we're running the ADC with a
    // low reference / supply voltage, setting this lets the ADC sample a little
    // faster.  On the Nucleo32, the ADC supply is solder-bridged to 3.3V, so
    // the supply voltage is always "high" and this is not needed.
    // SYSCFG->CFGR1 |= SYSCFG_CFGR1_BOOSTEN;

    // Reset all bits in the ADC "common" register
    // The L4XXX series can have multiple ADCs and some configuration is shared
    // between them.  On the L432KC there is only one, so the distinction is moot.
    ADC1_COMMON->CCR = 0;

    // Clock configuration: the ADC can get its clock from one of several sources
    // in order to match a desired conversion rate.

    // Configure prescaler
    // ADC1_COMMON->CCR |=  // leave at 0

    // Set clock source to be HCLK/1
    ADC1_COMMON->CCR |= 0b01 << ADC_CCR_CKMODE_Pos;

    // If we had a chip with multiple ADCs, we'd want to set them to run
    // independently.  L432 only has one, so do nothing here.
    
    // Configure ADC resolution (section 16.7.4 of reference)
    ADC1->CFGR &= ~ADC_CFGR_RES;
    ADC1->CFGR |= 0b10 << ADC_CFGR_RES_Pos; // 8-bit resolution
    
    // Set alignment
    // Leave at 0 (right-aligned), put result in least significant end of register

    // Configure the conversion sequence
    // The ADC can run a sequence of up to 16 conversions in one go; here we'll
    // just configure a single conversion, but this could be extended to support
    // multiple channels.
    // First, set the number of conversions in the sequence
    ADC1->SQR1 &= ADC_SQR1_L;
    ADC1->SQR1 |= 0x0 << ADC_SQR1_L_Pos; // 0 = 1 conversion

    // Next, program the actual sequence
    ADC1->SQR1 |= channel << ADC_SQR1_SQ1_Pos;
    // If there were additional channels in the sequence, we'd configure them here,
    // in the SQR1-4 registers.

    // Set mode as single-ended (i.e., pin voltage relative to chip ground)
    // Differential mode would measure the voltage between two pins
    //ADC1->DIFSEL &= ~(0b1 << channel); // Leave at 0

    // Select ADC sample time.  The lower the impedance (resistance) of whatever
    // is connected to the ADC pin, the faster the ADC's sample capacitor will
    // charge, and the shorter we can make this time.  See section 16.4.12 in
    // the reference manual for the bit values and table 64 in the L432
    // datasheet for the corresponding maximum impedance.
    if(channel < 10){
        ADC1->SMPR1 &= ~(0b010 << channel*3);
    }
    else{
        ADC1->SMPR2 &= ~(0b010 << (channel-10)*3);
    }
    // Every channel can have its own sample time, so configure that as necesssary.


    // Select single conversion mode (make one pass through the sequence and
    // stop, rather than running it continuously).  Note that "discontinuous
    // mode" (DISCEN) is a separate thing, where only parts of a longer sequence
    // get sampled.
    ADC1->CFGR &= ~ADC_CFGR_CONT;

    // Disable hardware trigger, we'll launch conversions with software
    ADC1->CFGR &= ~(0b11 << ADC_CFGR_EXTEN_Pos);

    // Clear ARRDY (ADC ready), hardware will set it when it's good to go
    ADC1->ISR &= ~ADC_ISR_ADRDY;

    // Enable ADC again
    ADC1->CR |= ADC_CR_ADEN;

    // Wait for ADRDY indicating the ADC is ready
    // ISR is the "Interrupt and Status Register"
    while(!(ADC1->ISR & ADC_ISR_ADRDY)) {}

    //return STM32_Error_OK;
}

// Request a single sample from the ADC, wait for the conversion to finish, and
// return the result.  adc_config_single() should be called before this
// function to select the channel, resolution, etc.  Once a pin is configured,
// this function can be called many times to request samples on the same pin.
// Returns the value from the ADC.  Range and alignment are determined by
// adc_config_single().
unsigned int adc_read_single() {
    // Start a conversion
    ADC1->CR |= ADC_CR_ADSTART;

    // Wait for the conversion to finish
    while(!(ADC1->ISR & ADC_ISR_EOC)) {}

    // Return the result
    return ADC1->DR;
}
