#include "gpio.h"

/* Mapping of an STM32_Pin to its GPIO port */ 

static GPIO_TypeDef *get_GPIO_port[24] = {
    GPIOA, GPIOA, GPIOA, GPIOA,  // PA0,  PA1,  PA2,  PA3,
    GPIOA, GPIOA, GPIOA, GPIOA,  // PA4,  PA5,  PA6,  PA7,
    GPIOA, GPIOA, GPIOA, GPIOA,  // PA8,  PA9,  PA10, PA11,
    GPIOA, GPIOA, GPIOA, GPIOA,  // PA12, PA13, PA14, PA15,
    GPIOB, GPIOB, GPIOB, GPIOB,  // PB0,  PB1,  PB2,  PB3,
    GPIOB, GPIOB, GPIOB, GPIOB   // PB4,  PB5,  PB6,  PB7
};
    
static int get_GPIO_pin[24] = {
    0,  1,  2,  3,  // PA0,  PA1,  PA2,  PA3,
    4,  5,  6,  7,  // PA4,  PA5,  PA6,  PA7,
    8,  9,  10, 11, // PA8,  PA9,  PA10, PA11,
    12, 13, 14, 15, // PA12, PA13, PA14, PA15,
    0,  1,  2,  3,  // PB0,  PB1,  PB2,  PB3,
    4,  5,  6,  7   // PB4,  PB5,  PB6,  PB7
};

static void GPIO_enable_port(GPIO_TypeDef *GPIOx) {
    if      (GPIOx == GPIOA) RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
    else if (GPIOx == GPIOB) RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
    else if (GPIOx == GPIOC) RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;
    else if (GPIOx == GPIOH) RCC->AHB2ENR |= RCC_AHB2ENR_GPIOHEN;
}


/**
 * @brief Configure a GPIO pin as an output.
 * @param pin The Nucleo pin to configure.
 * @param otype The output type (0 for push-pull, 1 for open-drain)
 * @param ospeed The output speed (0 for low, 1 for medium, 2 for high, 3 for very high)
 */
void GPIO_config_output(Nucleo_pin pin, uint8_t otype, uint8_t ospeed) {
    GPIO_TypeDef *GPIOx = get_GPIO_port[pin];
    int pin_offset = get_GPIO_pin[pin];

    GPIO_enable_port(GPIOx);

    GPIOx->MODER &= ~(0x03 << pin_offset * 2); // Clear both mode bits
    GPIOx->MODER |=  (0x01 << pin_offset * 2);

    GPIOx->OTYPER &= ~(0x01 << pin_offset); // Clear output type bit
    GPIOx->OTYPER |=  (otype << pin_offset); // Set

    GPIOx->OSPEEDR &= ~(0x03 << pin_offset * 2); // Clear both speed bits
    GPIOx->OSPEEDR |=  (ospeed << pin_offset * 2);
}

/**
 * @brief Configure a GPIO pin as an input.
 * @param pin The Nucleo pin to configure.
 * @param pupd The pull-up/pull-down configuration (0 for no pull, 1 for pull-up, 2 for pull-down)
 */
void GPIO_config_input(Nucleo_pin pin, uint8_t pupd) {
    GPIO_TypeDef *GPIOx = get_GPIO_port[pin];
    int pin_offset = get_GPIO_pin[pin];

    GPIO_enable_port(GPIOx);

    GPIOx->MODER &= ~(0x03 << pin_offset * 2); // Clear both mode bits

    GPIOx->PUPDR &= ~(0x03 << pin_offset * 2); // Clear both pull-up/pull-down bits
    GPIOx->PUPDR |=  (pupd << pin_offset * 2); // Set pull-up/pull-down
}


void GPIO_config_alternate_function(Nucleo_pin pin, uint8_t func) {
    GPIO_TypeDef *GPIOx = get_GPIO_port[pin];
    int pin_offset = get_GPIO_pin[pin];

    GPIO_enable_port(GPIOx);
    
    GPIOx->MODER &= ~(0x03 << (pin_offset * 2)); // Clear both mode bits
    GPIOx->MODER |=  (0x02 << (pin_offset * 2)); // Set to alternate function mode

    if (pin_offset < 8) {
        GPIOx->AFR[0] &= ~(0x0F << pin_offset * 4);
        GPIOx->AFR[0] |=  (func << pin_offset * 4);
    } else {
        GPIOx->AFR[1] &= ~(0x0F << (pin_offset - 8) * 4);
        GPIOx->AFR[1] |=  (func << (pin_offset - 8) * 4);
    }
}

void GPIO_config_analog(Nucleo_pin pin) {
    GPIO_TypeDef *GPIOx = get_GPIO_port[pin];
    int pin_offset = get_GPIO_pin[pin];

    GPIO_enable_port(GPIOx);

    GPIOx->MODER &= ~(0x03 << (pin_offset * 2)); // Clear both mode bits
    GPIOx->MODER |=  (0x03 << (pin_offset * 2)); // Set to analog mode
}

void GPIO_write(Nucleo_pin pin, bool value) {
    GPIO_TypeDef *GPIOx = get_GPIO_port[pin];
    int pin_offset = get_GPIO_pin[pin];

    if(value){
      GPIOx->BSRR = 1 << pin_offset;
    } else{
      GPIOx->BRR = 1 << pin_offset; 
    }
}


bool GPIO_read(Nucleo_pin pin) {
    GPIO_TypeDef *GPIOx = get_GPIO_port[pin];
    int pin_offset = get_GPIO_pin[pin];

    return GPIOx->IDR & (1U << pin_offset);
}

