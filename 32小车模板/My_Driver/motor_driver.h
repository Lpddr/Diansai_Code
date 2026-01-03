#ifndef __MOTOR_DRIVER_H
#define __MOTOR_DRIVER_H

#include "bsp_system.h"

// TB6612 电机配置结构体
typedef struct MOTOR_Config{
    TIM_HandleTypeDef *htim; // 定时器
    uint32_t pwm_channel; // PWM 通道
    struct {
        GPIO_TypeDef *port;
        uint16_t pin;
    } in1, in2; // 对应 TB6612 中的 AIN 或 BIN
    unsigned char reverse; // 电机的方向是否反转。0-正常，1-反转
} MOTOR_Config;


// 电机结构体
typedef struct MOTOR
{
    MOTOR_Config config;
    int speed;
    int dead_band_speed; // 死区（当 -dead_band_speed < speed < dead_band_speed 时电机不会转动）
} MOTOR;

extern MOTOR left_motor;
extern MOTOR right_motor;

// 电机初始化函数(绑定相关的功能引脚)
void Motor_Config_Init(MOTOR* motor, TIM_HandleTypeDef *htim, uint32_t pwm_channel, 
                       GPIO_TypeDef *in1_port, uint16_t in1_pin, GPIO_TypeDef *in2_port, uint16_t in2_pin, 
                       unsigned char reverse, int dead_band_speed);

// 速度控制
void Motor_Set_Speed(MOTOR* motor, int speed);

// 电机停止  
void Motor_Stop(MOTOR* motor);          

// 电机刹车
void Motor_Brake(MOTOR* motor);         

#endif
