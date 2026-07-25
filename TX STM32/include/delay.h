#ifndef DELAY_H_
#define DELAY_H_

#include <stdint.h>

void delay_init();
void delay_ms(uint32_t time);

uint64_t millis();


#endif
