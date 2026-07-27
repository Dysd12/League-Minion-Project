#include <stm32l432xx.h>
#include "delay.h"

#define	CTRL_ENABLE		(1U<<0) // Bit 0
#define CTRL_TICKINT	(1U<<1) // Bit 1
#define CTRL_CLKSRC		(1U<<2) // Bit 2
#define CTRL_COUNTFLAG	(1U<<16) // Bit 16


void delay_init() {
	/* Set Systick load register: Set to ? */
	SysTick->LOAD = 3999;
	/* Set Systick current value register: Cleared to 0 */
	SysTick->VAL = 0;
	/* Enable/Disable SysTick interrupt -> Enabled */
	SysTick->CTRL |= CTRL_TICKINT;
	/* Select internal clock source: Set to Core Clock */
	SysTick->CTRL |= CTRL_CLKSRC;
	/* Enable/Disable SysTick -> Enabled */
	SysTick->CTRL |= CTRL_ENABLE;
}

void delay_ms(uint32_t time){
	SysTick->VAL = 0; // Clear current value register
	uint64_t start = millis();
	while((millis() - start) < time);
}

volatile uint64_t mil;

uint64_t millis(void){
	__disable_irq();
	uint64_t ml = mil;
	__enable_irq();
	return ml;
}

void SysTick_Handler(void){
	mil++;
}