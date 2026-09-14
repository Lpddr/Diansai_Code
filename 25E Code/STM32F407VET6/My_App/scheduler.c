/**
 * @file scheduler.c
 * @brief 基于 SysTick 的裸机协作式周期调度。
 *
 * 为保持小型工程结构简单，任务表和任务入口统一放在本文件中。
 */
#include "scheduler.h"

#include "bsp_system.h"

typedef struct
{
    void (*task_func)(void);
    uint32_t period_ms;
    uint32_t last_run_ms;
} scheduler_task_t;

static uint16_t take_uart_frame(volatile uint8_t *ready_flag,
                                volatile uint16_t *received_size,
                                uint8_t *shared_buffer,
                                uint8_t *local_buffer,
                                uint16_t capacity)
{
    uint16_t size = 0U;
    uint32_t primask = __get_PRIMASK();

    __disable_irq();
    if (*ready_flag != 0U)
    {
        size = *received_size;
        if (size > capacity)
        {
            size = capacity;
        }
        memcpy(local_buffer, shared_buffer, size);
        *ready_flag = 0U;
    }
    if (primask == 0U)
    {
        __enable_irq();
    }

    return size;
}

static void Vision_Task(void)
{
    uint8_t frame[UART_RX_BUFFER_SIZE];
    uint16_t size = take_uart_frame(&uart6_flag,
                                    &uart6_size,
                                    uart6_read_buffer,
                                    frame,
                                    sizeof(frame));

    if (size > 0U)
    {
        parse_serial_data((const char *)frame, size);
    }
}

static void Chassis_Task(void)
{
    uint8_t frame[UART_RX_BUFFER_SIZE];
    uint16_t size = take_uart_frame(&uart4_flag,
                                    &uart4_size,
                                    uart4_read_buffer,
                                    frame,
                                    sizeof(frame));

    if (size > 0U)
    {
        Parse_M0_Data((const char *)frame, size);
    }
}

static void Gimbal_Control_Task(void)
{
    Pid_Task();
}

static scheduler_task_t schedule_task[] = {
    {Vision_Task,                    1U,   0U},
    {Chassis_Task,                  2U,   0U},
    {Gimbal_Control_Task,           5U,   0U},
    {Key_Task,                     10U,   0U},
    {Menu_Run,                     50U,   0U},
    {Pid_MotionFeedback_HealthTask, 100U, 0U},
};

void scheduler_init(void)
{
    uint32_t now_ms = HAL_GetTick();

    for (uint8_t i = 0U;
         i < (sizeof(schedule_task) / sizeof(schedule_task[0]));
         i++)
    {
        schedule_task[i].last_run_ms = now_ms;
    }
}

void scheduler_run(void)
{
    uint32_t now_ms = HAL_GetTick();

    for (uint8_t i = 0U;
         i < (sizeof(schedule_task) / sizeof(schedule_task[0]));
         i++)
    {
        uint32_t elapsed_ms = now_ms - schedule_task[i].last_run_ms;

        if (elapsed_ms >= schedule_task[i].period_ms)
        {
            schedule_task[i].last_run_ms += schedule_task[i].period_ms;

            if (elapsed_ms >= (schedule_task[i].period_ms * 2U))
            {
                schedule_task[i].last_run_ms = now_ms;
            }

            schedule_task[i].task_func();
        }
    }
}
