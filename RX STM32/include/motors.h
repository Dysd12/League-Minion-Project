#ifndef MOTORS_H_
#define MOTORS_H_

#include "gpio.h"

// Nucleo pins connected to the Pololu motor driver
#define APHASE  D3
#define AENABLE D9
#define BPHASE  D5
#define BENABLE D10

void set_motor_pwm(int drive_velocity, int turn_velocity);

#endif