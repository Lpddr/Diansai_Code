/**
 * @file interrupt.c
 * @brief 定时器中断适配。
 *
 * 该文件逻辑上属于BSP/系统底层：只维护必要的时基和状态，不在中断中执行
 * UART协议解析、PID计算或OLED刷新等耗时业务。
 */
#include "interrupt.h"
uint16_t Timer_Turn;
void My_TIM_Init(void)
{
    HAL_TIM_Base_Start_IT(&htim2);
    __HAL_TIM_SET_AUTORELOAD(&htim2, 1999);
    __HAL_TIM_SET_COUNTER(&htim2,0);
}



// TIM2 中断服务函数（1ms 中断）
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(htim->Instance == htim2.Instance) 
    {
        if(Turn_Flag)
            {
                if(++Timer_Turn>=TIME_TURN)
                {
                    Pid_Running=1;
                    pid_mode=0;
                    Turn_Flag=0;
                    Timer_Turn=0;
                }
            }
    }
}

