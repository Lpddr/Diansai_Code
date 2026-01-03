#include "motor_app.h"
MOTOR left_motor;
MOTOR right_motor;

void Motor_Init(void)
{
    Motor_Config_Init(&left_motor, &htim1, TIM_CHANNEL_1, AIN1_GPIO_Port, AIN1_Pin, AIN2_GPIO_Port, AIN2_Pin, 1, 80);
    Motor_Config_Init(&right_motor, &htim1, TIM_CHANNEL_2, BIN1_GPIO_Port, BIN1_Pin, BIN2_GPIO_Port, BIN2_Pin, 1, 80);
    HAL_GPIO_WritePin(STBY_GPIO_Port,STBY_Pin,GPIO_PIN_SET);
    HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_1);
    Motor_Set_Speed(&left_motor,0);
    Motor_Set_Speed(&right_motor,0);
    
}

void Motor_Task(void)
{
    
}

