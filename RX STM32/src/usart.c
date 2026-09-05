/**
 * Daniel Yeo
 * 2026/9/1
 */
#include <stm32l432xx.h>
#include <stdbool.h>
#include <stdint.h>
#include "usart.h"

/**
 * Settings
 * 8 data bits
 * 1 stop bit
 * 16x Rx Oversampling
 * Tx & Rx Enabled
 */
void usart2_init(unsigned int baud) {
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
    GPIOA->MODER &= ~((0x3 << 2*2) | (0x3 << 2*15));
    GPIOA->MODER |= (0x2 << 2*2) | (0x2 << 2*15); // AF mode for PA2 & PA15
    GPIOA->AFR[0] &= ~(0xF << 4*2);
    GPIOA->AFR[0] |= (0x7 << 4*2); // AF 7 for PA2
    GPIOA->AFR[1] &= ~(0xF << 4*7);
    GPIOA->AFR[1] |= (0x3 << 4*7); // AF 3 for PA15

    GPIOA->OSPEEDR |= (0x3 << 2*2) | (0x3 << 2*15); // High-speed
    GPIOA->OTYPER &= ~((0x3 << 2*2) | (0x3 << 2*15)); // Push-pull
    GPIOA->PUPDR &= ~((0x3 << 2*2) | (0x3 << 2*15));
    GPIOA->PUPDR |= (0x1 << 2*2) | (0x1 << 2*15); // Pull-down

    RCC->APB1ENR1 |= RCC_APB1ENR1_USART2EN; // Enable USART2 clock
    // Select SYSCLK as the USART2 clock source. The reset default is PCLK1;
    // we usually set both SYSCLK and PCLK1 to 80MHz anyway.
    RCC->CCIPR &= ~RCC_CCIPR_USART2SEL;
    RCC->CCIPR |=  RCC_CCIPR_USART2SEL_0;

    USART2->CR1 &= ~USART_CR1_UE; // Disable USART

    // Set baudrate as desired. This is done by dividing down the APB1 clock.
    extern uint32_t SystemCoreClock;
    uint32_t val = SystemCoreClock / baud;
    USART2->BRR = val;

    USART2->CR1 &= ~USART_CR1_M; // 8 data bits
    USART2->CR2 &= ~USART_CR2_STOP; // 1 stop bit
    USART2->CR1 &= ~USART_CR1_OVER8; // Oversampling by 16 (Rx runs 16x faster)
	USART2->CR1 |= USART_CR1_TE; // TX enable
	USART2->CR1 |= USART_CR1_RE; // RX enable

    USART2->CR1  |= USART_CR1_UE;               
	
	while ((USART2->ISR & USART_ISR_TEACK) == 0);
	while ((USART2->ISR & USART_ISR_REACK) == 0);
}

// Very basic function: send a character string to the UART, one byte at a time.
// Spin wait after each byte until the UART is ready for the next byte.
void usart_transmit(USART_TypeDef *USARTx, const uint8_t *data, unsigned int size)
{
    for (unsigned int i = 0; i < size; i++) {
        while (!(USARTx->ISR & USART_ISR_TXE)); // Wait until TXE = 1
        USARTx->TDR = data[i] & 0xFF; // Write to TDR for TXE = 1 -> TXE = 0
        // Wait 300us for TXE = 1 -> TXE = 0
        for (volatile int delay = 0; delay < 3000/7; delay++);
    }
    // Only necessary when shutting off UART
    while (!(USARTx->ISR & USART_ISR_TC));
    USARTx->ISR &= ~USART_ISR_TC;
}

void usart_receive(USART_TypeDef *USARTx, uint8_t *data, unsigned int size) 
{
    for (unsigned int i = 0; i < size; i++) {
        while (!(USARTx->ISR & USART_ISR_RXNE)); // wait for RXNE to be set (1)
        data[i] = USARTx->RDR & 0xFF;

        for (volatile int delay = 0; delay < 3000/7; delay++);
    }
}
