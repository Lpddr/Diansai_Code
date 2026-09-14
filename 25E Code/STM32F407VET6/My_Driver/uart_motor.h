#ifndef UART_MOTOR_H
#define UART_MOTOR_H

#include <stdint.h>

#define UART_RX_BUFFER_SIZE 64U

extern uint8_t uart4_read_buffer[UART_RX_BUFFER_SIZE];
extern volatile uint8_t uart4_flag;
extern volatile uint16_t uart4_size;

extern uint8_t uart6_read_buffer[UART_RX_BUFFER_SIZE];
extern volatile uint8_t uart6_flag;
extern volatile uint16_t uart6_size;

void My_UartInit(void);
void Parse_M0_Data(const char *data, uint16_t length);

#endif
