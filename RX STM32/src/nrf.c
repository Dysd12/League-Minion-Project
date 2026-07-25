/**
 * Author: Daniel Yeo
 * Date: 7/17/2026
 * File: nrf.c
 * Desc: Implementation file for the nRF24L01+ module. 
 */

#include <stm32l432xx.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <stddef.h>
#include "nrf.h"
#include "delay.h"
#include "spi.h"
#include "gpio.h"

static void set_ce(bool ce) {
    gpio_write(nRF24_CE, ce);
}

static void set_csn(bool csn) {
    gpio_write(nRF24_CSN, csn);
}

static void nRF24_write(uint8_t cmd, const uint8_t *data, unsigned int size) {
    cmd |= 0x20;
    //uint8_t temp = *data;
    set_csn(0);
    spi1_transmit(&cmd, 1);
    spi1_transmit(data, size);
    delay_ms(1); // Need delay to ensure the nRF24 has time to process the command
    set_csn(1);
}

static void nRF24_read(uint8_t cmd, uint8_t *data, unsigned int size) {
    set_csn(0);
    spi1_transmit(&cmd, 1);
    spi1_flush_rx();
    spi1_receive(data, size);
    delay_ms(1); // Need delay to ensure the nRF24 has time to process the command
    set_csn(1);
}

static void nRF24_cmd(uint8_t cmd) {
    set_csn(0);
    spi1_transmit(&cmd, 1);
    delay_ms(1);
    set_csn(1);
}

static void nRF24_print_registers(bool tx_mode) {

    if(tx_mode) {
        printf("=== TX MODE ===\n");
    } else {
        printf("=== RX MODE ===\n");
    }
    uint8_t val;

    nRF24_read(EN_AA,      &val, 1);
    printf("nRF24 EN_AA:      0x%02X (Expected: 0x00, Reset: 0x3F)\n", val);

    nRF24_read(SETUP_AW,   &val, 1);
    printf("nRF24 SETUP_AW:   0x%02X (Expected: 0x03, Reset: 0x03)\n", val);

    nRF24_read(SETUP_RETR, &val, 1);
    printf("nRF24 SETUP_RETR: 0x%02X (Expected: 0x00, Reset: 0x03)\n", val);

    nRF24_read(RF_CH,      &val, 1);
    printf("nRF24 RF_CH:      0x%02X (Expected: 0x0A, Reset: 0x02)\n", val);

    nRF24_read(RF_SETUP,   &val, 1);
    printf("nRF24 RF_SETUP:   0x%02X (Expected: 0x0E, Reset: 0x0F)\n", val);

    nRF24_read(STATUS, &val, 1);
    printf("nRF24 STATUS:     0x%02X (Expected: 0x0E, Reset: 0x0E)\n", val);

    nRF24_read(FIFO_STATUS, &val, 1);
    printf("nRF24 FIFO_STATUS: 0x%02X (Expected: 0x11, Reset: 0x11)\n\n", val);

    if(tx_mode) {
        nRF24_read(CONFIG,     &val, 1);
        printf("nRF24 CONFIG:     0x%02X (Expected: 0x0E, Reset: 0x08)\n", val);

        uint8_t tx_addr[5] = {0};
        nRF24_read(TX_ADDR, tx_addr, 5);
        printf("nRF24 TX_ADDR:    0x%02X%02X%02X%02X%02X (Expected: 0xAABBCCDDEE, Reset: E7E7E7E7E7)\n",
            tx_addr[4], tx_addr[3], tx_addr[2], tx_addr[1], tx_addr[0]); 
    } else {
        nRF24_read(CONFIG, &val, 1);
        printf("nRF24 CONFIG:     0x%02X (Expected: 0x0F, Reset: 0x08)\n", val);

        nRF24_read(EN_RXADDR, &val, 1);
        printf("nRF24 EN_RXADDR:  0x%02X (Expected: 0x02, Reset: 0x03)\n", val);

        nRF24_read(RX_PW_P1, &val, 1);
        printf("nRF24 RX_PW_P1:   0x%02X (Expected: 0x02, Reset: 0x00)\n", val);

        uint8_t rx_addr[5] = {0};
        nRF24_read(RX_ADDR_P1, rx_addr, 5);
        printf("nRF24 RX_ADDR_P1: 0x%02X%02X%02X%02X%02X (Expected: 0xAABBCCDDEE, Reset: 0xC2C2C2C2C2)\n", 
            rx_addr[4], rx_addr[3], rx_addr[2], rx_addr[1], rx_addr[0]);
    }
}

