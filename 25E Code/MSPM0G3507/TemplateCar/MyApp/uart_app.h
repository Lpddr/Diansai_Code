#ifndef UART_APP_H
#define UART_APP_H

#include "Mydefine.h"

#define UART_MAX_BUFFER 32

extern uint8_t uart_dma_buffer[UART_MAX_BUFFER];
extern uint8_t uart_buffer[UART_MAX_BUFFER];
extern uint8_t uart_flag;

int my_printf(UART_Regs *huart, const char *format, ...);
void Uart_CH340_Init(void);
void Uart_Task(void);
#endif