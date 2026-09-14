#ifndef BSP_SYSTEM_H
#define BSP_SYSTEM_H

/*
 * 历史遗留的工程聚合头文件，用于小型裸机工程快速共享公共声明。
 * 它不是严格的分层接口；新代码应优先直接包含实际依赖的头文件，避免
 * 应用、服务和驱动之间形成不必要的循环包含。
 */

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

/*组件头文件*/
#include "oled.h"
#include "pid.h"
#include "w25qxx.h"

/*服务层头文件*/
#include "task_service.h"
#include "motion_fusion_service.h"


extern UART_HandleTypeDef huart6;
extern UART_HandleTypeDef huart4;
extern UART_HandleTypeDef huart2;
extern DMA_HandleTypeDef hdma_usart6_rx;
extern DMA_HandleTypeDef hdma_uart4_rx;
extern DMA_HandleTypeDef hdma_usart2_rx;
extern TIM_HandleTypeDef htim2;

#endif
