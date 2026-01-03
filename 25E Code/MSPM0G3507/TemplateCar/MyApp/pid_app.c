#include "pid_app.h"
int basic_speed = 30; // 基础速度

bool pid_running = false; // PID 控制使能开关
unsigned char pid_control_mode = 0; // 0-角度环控制，1-循迹环控制
int output_left = 0, output_right = 0;
bool stby_flag=0;
/* PID 控制器实例 */
PID_T pid_speed_left;  // 左轮速度环
PID_T pid_speed_right; // 右轮速度环
PID_T pid_line;        // 循迹环
PID_T pid_angle;        // 角度环
/* PID 参数定义 */
PidParams_t pid_params_left = {
    .kp = 55.0f,       //55 
    .ki = 0.55,      //0.55
    .kd = 0.00f,      
    .out_min = -7999.0f,
    .out_max = 7999.0f,
};

PidParams_t pid_params_right = {
    .kp = 55.0f,        //55
    .ki = 0.5,      //0.5
    .kd = 0.00f,      
    .out_min = -7999.0f,
    .out_max = 7999.0f,
};

PidParams_t pid_params_line = {
    .kp = 8.5f,       //10.5 
    .ki = 0.0000f,      
    .kd = 8.5f,      //10.5
    .out_min = -100.0f,
    .out_max = 100.0f,
};
PidParams_t pid_params_angle = {
    .kp = 0.8f,        
    .ki = 0.00012f,      
    .kd = 10.00f,      
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
  

  pid_init(&pid_line,
           pid_params_line.kp, pid_params_line.ki, pid_params_line.kd,
           0.0f, pid_params_line.out_max);

  pid_init(&pid_angle,
           pid_params_angle.kp, pid_params_angle.ki, pid_params_angle.kd,
           0.0f, pid_params_angle.out_max);
  
  pid_set_target(&pid_speed_left, basic_speed);
  pid_set_target(&pid_speed_right, basic_speed);
  pid_set_target(&pid_line, 0);
  pid_set_target(&pid_angle, 0);

}

void Line_PID_control(void) // 循迹环控制
{
  int line_pid_output = 0;
  basic_speed=30;
  // 使用位置式 PID 计算利用循迹环计算输出
  line_pid_output = pid_calculate_positional(&pid_line, g_line_position_error);
  
  // 输出限幅
  line_pid_output = pid_constrain(line_pid_output, pid_params_line.out_min, pid_params_line.out_max);
  
  // 将差值作用在速度环的目标量上
  pid_set_target(&pid_speed_left, basic_speed + line_pid_output);
  pid_set_target(&pid_speed_right, basic_speed - line_pid_output);
}

void Angle_PID_control(void) // 角度环控制
{
  int angle_pid_output = 0;
    basic_speed=5;
  
  // 使用位置式 PID 计算利用角度环计算输出
  angle_pid_output = pid_calculate_positional(&pid_angle, wit_data.yaw);
  
  // 输出限幅
  angle_pid_output = pid_constrain(angle_pid_output, pid_params_angle.out_min, pid_params_angle.out_max);
  
  // 将差值作用在速度环的目标量上
  pid_set_target(&pid_speed_left, basic_speed - angle_pid_output);
  pid_set_target(&pid_speed_right, basic_speed + angle_pid_output);
}





void PID_Task(void)
{
    if(pid_running == false) return;
    if(counter_flag==0)
    Line_PID_control();
    output_left = pid_calculate_positional(&pid_speed_left, left_encoder.speed_cm_s);
    output_right = pid_calculate_positional(&pid_speed_right, right_encoder.speed_cm_s);
  
    // 输出限幅
    output_left = pid_constrain(output_left, pid_params_left.out_min, pid_params_left.out_max);
    output_right = pid_constrain(output_right, pid_params_right.out_min, pid_params_right.out_max);
    
    // 设置电机速度
        Motor_Set_Speed(&motor_right, output_right);
        Motor_Set_Speed(&motor_left, output_left); 
    if(!stby_flag)
    {
        DL_GPIO_setPins(STBY_PORT, STBY_ebl_PIN);
        stby_flag=1;
    }   
}

