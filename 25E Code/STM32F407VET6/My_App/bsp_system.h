#ifndef BSP_SYSTEM_H
#define BSP_SYSTEM_H

/*系统头文件*/
#include "main.h"

/*C语言头文件*/
#include "stdio.h"
#include "string.h"
#include "stdarg.h"
#include "math.h"
#include "stdbool.h"
/*逻辑头文件*/
#include "scheduler.h"
#include "interrupt.h"
#include "uart_motor.h"
#include "key_app.h"
#include "oled_app.h"
#include "pid_app.h"
#include "parse_app.h"
#include "flash_app.h"
/*底层文件*/
#include "Emm_V5.h"
#include "imu_wit.h"
/*组件头文件*/
#include "oled.h"
#include "pid.h"
#include "w25qxx.h"

extern UART_HandleTypeDef huart6;
extern UART_HandleTypeDef huart4;
extern UART_HandleTypeDef huart2;
extern DMA_HandleTypeDef hdma_usart6_rx;
extern DMA_HandleTypeDef hdma_uart4_rx;
extern DMA_HandleTypeDef hdma_usart2_rx;
extern TIM_HandleTypeDef htim2;

#endif
