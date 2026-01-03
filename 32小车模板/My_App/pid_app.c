#include "pid_app.h"

int basic_speed = 80; // 基础速度
bool pid_running = false; // PID 控制使能开关
unsigned char pid_control_mode = 0; // 0-角度环控制，1-循迹环控制
/* PID 控制器实例 */
PID_T pid_speed_left;  // 左轮速度环
PID_T pid_speed_right; // 右轮速度环


/* PID 参数定义 */
PidParams_t pid_params_left = {
    .kp = 15.0f,        
    .ki = 0.150f,      
    .kd = 0.00f,      
    .out_min = -999.0f,
    .out_max = 999.0f,
};

PidParams_t pid_params_right = {
    .kp = 15.0f,        
    .ki = 0.150f,      
    .kd = 0.00f,      
    .out_min = -999.0f,
    .out_max = 999.0f,
};


void PID_Init(void)
{
  pid_init(&pid_speed_left,
           pid_params_left.kp, pid_params_left.ki, pid_params_left.kd,
           0.0f, pid_params_left.out_max);
  
  pid_init(&pid_speed_right,
           pid_params_right.kp, pid_params_right.ki, pid_params_right.kd,
           0.0f, pid_params_right.out_max);
  
  pid_set_target(&pid_speed_left, basic_speed);
  pid_set_target(&pid_speed_right, basic_speed);
}


void PID_Task(void)
{
  if(pid_running == false) return;
  
    int output_left = 0, output_right = 0;
  
  
    // 使用位置式 PID 计算利用速度环计算输出
    output_left = pid_calculate_positional(&pid_speed_left, left_encoder.speed_cm_s);
    output_right = pid_calculate_positional(&pid_speed_right, right_encoder.speed_cm_s);
  
    // 输出限幅
    output_left = pid_constrain(output_left, pid_params_left.out_min, pid_params_left.out_max);
    output_right = pid_constrain(output_right, pid_params_right.out_min, pid_params_right.out_max);
    
    // 设置电机速度
    Motor_Set_Speed(&left_motor, output_left);
    Motor_Set_Speed(&right_motor, output_right);
}

