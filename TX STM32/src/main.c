/**
 * Daniel Yeo
 * 2026/9/1
 */
#include <stm32l432xx.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "adc.h"
#include "delay.h"
#include "gpio.h"
#include "nrf.h"
#include "spi.h"
#include "usart.h"

/* Joystick Pins */
#define VRX D6
#define VRY D3

int main() {
    uint8_t tx_address[5] = {0xEE, 0xDD, 0xCC, 0xBB, 0xAA}; // LSB written first
    uint8_t channel = 10;
    uint8_t tx_data[2];

    usart2_init(9600);
    delay_init();
    spi1_init();
	nRF24_init(tx_address, channel, 1);
    adc_init();

    while(1) {
        adc_config_single(VRX);
        tx_data[0] = adc_read_single();
        adc_config_single(VRY);
        tx_data[1] = adc_read_single();
        if(nRF24_transmit(tx_data)) {
            printf("Transmitted Data: (%d, %d)\n", tx_data[0], tx_data[1]);
        }
        delay_ms(100);
    }
    return 0;
}

/* Testing*/
int main() {
    GPIO_write(D10, 0);
    uint32_t time = 100*300/7; // Wait for HW to set TXE to 0
    while(--time);
    GPIO_write(D10, 1);
    
    // testing if the delay is needed so tx fifo doesnt fill up.
    // or if even just two bytes will break the DR to Tx FIFO thing.
    spi1_init();
    uint8_t tx_data[4] = {0xAB, 0xCD, 0xEF, 0x01};
    spi_transmit(SPI1, tx_data, 2);
    
    return 0;
}


int _write(int file, char *data, int len) {
    usart_transmit(USART2, data, len);
    return len;
}