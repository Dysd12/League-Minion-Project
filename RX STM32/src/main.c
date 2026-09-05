/**
 * Daniel Yeo
 * 2026/9/2
 */
#include <stm32l432xx.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "delay.h"
#include "gpio.h"
#include "motors.h"
#include "nrf.h"
#include "spi.h"
#include "timer.h"
#include "usart.h"


int main() {
    uint8_t address[5] = {0xEE, 0xDD, 0xCC, 0xBB, 0xAA}; // LSB written first
    uint8_t channel = 10;
    uint8_t rx_data[2] = {0};
    int motor_data[2] = {0};

    usart2_init(9600);
    delay_init();
    spi1_init();
	nRF24_init(address, channel, 0);
    tim1_init(1000); // 1 kHz PWM frequency
    motors_init();
    
    while(1) {
        if(nRF24_receive(rx_data)) {
            motor_data[0] =   rx_data[0] - 120;
            motor_data[1] = -(rx_data[1] - 120);
            printf("Received Data: (%d, %d)\n", motor_data[0], motor_data[1]);
            set_motor_pwm(motor_data[0], motor_data[1]);
        }
        delay_ms(100);
    }
    return 0;
}

int _write(int file, char *data, int len) {
    serial_write(USART2, data, len);
    return len;
}