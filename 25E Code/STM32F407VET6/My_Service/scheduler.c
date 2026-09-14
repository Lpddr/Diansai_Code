#include "scheduler.h"

#include "bsp_system.h"

typedef struct
{
    void (*task_func)(void);
    uint32_t rate_ms;
    uint32_t last_run;
} scheduler_task_t;

static uint8_t task_num;

/* 服务层统一编排控制、通信及人机交互周期任务。 */
static scheduler_task_t schedule_task[] = {
    {Vision_Service_Task,          1,   0},
    {Chassis_Service_Task,         2,   0},
    {Gimbal_Control_Service_Task,  5,   0},
    {Key_Task,                     10,  0},
    {Menu_Run,                     50,  0},
    {MotionFusion_HealthTask,      100, 0},
};

void scheduler_init(void)
{
    task_num = sizeof(schedule_task) / sizeof(schedule_task[0]);
}

void scheduler_run(void)
{
    uint32_t now_time = HAL_GetTick();

    for (uint8_t i = 0; i < task_num; i++)
    {
        uint32_t elapsed = now_time - schedule_task[i].last_run;

        if (elapsed >= schedule_task[i].rate_ms)
        {
            /* 按理论释放时刻推进，减少任务执行时间带来的周期漂移。 */
            schedule_task[i].last_run += schedule_task[i].rate_ms;

            /* 严重超期时跳过积压周期，避免连续补跑阻塞主循环。 */
            if (elapsed >= (schedule_task[i].rate_ms * 2U))
            {
                schedule_task[i].last_run = now_time;
            }

            schedule_task[i].task_func();
        }
    }
}
