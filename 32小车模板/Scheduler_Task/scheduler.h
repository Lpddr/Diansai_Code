#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "bsp_system.h"
// 任务结构体
typedef struct {
  void (*task_func)(void);  // 任务函数指针
  uint32_t rate_ms;         // 执行周期（毫秒）
  uint32_t last_run;           // 上次执行时间（初始化为 0，每次运行时刷新）
} scheduler_task_t;

void Scheduler_Init(void);
void Scheduler_Run(void);
#endif
