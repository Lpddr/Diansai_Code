#include "motor_app.h"
// 全局电机变量
MOTOR motor_left;
MOTOR motor_right;
// 双电机初始化函数
void Motor_Init(void) {
    // 初始化电机A (左电机 - 使用PWM通道0, AIN1/AIN2引脚)
    Motor_Config_Init(&motor_left,
                       PWM_MOTOR_INST,                    // TIMG6
                       GPIO_PWM_MOTOR_C0_IDX,             // DL_TIMER_CC_0_INDEX
                       AIN_PORT,                          // GPIOB (使用AIN_PORT宏)
                       AIN_AIN1_PIN,                      // DL_GPIO_PIN_20
                       AIN_PORT,                          // GPIOB 
                       AIN_AIN2_PIN,                      // DL_GPIO_PIN_24
                       0,                                 // 不反转
                       5);                               // 死区速度值
         
    // 初始化电机B (右电机 - 使用PWM通道1, BIN1/BIN2引脚)
    Motor_Config_Init(&motor_right,
                       PWM_MOTOR_INST,                    // TIMG6
                       GPIO_PWM_MOTOR_C1_IDX,             // DL_TIMER_CC_1_INDEX
                       BIN_PORT,                          // GPIOB
                       BIN_BIN1_PIN,                      // DL_GPIO_PIN_18
                       BIN_PORT,                          // GPIOB
                       BIN_BIN2_PIN,                      // DL_GPIO_PIN_19
                       0,                                 // 不反转
                       5);                               // 死区速度值
         
    // 启用STBY引脚 
    DL_GPIO_setPins(STBY_PORT, STBY_ebl_PIN);
    
    // 添加短暂延时让系统稳定
    DL_Common_delayCycles(1000);
}


void Motor_Task(void)
{
    Motor_Set_Speed(&motor_left, 3000);
    Motor_Set_Speed(&motor_right, 1000);
}