#include <stm32l432xx.h>
#include <stdint.h>
#include <stdio.h>
#include "spi.h"
#include "gpio.h"
#include "delay.h"


void spi_init(void) {
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
    /* CR1 */
    // Bits 0,1: Mode 0
    SPI1->CR1 &= ~(SPI_CR1_CPHA | SPI_CR1_CPOL);
    // Bit 2: Master Mode
    SPI1->CR1 |= SPI_CR1_MSTR;
    // Bits 3,4,5: Baud Rate fpclk/16
    SPI1->CR1 &= ~SPI_CR1_BR;
    SPI1->CR1 |= SPI_CR1_BR_0 | SPI_CR1_BR_1 | SPI_CR1_BR_2;
    // Bits 8,9: SSM and SSI
    SPI1->CR1 |= SPI_CR1_SSI | SPI_CR1_SSM;
    /* CR2 */
    // Bits 8,9,10,11: Data Size (8-bit)
    SPI1->CR2 &= ~SPI_CR2_DS;
    SPI1->CR2 |= SPI_CR2_DS_0 | SPI_CR2_DS_1 | SPI_CR2_DS_2;
    // Bit 12: RXNE Threshold (8-bits)
    SPI1->CR2 |= SPI_CR2_FRXTH;
    // Spi Enable
    SPI1->CR1 |= SPI_CR1_SPE;
    
    // Pin D11/PB5/MOSI
    GPIOB->MODER   &= ~(0x3UL << 5*2);
    GPIOB->MODER   |=  (0x2UL << 5*2);
    GPIOB->OTYPER  &= ~(0x1UL << 5*1);
    GPIOB->OSPEEDR &= ~(0x3UL << 5*2);
    GPIOB->OSPEEDR |=  (0x3UL << 5*2);
    GPIOB->AFR[0]  &= ~(0xFUL << 5*4);
    GPIOB->AFR[0]  |=  (0x5UL << 5*4);
    // Pin D12/PB4/MISO
    GPIOB->MODER   &= ~(0x3UL << 4*2);
    GPIOB->MODER   |=  (0x2UL << 4*2);
    GPIOB->OTYPER  &= ~(0x1UL << 4*1);
    GPIOB->OSPEEDR &= ~(0x3UL << 4*2);
    GPIOB->OSPEEDR |=  (0x3UL << 4*2);
    GPIOB->AFR[0]  &= ~(0xFUL << 4*4);
    GPIOB->AFR[0]  |=  (0x5UL << 4*4);
    // Pin D13/PB3/SCK
    GPIOB->MODER   &= ~(0x3UL << 3*2);
    GPIOB->MODER   |=  (0x2UL << 3*2);
    GPIOB->OTYPER  &= ~(0x1UL << 3*1);
    GPIOB->OSPEEDR &= ~(0x3UL << 3*2);
    GPIOB->OSPEEDR |=  (0x3UL << 3*2);
    GPIOB->AFR[0]  &= ~(0xFUL << 3*4);
    GPIOB->AFR[0]  |=  (0x5UL << 3*4);
    // printf("CR1: 0x%04X  (Expecting: 0x035C)\n", (unsigned int)SPI1->CR1);
    // printf("CR2: 0x%04X  (Expecting 0x1700)\n", (unsigned int)SPI1->CR2);
}

void spi1_transmit(const uint8_t *data, unsigned int size) {
    for(int i = 0; i < size; i++) {
        /* 1. Wait for space in TX FIFO. */
        while(!(SPI1->SR & SPI_SR_TXE));
        /* 2. Write byte to the SPI's TX FIFO. */
        *(__IO uint8_t *)&SPI1->DR = data[i];
        delay_ms(1);
    }
}

void spi1_receive(uint8_t *data, unsigned int size) {
    for (int i = 0; i < size; i++) {
        /* 1. Send out a byte to pulse the SCK and receive a byte. */
        *((__IO uint8_t *)&SPI1->DR) = 0x00;
        /* 2. Wait for all eight bits.*/
        while(!(SPI1->SR & SPI_SR_RXNE));
        /* 3. Read in the received byte. */
        data[i] = *((__IO uint8_t *)&SPI1->DR); // Read 8-bit
    }
}

void spi1_flush_rx(void) {
    /* 1. Wait for the shift register to finish shifting in/out bits. */
    while(SPI1->SR & SPI_SR_BSY);
    /* 2. Empty the SPI's RX FIFO by reading in all the bytes. */
    while(SPI1->SR & SPI_SR_RXNE) {
        (void)*((__IO uint8_t *)&SPI1->DR);
    }
}