#include <stm32l432xx.h>
#include <stdint.h>
#include <stdio.h>
#include "spi.h"
#include "gpio.h"
#include "delay.h"


void SPI1_init(void) {
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
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
    
    GPIO_config_alternate_function(MOSI, 5);
    GPIO_config_alternate_function(MISO, 5);
    GPIO_config_alternate_function(SCK, 5);
}

void SPI1_transmit(const uint8_t *data, uint8_t size) {
    for(uint8_t i = 0; i < size; i++) {
        while(!(SPI1->SR & SPI_SR_TXE)); // Wait for space in TX FIFO
        //printf("SPI1 SR 1: 0x%04x\n", (unsigned int)SPI1->SR);
        *(__IO uint8_t *)&SPI1->DR = data[i]; // Place byte in DR (TX FIFO)
        //printf("SPI1 SR 2: 0x%04x\n", (unsigned int)SPI1->SR);
        // delay(1) TXE flag is direct consequence of writing to DR thus delay not required.
        while (!(SPI1->SR & SPI_SR_RXNE));      // wait for the shifted-in byte
        (void)*(__IO uint8_t *)&SPI1->DR;
    }
}

void SPI1_receive(uint8_t *data, uint8_t size) {
    for (uint8_t i = 0; i < size; i++) {
        *((__IO uint8_t *)&SPI1->DR) = 0x00; // Have to send dummy byte for SCK
        while(!(SPI1->SR & SPI_SR_RXNE));
        data[i] = *((__IO uint8_t *)&SPI1->DR); // Read 8-bit
    }
}

void SPI1_flush_rx(void) {
    while(SPI1->SR & SPI_SR_BSY); // Wait for TX FIFO empty & shift reg idle
    while(SPI1->SR & SPI_SR_RXNE) {
        (void)*((__IO uint8_t *)&SPI1->DR);
    }
}