#ifndef TIMER_H_
#define TIMER_H_

#include <stm32l432xx.h>
#include <stdint.h>
#include <stdbool.h>
#include "gpio.h"


void TIM1_init(uint32_t freq_hz);
bool TIM1_config_pwm(Nucleo_pin pin, unsigned int duty_cycle);

#endif

