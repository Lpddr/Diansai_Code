#ifndef UART_MOTOR_H
#define UART_MOTOR_H

#define UART_RX_BUFFER_SIZE 64

#define TURN_ANGLE 60
#include "bsp_system.h"

extern uint8_t uart6_read_buffer[UART_RX_BUFFER_SIZE];
extern uint8_t uart6_flag;
extern uint8_t uart6_size;
extern bool Turn_Flag;
extern uint8_t car_point;


extern uint8_t uart4_read_buffer[UART_RX_BUFFER_SIZE];
extern uint8_t uart4_flag;
int my_printf(UART_HandleTypeDef *huart, const char *format, ...);
void My_UartInit(void);
void Uart_Proc(void);
void Parse_M0_Data(const char* data);

#endif

