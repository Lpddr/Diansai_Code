#ifndef PID_APP_H
#define PID_APP_H

#include "bsp_system.h"

#define X_shaft 0
#define X_huart huart2
/* UART4保留给MSPM0底盘遥测链路。 */

#define CROSS_X 153
#define CROSS_Y 83

typedef struct
{
    float kp;					/* 比例 */
    float ki;					/* 积分 */
    float kd;					/* 微分 */
    float target;				/* 目标值 */
    float limit;                /* PID(out)输出限幅值 */
}PID_para;


void Pid_Init(void);
void Pid_Task(void);
extern float X_Output;
extern float Y_Output;
extern float X_Target_Rate;
extern bool Pid_Running;//0停止 1启动
extern uint8_t pid_mode;
#endif
