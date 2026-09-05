#ifndef TIMER_H_
#define TIMER_H_

#include <stm32l432xx.h>
#include <stdint.h>
#include <stdbool.h>
#include "gpio.h"


void tim1_init(uint32_t freq_Hz);
bool tim_config_pwm(TIM_TypeDef *TIMx, Nucleo_pin pin, unsigned int duty_cycle);


#endif