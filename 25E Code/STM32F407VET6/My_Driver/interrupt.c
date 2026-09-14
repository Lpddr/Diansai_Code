/**
 * @file interrupt.c
 * @brief TIM2初始化及转弯等待计时。
 */
#include "interrupt.h"

#include <stdbool.h>
#include <stdint.h>

#include "pid_app.h"
#include "tim.h"

#define TURN_DELAY_MS 750U

static volatile uint16_t turn_elapsed_ms;
static volatile bool turn_timing;

void My_TIM_Init(void)
{
    turn_elapsed_ms = 0U;
    turn_timing = false;

    __HAL_TIM_SET_AUTORELOAD(&htim2, 1999U);
    __HAL_TIM_SET_COUNTER(&htim2, 0U);
    HAL_TIM_Base_Start_IT(&htim2);
}

void Turn_Timer_Start(void)
{
    turn_elapsed_ms = 0U;
    turn_timing = true;
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM2 && turn_timing)
    {
        if (++turn_elapsed_ms >= TURN_DELAY_MS)
        {
            Pid_Running = true;
            pid_mode = 0U;
            turn_timing = false;
            turn_elapsed_ms = 0U;
        }
    }
}
