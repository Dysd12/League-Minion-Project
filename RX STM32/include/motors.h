#ifndef MOTORS_H_
#define MOTORS_H_

#include "gpio.h"

// Nucleo pins connected to the Pololu motor driver
#define APHASE  D5
#define AENABLE D9
#define BPHASE  D6
#define BENABLE D10

void motors_init();
void set_motor_pwm(int drive_velocity, int turn_velocity);


#endif