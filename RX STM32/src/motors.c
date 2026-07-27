#include <stm32l432xx.h>
#include "motors.h"

#define MAX_PWM 1023

void set_motor_pwm(int drive_velocity, int turn_velocity) {
    // Calculate motor speeds based on drive and turn speeds
    int left_motor_speed  = drive_velocity + turn_velocity;
    int right_motor_speed = drive_velocity - turn_velocity;

    // Reducing small values to zero to prevent jitter
    if (left_motor_speed < 5 && left_motor_speed > -5) {
        left_motor_speed = 0;
    }
    if (right_motor_speed < 5 && right_motor_speed > -5) {
        right_motor_speed = 0;
    }
    
    // Scaling motor speeds to a PWM percentage (0-100)
    int left_motor_pwm  = (left_motor_speed * 100) / 240;
    int right_motor_pwm = (right_motor_speed * 100) / 240;

    // Left Motor (B)
    if (left_motor_speed > 0) {
        gpio_write(BPHASE, 1);
        TIM1_config_pwm(BENABLE, (unsigned int)left_motor_pwm);    
    } else {
        gpio_write(BPHASE, 0);
        TIM1_config_pwm(BENABLE, (unsigned int)(-left_motor_pwm));
    }

    // Right Motor (A)
    if (right_motor_speed > 0) {
        gpio_write(APHASE, 1);
        TIM1_config_pwm(AENABLE, (unsigned int)right_motor_pwm);
    } else {
        gpio_write(APHASE, 0);
        TIM1_config_pwm(AENABLE, (unsigned int)(-right_motor_pwm));
    }
}
