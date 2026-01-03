#include "pid_app.h"
bool Pid_Running;//0停止 1启动
bool cross_OK;//十字架瞄准标志位
bool laser_open;
uint8_t pid_mode;
PID_T X_pid,X_big_jin_pid,X_big_yuan_pid;
float X_Output=0,Y_Output=0;
uint16_t Timer_500ms;
PID_para X_para={
    .kp=0.5,//0.12//0.18//0.2//0.3//0.4 //0.8(超调1)//0.5      //0.4
    .ki=0,
    .kd=1000,//10//80//100//150//200//400(超调1)//250     //250
    .target=0,
    .limit=10,
};

PID_para X_big_jin_para={
    .kp=0.45,//0.5
    .ki=0,
    .kd=1500,//1500
    .target=0,
    .limit=100,
};

PID_para X_big_yuan_para={
    .kp=0.5,
    .ki=0,
    .kd=20000,
    .target=0,
    .limit=200,
};


///**
// * @brief    电机角度位置控制
// * @param    huart: 串口句柄
// * @param    angle: 目标角度(度)，正值为顺时针，负值为逆时针
// * @param    raF: 相位/绝对标志，false为相对运动，true为绝对值运动
// * @note     步进电机参数：1.8°精度，16细分，每脉冲 = 0.1125°
// */
//void Motor_Position_Control(UART_HandleTypeDef* huart, float angle, bool raF) 
//{    
//    // 计算脉冲数：1.8°精度，16细分，每脉冲角度 = 1.8/16 = 0.1125°
//    uint32_t pulses = (uint32_t)(fabsf(angle) / 0.1125f);
//    
//    if(angle < 0)
//    {
//        // 负角度，逆时针方向(CCW)
//        Emm_V5_Pos_Control(huart, 0x01, 1, 10, 0, pulses, raF, 0);
//    }
//    else
//    {
//        // 正角度或零角度，顺时针方向(CW)
//        Emm_V5_Pos_Control(huart, 0x01, 0, 10, 0, pulses, raF, 0);
//    }
//}


/*设置输出*/
void Motor_Position_Control(UART_HandleTypeDef* huart,float output)
{
   if(output < 0)
   {
       output = -output;  // 取绝对值
       Emm_V5_Vel_Control(huart, 0x01, 1,(uint16_t)output, 0, 0);
   }
   else
   {
       Emm_V5_Vel_Control(huart, 0x01, 0,(uint16_t)output, 0, 0);
   }
}




void Pid_Init(void)
{
    pid_init(&X_pid,X_para.kp,X_para.ki,X_para.kd,X_para.target,X_para.limit);
    pid_init(&X_big_jin_pid,X_big_jin_para.kp,X_big_jin_para.ki,X_big_jin_para.kd,X_big_jin_para.target,X_big_jin_para.limit);
    pid_init(&X_big_yuan_pid,X_big_yuan_para.kp,X_big_yuan_para.ki,X_big_yuan_para.kd,X_big_yuan_para.target,X_big_yuan_para.limit);
}


/**
 * @brief PID模式0处理函数
 * @note  如果X轴差值小于5，立即启动激光；否则进行X轴PID控制
 */
static void Pid_Mode_0_Handler(void)
{
    
    // 圆心坐标有效，进行PID控制
    // 检查X轴是否接近目标位置
    if (ABS(cross.x - g_circle.center_x) <= 4) {
        // 接近目标，开始计时
        Timer_500ms++;
        
        // 如果持续时间超过500ms（50 * 10ms），启动激光
        if (Timer_500ms >= 80) {
            if (laser_open == 0) {
                HAL_GPIO_WritePin(LASER_GPIO_Port, LASER_Pin, GPIO_PIN_SET);
                laser_open = 1;
            }
            Timer_500ms = 0;  // 重置计时器，避免重复触发
        }
    } else {
        // 远离目标，重置计时器
        Timer_500ms = 0;
    }
    
    // 执行X轴PID控制
    pid_set_target(&X_pid, cross.x);
    X_Output = pid_calculate_positional(&X_pid, g_circle.center_x);
    Motor_Position_Control(&X_huart, X_Output);
}

/**
 * @brief PID模式1处理函数
 * @note  如果圆心坐标为0则电机旋转；否则PID控制，差值小于5超过500ms后启动激光
 */
static void Pid_Mode_1_Handler(void)
{
    // 检查圆心坐标是否有效
    if (g_circle.center_x == 0 || g_circle.center_y == 0) {
        // 圆心坐标无效，给电机一个固定的旋转速度
        Emm_V5_Vel_Control(&X_huart, 0x01, 0, 30, 0, 0);
        Timer_500ms = 0;  // 重置计时器
        return;
    }
    
    // 圆心坐标有效，进行PID控制
    // 检查X轴是否接近目标位置
    if (ABS(cross.x - g_circle.center_x) <= 4) {
        // 接近目标，开始计时
        Timer_500ms++;
        
        // 如果持续时间超过500ms（50 * 10ms），启动激光
        if (Timer_500ms >= 150) {
            if (laser_open == 0) {
                HAL_GPIO_WritePin(LASER_GPIO_Port, LASER_Pin, GPIO_PIN_SET);
                laser_open = 1;
            }
            Timer_500ms = 0;  // 重置计时器，避免重复触发
        }
    } else {
        // 远离目标，重置计时器
        Timer_500ms = 0;
    }
    
    // 执行X轴PID控制
    pid_set_target(&X_pid, cross.x);
    X_Output = pid_calculate_positional(&X_pid, g_circle.center_x);
    Motor_Position_Control(&X_huart, X_Output);
}

/**
 * @brief PID模式0处理函数
 * @note  如果X轴差值小于5，立即启动激光；否则进行X轴PID控制
 */
static void Pid_Mode_2_Handler(void)
{
 
    // 执行X轴PID控制
    pid_set_target(&X_big_jin_pid, cross.x);
    X_Output = pid_calculate_positional(&X_big_jin_pid, g_circle.center_x);
    Motor_Position_Control(&X_huart, X_Output);
}
/**
 * @brief PID模式0处理函数
 * @note  如果X轴差值小于5，立即启动激光；否则进行X轴PID控制
 */
static void Pid_Mode_3_Handler(void)
{
 
    // 执行X轴PID控制
    pid_set_target(&X_big_yuan_pid, cross.x);
    X_Output = pid_calculate_positional(&X_big_yuan_pid, g_circle.center_x);
    Motor_Position_Control(&X_huart, X_Output);
}

/**
 * @brief PID控制任务
 * @note  建议移出中断，在主循环中执行
 */
void Pid_Task(void)
{
    // 如果PID未启动，直接返回
    if (Pid_Running == 0) {
        return;
    }
    
    switch (pid_mode) {
        case 0:  // 模式0：基础PID控制
            Pid_Mode_0_Handler();
            break;
        case 1:  // 模式1：带延时激光控制的PID
            Pid_Mode_1_Handler();
            break;
        case 2:
            Pid_Mode_2_Handler();
        break;
        case 3:
            Pid_Mode_3_Handler();
        break;
        default:
            // 未知模式，不执行任何操作
            break;
    }
}


