/**
 * Daniel Yeo
 * 2026/9/1
 */
#include <stm32l432xx.h>
#include <stdint.h>
#include <stdio.h>
#include "spi.h"
#include "gpio.h"
#include "delay.h"

/**
 * Settings
 */
void spi1_init(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;

    SPI1->CR1 &= ~(SPI_CR1_CPHA | SPI_CR1_CPOL); // Mode 0 polarity
    SPI1->CR1 |= SPI_CR1_MSTR; // Master mode
    SPI1->CR1 &= ~SPI_CR1_BR;
    SPI1->CR1 |= SPI_CR1_BR_0 | SPI_CR1_BR_1 | SPI_CR1_BR_2; // fpclk/16 Baud Rate
    SPI1->CR1 |= SPI_CR1_SSI | SPI_CR1_SSM; // SSM
    SPI1->CR2 &= ~SPI_CR2_DS;
    SPI1->CR2 |= SPI_CR2_DS_0 | SPI_CR2_DS_1 | SPI_CR2_DS_2; // 8-bit data
    SPI1->CR2 |= SPI_CR2_FRXTH; // 8-bit data threshold
    SPI1->CR1 |= SPI_CR1_SPE; // Enable SPI
    
    gpio_config_alternate_function(MOSI, 5);
    gpio_config_alternate_function(MISO, 5);
    gpio_config_alternate_function(SCK, 5);
}

void spi_transmit(SPI_TypeDef *SPIx, const uint8_t *data, unsigned int size)
{
    for (unsigned int i = 0; i < size; i++) {
        while (!(SPIx->SR & SPI_SR_TXE)); // Wait for HW to set TXE to 1
        *(__IO uint8_t *)&SPIx->DR = data[i]; // HW sets TXE to 0
        // Delay so code doesn't run again before TXE set to 0
        // Could allow data to be put into Tx FIFO (or DR?) while it is full.
        for (volatile int delay = 0; delay < 200; delay++); // 200 = 0.5 ms
    }
}

void spi_receive(SPI_TypeDef *SPIx, uint8_t *data, unsigned int size)
{
    for (unsigned int i = 0; i < size; i++) {
        *((__IO uint8_t *)&SPIx->DR) = 0x00;

        while (!(SPIx->SR & SPI_SR_RXNE)); // Wait for HW to set RXNE to 1
        data[i] = *((__IO uint8_t *)&SPIx->DR); // HW sets RXNE to 0
        // Delay so code doesn't run again before RXNE set to 0
        // Could allow data to be read from Rx FIFO (DR) with no data in it.
        for (volatile int delay = 0; delay < 200; delay++);
    }
}

void spi_flush_rx(SPI_TypeDef *SPIx)
{
    while (SPIx->SR & SPI_SR_BSY); // Wait for TX FIFO empty & shift reg idle
    while (SPIx->SR & SPI_SR_RXNE) { // Wait for RXNE to be 1
        (void)*((__IO uint8_t *)&SPIx->DR);
        // No delay because even if RXNE = 1 while nothing in the DR, nothing bad should happen.
    }
}