#ifndef __MOTOR_DRIVER_H
#define __MOTOR_DRIVER_H

#include "ti_msp_dl_config.h"
// TB6612 电机驱动结构体
typedef struct MOTOR_Config{
    GPTIMER_Regs *timer_inst;     // 定时器实例
    uint32_t pwm_channel;         // PWM通道 (DL_TIMER_CC_0, DL_TIMER_CC_1等)
    struct {
        GPIO_Regs *port;
        uint32_t pin;
    } in1, in2;                   // 对应TB6612的AIN或BIN引脚
    unsigned char reverse;        // 电机方向是否反转：0-正常，1-反转
} MOTOR_Config;

// 电机结构体
typedef struct MOTOR
{
    MOTOR_Config config;
    float speed;
    int dead_band_speed;          // 死区：当 -dead_band_speed < speed < dead_band_speed 时电机不会转动
} MOTOR;

extern MOTOR motor_left;
extern MOTOR motor_right;
// 电机初始化函数(绑定相关的功能引脚)
void Motor_Config_Init(MOTOR* motor, GPTIMER_Regs *timer_inst, uint32_t pwm_channel, 
                       GPIO_Regs *in1_port, uint32_t in1_pin, GPIO_Regs *in2_port, uint32_t in2_pin, 
                       unsigned char reverse, int dead_band_speed);

// 设置速度
void Motor_Set_Speed(MOTOR* motor, int speed);

// 电机停止  
void Motor_Stop(MOTOR* motor);          

// 电机刹车
void Motor_Brake(MOTOR* motor);         

#endif