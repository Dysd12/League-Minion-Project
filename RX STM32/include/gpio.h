#ifndef GPIO_H_
#define GPIO_H_

#include <stm32l432xx.h>
#include <stdbool.h>

typedef enum {
	A0,A1,A2,A3,A4,A5,A6,A7,
	D0,D1,D2,D3,D4,D5,D6,D7,
	D8,D9,D10,D11,D12,D13
} STM32_Pin;

// GPIO modes
#define INPUT 0b00
#define OUTPUT 0b01
#define ALTERNATE_FUNCTION 0b10
#define ANALOG 0b11
    
// GPIO pullup modes
#define PULL_OFF 0b00
#define PULL_UP 0b01
#define PULL_DOWN 0b10


void gpio_config_mode(STM32_Pin pin, unsigned int mode);
void gpio_config_pullup(STM32_Pin pin, unsigned int mode);
void gpio_config_alternate_function(STM32_Pin pin, unsigned int function);
void gpio_write(STM32_Pin pin, bool value);
bool gpio_read(STM32_Pin pin);


#endif