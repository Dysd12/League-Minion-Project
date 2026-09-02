#ifndef USART_H_
#define USART_H_

#include <stm32l432xx.h>
#include <stdint.h>

void usart2_init(unsigned int baud);
void usart_transmit(USART_TypeDef *USARTx, const uint8_t *data, unsigned int size);
void usart_receive(USART_TypeDef *USARTx, uint8_t *data, unsigned int size);


#endif