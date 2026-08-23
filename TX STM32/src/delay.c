#include <stm32l432xx.h>
#include "delay.h"

#define	CTRL_ENABLE		(1U << 0) // Bit 0
#define CTRL_TICKINT	(1U << 1) // Bit 1
#define CTRL_CLKSRC		(1U << 2) // Bit 2
#define CTRL_COUNTFLAG	(1U << 16) // Bit 16


void delay_init() {
	SysTick->LOAD = 3999;
	SysTick->VAL = 0;
	SysTick->CTRL |= CTRL_TICKINT;
	SysTick->CTRL |= CTRL_CLKSRC;
	SysTick->CTRL |= CTRL_ENABLE;
}

static volatile uint32_t micro_seconds = 0;

void SysTick_Handler(void){
	micro_seconds++;
}

void delay_ms(uint32_t time){
	__disable_irq();
	micro_seconds = 0;
	SysTick->VAL = 0;
	__enable_irq();
	while (micro_seconds < time);
}

// void delay_ms(uint32_t time){
// 	delay_us(time * 1000);
// }