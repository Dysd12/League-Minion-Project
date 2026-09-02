#ifndef SPI_H_
#define SPI_H_

#include <stdint.h>
#include "gpio.h"

/* Pins */
#define MOSI    D11
#define MISO    D12
#define SCK     D13

/**
 * @brief Initializes the SPI peripheral.
 */
void spi1_init(void);
/**
 * @brief Sends data from TX FIFO to peripheral.
 * @param data Pointer to data to be sent.
 * @param size Number of bytes to send.
 */
void spi_transmit(SPI_TypeDef *SPIx, const uint8_t *data, unsigned int size);
/**
 * @brief Saves data from RX FIFO into a buffer.
 * @param data Pointer to where data is saved.
 * @param size Number of bytes to save.
 */
void spi_receive(SPI_TypeDef *SPIx, uint8_t *data, unsigned int size);
/**
 * @brief Flushes the RX FIFO.
 */
void spi_flush_rx(SPI_TypeDef *SPIx);


#endif