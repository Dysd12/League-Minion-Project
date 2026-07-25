#include "gpio.h"

/* Mapping of an STM32_Pin to its GPIO port */ 
GPIO_TypeDef* g_GPIO_port[D13+1] = {
    GPIOA,GPIOA,GPIOA,GPIOA,  // A0=PA0,A1=PA1,A2=PA3,A3=PA4
    GPIOA,GPIOA,GPIOA,GPIOA,  // A4=PA5,A5=PA6,A6=PA7,A7=PA2
    GPIOA,GPIOA,GPIOA,GPIOB,  // D0=PA10,D1=PA9,D2=PA12,D3=PB0
    GPIOB,GPIOB,GPIOB,GPIOC,  // D4=PB7,D5=PB6,D6=PB1,D7=PC14
    GPIOC,GPIOA,GPIOA,GPIOB,  // D8=PC15,D9=PA8,D10=PA11,D11=PB5
    GPIOB,GPIOB               // D12=PB4,D13=PB3.
};

// Mapping of an STM32_Pin to its GPIO pin 
uint8_t g_GPIO_pin[D13+1] = {
    0,1,3,4,    // A0=PA0,A1=PA1,A2=PA3,A3=PA4
    5,6,7,2,    // A4=PA5,A5=PA6,A6=PA7,A7=PA2
    10,9,12,0,  // D0=PA10,D1=PA9,D2=PA12,D3=PB0
    7,6,1,14,   // D4=PB7,D5=PB6,D6=PB1,D7=PC14
    15,8,11,5,  // D8=PC15,D9=PA8,D10=PA11,D11=PB5
    4,3         // D12=PB4,D13=PB3.
};

// Enables a GPIO port (A, B, C, or H) by setting the appropriate bit in the RCC
// clock enable register.
//   gpio: Pointer to GPIO port to enable, one of GPIOA, GPIOB, GPIOC, GPIOH
static void gpio_enable_port(GPIO_TypeDef *port) {
    if      (port == GPIOA) RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
    else if (port == GPIOB) RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
    else if (port == GPIOC) RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;
    else                    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOHEN;
}


// Configure the direction for a given GPIO pin
//   pin: A Nucleo pin ID (D2, A4, etc.)
//   direction: One of INPUT (0b00) or OUTPUT (0b01).  Other modes are invalid.
// Returns void_INVALID_CONFIG for invalid direction value, otherwise
// returns void_OK.
void gpio_config_mode(STM32_Pin pin, unsigned int mode) {
    GPIO_TypeDef *port = g_GPIO_port[pin];
    uint8_t pin_offset = g_GPIO_pin[pin];

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
void gpio_config_pullup(STM32_Pin pin, unsigned int mode) {
    GPIO_TypeDef* port = g_GPIO_port[pin];
    uint8_t pin_offset = g_GPIO_pin[pin];

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
void gpio_config_alternate_function(STM32_Pin pin, unsigned int function) {
    GPIO_TypeDef* port = g_GPIO_port[pin];
    uint8_t pin_offset = g_GPIO_pin[pin];

    unsigned int afr_offset = pin_offset * 4; // 4 bits per pin
    // keep it to AFR[0] or AFR[1], and each is 32-bits wide. So pin offset is at
    // most 7 (0-7 = 8 pins) 
    port->AFR[afr_offset >> 5] &= ~(0b1111 << (0b11111 & afr_offset));
    port->AFR[afr_offset >> 5] |=  (function << (0b11111 & afr_offset));
}

// Set the value of a single GPIO output pin.
//   pin: A Nucleo pin ID (D2, A4, etc.)
//   value: Boolean 0 or 1 to send to the pin
void gpio_write(STM32_Pin pin, bool value) {
    GPIO_TypeDef* port = g_GPIO_port[pin];
    uint8_t pin_offset = g_GPIO_pin[pin];

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
bool gpio_read(STM32_Pin pin) {
    GPIO_TypeDef* port = g_GPIO_port[pin];
    uint8_t pin_offset = g_GPIO_pin[pin];

    return (port->IDR >> pin_offset) & 1UL;
}

