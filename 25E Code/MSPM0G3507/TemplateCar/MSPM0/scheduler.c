#include "scheduler.h"
uint8_t system_mode=1;//题目
uint16_t distance;
uint8_t point_count;
unsigned char output_ff_flag;
unsigned int intput_timer500ms;

unsigned char intput_ff_flag;
unsigned int output_timer500ms;

unsigned int led_timer500ms; // 每经过一个点，LED 点亮 500ms 后熄灭
bool led_state;
unsigned char circle_count = 0; // 第四题的圈数计数器
// 全局变量，用于存储任务数量
uint8_t task_num;

// 静态任务数组，每个任务包含任务函数、执行周期（毫秒）和上次运行时间（毫秒）
static scheduler_task_t scheduler_task[] =
{
  {Key_Task, 10, 0},
  {Oled_Task, 10, 0},
  {Uart_Task, 10, 0},
//  {Mpu6050_Task, 1, 0},
{Task_GraySensor,10,0},
  {Gray_Task, 10, 0},
  {Motor_Task, 10, 0},
//  {Encoder_Task, 10, 0},
};


/**
 * @brief 调度器初始化函数
 * 计算任务数组的元素个数，并将结果存储在 task_num 中
 */
void Scheduler_Init(void)
{
  // 计算任务数组的元素个数，并将结果存储在 task_num 中
  task_num = sizeof(scheduler_task) / sizeof(scheduler_task_t); // 数组大小 / 数组成员大小 = 数组元素个数
}

/**
 * @brief 调度器运行函数
 * 遍历任务数组，检查是否有任务需要执行。如果当前时间已经超过任务的执行周期，则执行该任务并更新上次运行时间
 */
void Scheduler_Run(void)
{
  // 遍历任务数组中的所有任务
  for (uint8_t i = 0; i < task_num; i++)
  {
    // 获取当前的系统时间（毫秒）
    uint32_t now_time = tick_ms;

    // 检查当前时间是否达到任务的执行时间
    if (now_time >= scheduler_task[i].rate_ms + scheduler_task[i].last_run)
    {
      // 更新任务的上次运行时间为当前时间
      scheduler_task[i].last_run = now_time;

      // 执行任务函数
      scheduler_task[i].task_func();
    }
  }
}
bool counter_flag;
uint16_t Counter_Timer=0;//刹车时间
uint8_t Count_Pid;
uint8_t car_point;//转弯次数
uint8_t car_turns=1;//设定行驶圈数
void Turn_Angle(bool dir)
{
    if(dir==0)
    {
        pid_set_target(&pid_speed_left,-TURN_SPEED);
        pid_set_target(&pid_speed_right, TURN_SPEED);
    }
    else 
    {
        pid_set_target(&pid_speed_left,TURN_SPEED);
        pid_set_target(&pid_speed_right,-TURN_SPEED);
    }
    counter_flag=1;
}
void Car_Update(void)
{
    car_point++;
    if((car_point-1)/4==car_turns)
    {
        pid_running=0;
        car_point=0;
        Motor_Brake(&motor_left);
        Motor_Brake(&motor_right);
    }
}
// 预计算的位计数查找表（适用于任何C编译器，包括TI CCS）
static const uint8_t bit_count_table[16] = {
    0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4
};
void TIMER_10ms_INST_IRQHandler(void)
{
    DL_Timer_clearInterruptStatus(TIMER_10ms_INST, DL_TIMER_INTERRUPT_ZERO_EVENT);


    if(counter_flag)
    {
        Counter_Timer++;
        if(Counter_Timer>=550)
        {
            pid_set_target(&pid_speed_left,0);
            pid_set_target(&pid_speed_right,0);
        }
        if(Counter_Timer>=750)
        {
            Counter_Timer=0;
            counter_flag=0;
        }
    }
    if(++Count_Pid>=5)
    {
        Count_Pid=0;
    if(counter_flag==0&&pid_running==1)
{
    
    // if(Digital==0x70)
    // {
    //     Car_Update();
    //     Turn_Angle(LEFT);
    // }
    // else if(Digital==0x07)
    // {
    //     Car_Update();
    //     Turn_Angle(RIGHT);
    // }

       // 提取左侧4个传感器的状态 (bit7-bit4)
    uint8_t left_sensors = (Digital >> 4) & 0x0F;
    // 提取右侧4个传感器的状态 (bit3-bit0)  
    uint8_t right_sensors = Digital & 0x0F;
    
    // 使用查表法统计1的个数
    uint8_t left_count = bit_count_table[left_sensors];
    uint8_t right_count = bit_count_table[right_sensors];
    
    // 左侧3个或以上检测到黑线，向左转
    if(left_count >= 4) {
        Car_Update();
        Turn_Angle(RIGHT);
    }
    // 右侧3个或以上检测到黑线，向右转
    else if(right_count >= 4) {
        Car_Update();
        Turn_Angle(LEFT);
        
    }
}

        Update_Encoder_Data_Filtered(&left_encoder);
        Update_Encoder_Data_Filtered(&right_encoder);
        PID_Task();
    }
}

