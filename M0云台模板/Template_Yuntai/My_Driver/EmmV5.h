#ifndef EMMV5_H
#define EMMV5_H
#include "bsp_system.h"

// 步进电机相关常数定义
#define TIMER_FREQ_HZ           40000000    // 40MHz定时器频率
#define STEP_ANGLE_DEG          1.8f        // 基本步距角1.8°
#define MICROSTEPS              16          // 细分数
#define DEGREES_PER_REVOLUTION  360.0f      // 每圈360°

// 计算每圈所需脉冲数
#define STEPS_PER_REV           (DEGREES_PER_REVOLUTION / STEP_ANGLE_DEG)  // 200步/圈
#define PULSES_PER_REV          (STEPS_PER_REV * MICROSTEPS)               // 3200脉冲/圈

// 每个脉冲的角度
#define DEGREES_PER_PULSE       (STEP_ANGLE_DEG / MICROSTEPS)              // 0.1125°/脉冲

// 步进电机方向枚举
typedef enum {
    STEPPER_DIR_CW = 0,     // 顺时针
    STEPPER_DIR_CCW = 1     // 逆时针
} STEPPER_DIRECTION;

// 步进电机配置结构体
typedef struct {
    GPTIMER_Regs *timer_inst;           // 定时器实例
    GPIO_Regs *direction_port;          // 方向引脚端口  
    uint32_t direction_pin;             // 方向引脚
    GPIO_Regs *pulse_port;              // 脉冲引脚端口
    uint32_t pulse_pin;                 // 脉冲引脚
    bool reverse;                       // 方向反转标志
} STEPPER_Config;

// 步进电机控制结构体
typedef struct {
    float current_rpm;                  // 当前转速
    uint32_t current_period;            // 当前定时器周期值
    STEPPER_DIRECTION direction;        // 运行方向
} STEPPER_Control;

// 步进电机结构体
typedef struct {
    STEPPER_Config config;              // 配置参数
    STEPPER_Control control;            // 控制参数
    bool Motor_running;//0停止，1运行
} STEPPER_MOTOR;

// 全局电机实例声明
extern STEPPER_MOTOR MotorX;
extern STEPPER_MOTOR MotorY;

// 函数声明

/**
 * @brief 步进电机配置初始化
 * @param motor: 电机结构体指针
 * @param timer_inst: 定时器实例
 * @param direction_port: 方向引脚端口
 * @param direction_pin: 方向引脚
 * @param pulse_port: 脉冲引脚端口
 * @param pulse_pin: 脉冲引脚
 * @param reverse: 方向反转标志
 */
void Stepper_Config_Init(STEPPER_MOTOR* motor, 
                        GPTIMER_Regs *timer_inst,
                        GPIO_Regs *direction_port, uint32_t direction_pin,
                        GPIO_Regs *pulse_port, uint32_t pulse_pin,
                        bool reverse);

/**
 * @brief 设置步进电机转速并启动
 * @param motor: 电机结构体指针
 * @param rpm: 目标转速(转/分钟)，正值为顺时针，负值为逆时针
 * @return true: 设置成功, false: 设置失败
 */
bool Stepper_Set_Speed(STEPPER_MOTOR* motor, float rpm);

/**
 * @brief 停止步进电机
 * @param motor: 电机结构体指针
 */
void Stepper_Stop(STEPPER_MOTOR* motor);


/**
 * @brief 获取当前转速
 * @param motor: 电机结构体指针
 * @return 当前转速(RPM)
 */
float Stepper_Get_Current_RPM(STEPPER_MOTOR* motor);

/**
 * @brief 步进电机系统初始化
 */
void Stepper_System_Init(void);
#endif