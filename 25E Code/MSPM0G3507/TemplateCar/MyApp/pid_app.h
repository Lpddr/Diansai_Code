#ifndef PID_APP_H
#define PID_APP_H

#include "Mydefine.h"
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
void PID_Init(void);
void PID_Task(void);
#endif