#include <stm32l432xx.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "delay.h"
#include "gpio.h"
#include "nrf.h"
#include "spi.h"
#include "usart.h"


int main() {
    uint8_t ADDRESS[5] = {0xEE, 0xDD, 0xCC, 0xBB, 0xAA}; // LSB written first
    uint8_t CHANNEL = 10;
    uint8_t rx_data[2] = {0};

    host_serial_init();
    delay_init();
    spi_init();
	nRF24_init(ADDRESS, CHANNEL, 0);

    while(1) {
        if(nRF24_receive(rx_data)) {
            printf("Received Data: (%d, %d)\n", (rx_data[0] - 120), -(rx_data[1] - 120));
        }
        delay_ms(100);
    }
    return 0;
}

int _write(int file, char *data, int len) {
    serial_write(USART2, data, len);
    return len;
}