#include "interrupt.h"

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
        
    }
}

