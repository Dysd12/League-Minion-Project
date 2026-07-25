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

    host_serial_init();
    delay_init();
    spi_init();
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

int _write(int file, char *data, int len) {
    serial_write(USART2, data, len);
    return len;
}