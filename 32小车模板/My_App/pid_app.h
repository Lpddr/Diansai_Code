#ifndef PID_APP_H
#define PID_APP_H

#include "bsp_system.h"
#define MAX_PID_MODE 4
// PID参数结构体
typedef struct
{
    float kp;          // 比例系数
    float ki;          // 积分系数
    float kd;          // 微分系数
    float out_min;     // 输出最小值
    float out_max;     // 输出最大值
} PidParams_t;
extern bool pid_running;
extern unsigned char pid_control_mode;

extern int basic_speed; // 基础速度
void PID_Init(void);
void PID_Task(void);
#endif

