#ifndef BSP_SYSTEM_H
#define BSP_SYSTEM_H
/*系统头文件*/
#include "main.h"

/*C语言头文件*/
#include "stdio.h"
#include "string.h"
#include "stdarg.h"
#include <stdint.h>
#include <stdbool.h>

/*逻辑头文件*/
#include "scheduler.h"
#include "interrupt.h"
#include "uart_app.h"
#include "motor_app.h"
#include "encoder_app.h"
#include "gray_app.h"
#include "oled_app.h"
#include "key_app.h"
#include "pid_app.h"


/*底层文件*/ 
#include "motor_driver.h"
#include "encoder_driver.h"


/*组件头文件*/
#include "oled.h"
#include "pid.h"
#include "ebtn.h"
#include "w25qxx.h"
#include "wit.h"

extern UART_HandleTypeDef huart2;
extern DMA_HandleTypeDef hdma_usart2_rx;

extern UART_HandleTypeDef huart4;
extern DMA_HandleTypeDef hdma_uart4_rx;

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;
extern I2C_HandleTypeDef hi2c2;

extern SPI_HandleTypeDef hspi1;

#endif
