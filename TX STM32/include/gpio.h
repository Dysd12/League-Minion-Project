#ifndef GPIO_H_
#define GPIO_H_

#include <stm32l432xx.h>
#include <stdbool.h>

// _x means no Nucleo pin exists for the corresponding STM32 pin.
typedef enum {
	A0  = 0,   A1  = 1,   A7 = 2,   A2  = 3,   // PA0,  PA1,  PA2,  PA3,
	A3  = 4,   A4  = 5,   A5 = 6,   A6  = 7,   // PA4,  PA5,  PA6,  PA7,
	D9  = 8,   D1  = 9,   D0 = 10,  D10 = 11,  // PA8,  PA9,  PA10, PA11,
	D2  = 12,  _0  = -1,  _1 = -1,  _2  = -1,  // PA12, PA13, PA14, PA15,
	D3  = 16,  D6  = 17,  _3 = -1,  D13 = 19,  // PB0,  PB1,  PB2,  PB3,
	D12 = 20,  D11 = 21,  D5 = 22,  D4  = 23   // PB4,  PB5,  PB6,  PB7
} Nucleo_pin;


void GPIO_config_output(Nucleo_pin pin, uint8_t otype, uint8_t ospeed);
void GPIO_config_input(Nucleo_pin pin, uint8_t pupd);
void GPIO_config_alternate_function(Nucleo_pin pin, uint8_t func);
void GPIO_config_analog(Nucleo_pin pin);
bool GPIO_read(Nucleo_pin pin);
void GPIO_write(Nucleo_pin pin, bool value);


#endif