#ifndef UART_APP_H
#define UART_APP_H

#include "bsp_system.h"

#define UART_RX_BUFFER_SIZE 64

typedef struct {
    float pitch;
    float roll;
    float yaw;
    float temperature;
    int16_t ax;
    int16_t ay;
    int16_t az;
    int16_t gx;
    int16_t gy;
    int16_t gz;
    int16_t version;
} WIT_Data_t;

extern WIT_Data_t wit_data;


void My_Uart_Init(void);
void Uart_Proc(void);
int my_printf(UART_HandleTypeDef *huart, const char *format, ...);



#endif
