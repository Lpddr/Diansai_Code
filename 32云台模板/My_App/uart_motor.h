#ifndef UART_MOTOR_H
#define UART_MOTOR_H

#define UART_RX_BUFFER_SIZE 64

#include "bsp_system.h"


int my_printf(UART_HandleTypeDef *huart, const char *format, ...);
void My_UartInit(void);
void Uart_Proc(void);

void Stepmotor_Task(void);

#endif

