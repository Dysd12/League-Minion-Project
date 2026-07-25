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
void spi_init(void);

/**
 * @brief Sends data from TX FIFO to peripheral.
 * @param data Pointer to data to be sent.
 * @param size Number of bytes to send.
 */
void spi1_transmit(const uint8_t *data, unsigned int size);

/**
 * @brief Saves data from RX FIFO into a buffer.
 * @param data Pointer to where data is saved.
 * @param size Number of bytes to save.
 */
void spi1_receive(uint8_t *data, unsigned int size);

/**
 * @brief Flushes the RX FIFO.
 */
void spi1_flush_rx(void);

#endif