#include <stm32l432xx.h>
#include "motors.h"
#include "timer.h"


void motors_init() {
    // Configure motor control pins as outputs
    gpio_config_output(APHASE, 0, 0); // Motor A phase pin
    gpio_config_alternate_function(AENABLE, 1); // Motor A enable pin
    gpio_config_output(BPHASE, 0, 0); // Motor B phase pin
    gpio_config_alternate_function(BENABLE, 1); // Motor B enable pin
}

void set_motor_pwm(int drive_velocity, int turn_velocity) {
    // Calculate motor speeds based on drive and turn speeds
    int left_motor_speed = drive_velocity + turn_velocity;
    int right_motor_speed = drive_velocity - turn_velocity;

    // Reducing small values to zero to prevent jitter
    if (left_motor_speed < 5 && left_motor_speed > -5) {
        left_motor_speed = 0;
    }
    if (right_motor_speed < 5 && right_motor_speed > -5) {
        right_motor_speed = 0;
    }
    
    // Scaling motor speeds to a PWM percentage (0-100)
    int left_motor_pwm = (left_motor_speed  * 100) / 240;
    int right_motor_pwm = (right_motor_speed * 100) / 240;

    // Left Motor (B)
    if (left_motor_speed > 0) {
        gpio_write(BPHASE, 1);
        tim1_config_pwm(BENABLE, left_motor_pwm);    
    } else {
        gpio_write(BPHASE, 0);
        tim1_config_pwm(BENABLE, -left_motor_pwm);
    }

    // Right Motor (A)
    if (right_motor_speed > 0) {
        gpio_write(APHASE, 1);
        tim1_config_pwm(AENABLE, right_motor_pwm);
    } else {
        gpio_write(APHASE, 0);
        tim1_config_pwm(AENABLE, -right_motor_pwm);
    }
}
