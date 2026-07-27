#include "gpio.h"

/* Mapping of an STM32_Pin to its GPIO port */ 

static const GPIO_TypeDef *get_GPIO_port[24] = {
    GPIOA, GPIOA, GPIOA, GPIOA,  // PA0,  PA1,  PA2,  PA3,
    GPIOA, GPIOA, GPIOA, GPIOA,  // PA4,  PA5,  PA6,  PA7,
    GPIOA, GPIOA, GPIOA, GPIOA,  // PA8,  PA9,  PA10, PA11,
    GPIOA, GPIOA, GPIOA, GPIOA,  // PA12, PA13, PA14, PA15,
    GPIOB, GPIOB, GPIOB, GPIOB,  // PB0,  PB1,  PB2,  PB3,
    GPIOB, GPIOB, GPIOB, GPIOB   // PB4,  PB5,  PB6,  PB7
};
    
static const int get_GPIO_pin[24] = {
    0,  1,  2,  3,  // PA0,  PA1,  PA2,  PA3,
    4,  5,  6,  7,  // PA4,  PA5,  PA6,  PA7,
    8,  9,  10, 11, // PA8,  PA9,  PA10, PA11,
    12, 13, 14, 15, // PA12, PA13, PA14, PA15,
    0,  1,  2,  3,  // PB0,  PB1,  PB2,  PB3,
    4,  5,  6,  7   // PB4,  PB5,  PB6,  PB7
};

// Enables a GPIO port (A, B, C, or H) by setting the appropriate bit in the RCC
// clock enable register.
//   gpio: Pointer to GPIO port to enable, one of GPIOA, GPIOB, GPIOC, GPIOH
static void gpio_enable_port(GPIO_TypeDef *GPIOx) {
    if      (GPIOx == GPIOA) RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
    else if (GPIOx == GPIOB) RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
    else if (GPIOx == GPIOC) RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;
    else if (GPIOx == GPIOH) RCC->AHB2ENR |= RCC_AHB2ENR_GPIOHEN;
}

// Configure the direction for a given GPIO pin
//   pin: A Nucleo pin ID (D2, A4, etc.)
//   direction: One of INPUT (0b00) or OUTPUT (0b01).  Other modes are invalid.
// Returns void_INVALID_CONFIG for invalid direction value, otherwise
// returns void_OK.
void gpio_config_mode(Nucleo_pin pin, unsigned int mode) {
    GPIO_TypeDef *port = get_GPIO_port[pin];
    int pin_offset = get_GPIO_pin[pin];

    gpio_enable_port(port);

    port->MODER &= ~(0b11 << pin_offset*2); // Clear both mode bits
    port->MODER |=  (mode << pin_offset*2);
}

// Configure the pullup mode for a given GPIO pin.  This is only meaningful when
// the pin is configured as an input, but it will set the registers regardless
// of mode.
//   pin: A Nucleo pin ID (D2, A4, etc.)
//   mode: One of PULL_OFF (0b00), PULL_UP (0b01) or PULL_DOWN (0b10).  Other
//         modes are invalid.
// Returns void_INVALID_CONFIG for invalid pullup mode value, otherwise
// returns void_OK.
void gpio_config_pullup(Nucleo_pin pin, unsigned int mode) {
    GPIO_TypeDef* port = get_GPIO_port[pin];
    int pin_offset = get_GPIO_pin[pin];

    // if(mode & ~0b11UL){ // Only bottom two bits are are valid
    //     return void_INVALID_CONFIG;
    // }
    port->PUPDR &= ~(0b11 << pin_offset*2); // Clear both mode bits
    port->PUPDR |=  (mode << pin_offset*2);
}

// Configure a GPIO pin for one of its "alternate functions".  See Tables 15 and
// 16 of the STM32L432KC datasheet for a complete listing of the alternate
// functions for each pin.
//   pin: A Nucleo pin ID (D2, A4, etc.)
//   function: an integer 0-15 to select the alternate function
// Always returns void_OK; in the future this may return errors for
// invalid configurations.
void gpio_config_alternate_function(Nucleo_pin pin, unsigned int function) {
    GPIO_TypeDef* port = get_GPIO_port[pin];
    int pin_offset = get_GPIO_pin[pin];

    unsigned int afr_offset = pin_offset * 4; // 4 bits per pin
    // keep it to AFR[0] or AFR[1], and each is 32-bits wide. So pin offset is at
    // most 7 (0-7 = 8 pins) 
    port->AFR[afr_offset >> 5] &= ~(0b1111 << (0b11111 & afr_offset));
    port->AFR[afr_offset >> 5] |=  (function << (0b11111 & afr_offset));
}

// Set the value of a single GPIO output pin.
//   pin: A Nucleo pin ID (D2, A4, etc.)
//   value: Boolean 0 or 1 to send to the pin
void gpio_write(Nucleo_pin pin, bool value) {
    GPIO_TypeDef* port = get_GPIO_port[pin];
    int pin_offset = get_GPIO_pin[pin];

    if(value){
      port->BSRR = 1 << pin_offset;
    } else{
      port->BRR = 1 << pin_offset; 
    }
}

// Read the value of a single GPIO pin.  This is only meaningful if the pin is
// configured as an input.
//   pin: A Nucleo pin ID (D2, A4, etc.)
// Returns a boolean, indicating the value (0/low or 1/high) of the pin
bool gpio_read(Nucleo_pin pin) {
    GPIO_TypeDef* port = get_GPIO_port[pin];
    int pin_offset = get_GPIO_pin[pin];

    return (port->IDR >> pin_offset) & 1UL;
}

