/**
 * Daniel Yeo
 * 2026/9/1
 */
#include <stm32l432xx.h>
#include "delay.h"

void delay_init() {
	SysTick->LOAD = 3999;
	SysTick->VAL = 0;
	SysTick->CTRL |= 0x1 << 1; // tick int
	SysTick->CTRL |= 0x1 << 2; // clock source
	SysTick->CTRL |= 0x1 << 0; // Enable
}

static volatile uint32_t micro_seconds = 0;

void SysTick_Handler(void)
{
	micro_seconds++;
}

void delay_ms(uint32_t time)
{
	__disable_irq();
	micro_seconds = 0;
	SysTick->VAL = 0;
	__enable_irq();
	while (micro_seconds < time);
}