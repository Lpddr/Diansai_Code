#include "interrupt.h"
unsigned char measure_timer10ms;
// TIM2 中断服务函数（1ms 中断）
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(htim->Instance == htim2.Instance) 
    {
        /* 10ms 测量任务*/
        if(++measure_timer10ms >= 10) 
        {
            measure_timer10ms = 0;
            Encoder_Task();
            PID_Task();
        }
        
    }
}

