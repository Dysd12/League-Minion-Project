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
void SPI1_init(void);

/**
 * @brief Sends data from TX FIFO to peripheral.
 * @param data Pointer to data to be sent.
 * @param size Number of bytes to send.
 */
void SPI1_transmit(const uint8_t *data, uint8_t size);

/**
 * @brief Saves data from RX FIFO into a buffer.
 * @param data Pointer to where data is saved.
 * @param size Number of bytes to save.
 */
void SPI1_receive(uint8_t *data, uint8_t size);

/**
 * @brief Flushes the RX FIFO.
 */
void SPI1_flush_rx(void);

#endif