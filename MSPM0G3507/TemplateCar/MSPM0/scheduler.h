#ifndef __SCHEDULER_H__
#define __SCHEDULER_H__

#include "Mydefine.h"

// 任务结构体
typedef struct {
  void (*task_func)(void);  // 任务函数指针
  uint32_t rate_ms;         // 执行周期（毫秒）
  uint32_t last_run;        // 上次执行时间（初始化为 0，每次运行时刷新）
} scheduler_task_t;

#define LEFT 0
#define RIGHT 1
#define TURN_SPEED 27

extern uint8_t point_count;
extern uint8_t system_mode;
extern uint8_t point_count;
extern bool counter_flag;
extern uint8_t car_point;
extern uint8_t car_turns;//设定行驶圈数
void Scheduler_Init(void);
void Scheduler_Run(void);
  
#endif