void nRF24_init(const uint8_t *address, uint8_t channel, bool tx_mode) {
    delay_ms(100); // Wait for nRF24 to power up
    gpio_config_mode(nRF24_CE, OUTPUT);
    gpio_config_mode(nRF24_CSN, OUTPUT);
    set_ce(0);
    set_csn(1);

    // Reset Status & Flush Buffers
    nRF24_cmd(FLUSH_TX); // Clears the TX buffer, can hold 3 packets (in this case a packet holds 2 bytes)
    nRF24_cmd(FLUSH_RX);
    nRF24_write(STATUS, (uint8_t[]){0x70}, 1); // Clear interrupts, Write 1 to clear
    
    // TX RX Common Configs
    nRF24_write(EN_AA,     (uint8_t[]){0x00}, 1); // Disable Auto-ACK
    nRF24_write(SETUP_AW,  (uint8_t[]){0x03}, 1); // 5-byte address width
    nRF24_write(SETUP_RETR,(uint8_t[]){0x00}, 1); // No retransmissions
    nRF24_write(RF_CH,     &channel,          1);
    nRF24_write(RF_SETUP,  (uint8_t[]){0x0E}, 1); // 2Mbps, 0dBm
    // Per TX RX Configs
    if (tx_mode) {
        nRF24_write(CONFIG,     (uint8_t[]){0x0E}, 1); // TX Mode, Power Up
        nRF24_write(TX_ADDR,    address,           5); // Set TX Target Address
    } else {
        nRF24_write(CONFIG,     (uint8_t[]){0x0F}, 1); // RX Mode, Power Up
        nRF24_write(EN_RXADDR,  (uint8_t[]){0x02}, 1); // Enable Pipe 1 Only
        nRF24_write(RX_PW_P1,   (uint8_t[]){0x02}, 1); // 2-byte payload for Pipe 1
        nRF24_write(RX_ADDR_P1, address,           5); // Set RX Pipe 1 address
        set_ce(1);
    }
    
    nRF24_print_registers(tx_mode);
}

bool nRF24_transmit(const uint8_t *data) {
    uint8_t status_reg = 0;

    nRF24_write(W_TX_PAYLOAD, data, 2); // Write payload to nRF24 module

    nRF24_read(FIFO_STATUS, &status_reg, 1);
    printf("NRF FIFO_STATUS 1: 0x%02X\n", status_reg);

    set_ce(1); // Flash CE to transmit package
    //delay_ms(1);
    set_ce(0); 

    nRF24_read(FIFO_STATUS, &status_reg, 1);
    printf("NRF FIFO_STATUS 2: 0x%02X\n", status_reg);
    
    nRF24_read(STATUS, &status_reg, 1);
    if(status_reg & STATUS_TX_DS) {
        nRF24_write(STATUS, (uint8_t[]){STATUS_TX_DS}, 1);
        nRF24_cmd(FLUSH_TX); // Clear TX FIFO, get rid of new data the MCU didn't have time to process
        return 1;
    }
    return 0;
}

bool nRF24_receive(uint8_t *data) {
    uint8_t status_reg = 0;
    nRF24_read(STATUS, &status_reg, 1);
    printf("STATUS: 0x%02X\n", status_reg);

    // Check if Data is Available
    if (status_reg & STATUS_RX_DR) {
        nRF24_read(R_RX_PAYLOAD, data, 2); // Read data
        nRF24_write(STATUS, (uint8_t[]){STATUS_RX_DR}, 1); // Clear flag
        nRF24_cmd(FLUSH_RX); // Clear RX FIFO, get rid of new data the MCU didn't have time to process
        return 1;
    }
    return 0;
}