#ifndef ADC_H_
#define ADC_H_

#include "gpio.h"

void adc_init(void);
void adc_config_single(Nucleo_pin pin);
unsigned int adc_read_single(void);

#endif