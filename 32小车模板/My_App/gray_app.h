#ifndef GRAY_APP_H
#define GRAY_APP_H

#include "bsp_system.h"
// 灰度传感器引脚数量定义
#define GRAY_SENSOR_COUNT 7


void Gray_Task(void);
extern uint8_t Digital;
#endif
