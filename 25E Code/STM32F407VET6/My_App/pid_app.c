#include "pid_app.h"
bool Pid_Running;//0停止 1启动
bool cross_OK;//十字架瞄准标志位
bool laser_open;
uint8_t pid_mode;
PID_T X_pid,X_big_jin_pid,X_big_yuan_pid;
PID_T X_rate_pid;
float X_Output=0,Y_Output=0;
float X_Target_Rate=0;
uint16_t Timer_500ms;
static bool rate_loop_active;

#define GIMBAL_IMU_RATE_SIGN             1.0f
#define ENCODER_BODY_FF_GAIN             0.12f
#define ENCODER_BODY_FF_SIGN             1.0f
#define ENCODER_BODY_FF_LPF_ALPHA        0.70f
#define ENCODER_BODY_FF_LIMIT            30.0f
#define MOTION_TELEMETRY_TIMEOUT_MS      50U

typedef struct
{
    float gimbal_yaw_rate_dps;
    float encoder_feedforward_output;
    uint32_t update_tick_ms;
    bool valid;
} motion_feedback_t;

static motion_feedback_t motion_feedback;

static bool MotionFeedback_IsValid(void)
{
    return motion_feedback.valid &&
           (uint32_t)(HAL_GetTick() - motion_feedback.update_tick_ms) <=
               MOTION_TELEMETRY_TIMEOUT_MS;
}

void Pid_UpdateMotionFeedback(float gimbal_yaw_rate_dps,
                              float encoder_speed_diff_cm_s)
{
    float raw_feedforward = ENCODER_BODY_FF_SIGN *
                            ENCODER_BODY_FF_GAIN *
                            encoder_speed_diff_cm_s;

    if (raw_feedforward > ENCODER_BODY_FF_LIMIT)
    {
        raw_feedforward = ENCODER_BODY_FF_LIMIT;
    }
    else if (raw_feedforward < -ENCODER_BODY_FF_LIMIT)
    {
        raw_feedforward = -ENCODER_BODY_FF_LIMIT;
    }

    motion_feedback.gimbal_yaw_rate_dps =
        GIMBAL_IMU_RATE_SIGN * gimbal_yaw_rate_dps;

    if (motion_feedback.valid)
    {
        motion_feedback.encoder_feedforward_output =
            ENCODER_BODY_FF_LPF_ALPHA *
                motion_feedback.encoder_feedforward_output +
            (1.0f - ENCODER_BODY_FF_LPF_ALPHA) * raw_feedforward;
    }
    else
    {
        motion_feedback.encoder_feedforward_output = raw_feedforward;
    }

    motion_feedback.update_tick_ms = HAL_GetTick();
    motion_feedback.valid = true;
}

void Pid_MotionFeedback_HealthTask(void)
{
    if (motion_feedback.valid && !MotionFeedback_IsValid())
    {
        motion_feedback.valid = false;
        motion_feedback.encoder_feedforward_output = 0.0f;
    }
}
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

/*
 * 云台角速度内环初始参数。
 * 外环输出单位按期望角速度(deg/s)解释，内环反馈为云台IMU的gz。
 * 该组参数只作为安全起调值，必须在实车上按“先内环、后外环”的顺序整定。
 */
PID_para X_rate_para={
    .kp=1.0f,
    .ki=0.01f,
    .kd=0.02f,
    .target=0,
    .limit=300,
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

static float Cascade_Gimbal_Control(PID_T *position_pid)
{
    float rate_feedback;
    float motor_output;

    /* 位置外环：视觉像素偏差 -> 云台期望角速度。 */
    pid_set_target(position_pid, cross.x);
    X_Target_Rate =
        pid_calculate_positional(position_pid, g_circle.center_x);

    if (MotionFeedback_IsValid())
    {
        if (!rate_loop_active)
        {
            pid_reset(&X_rate_pid);
            rate_loop_active = true;
        }

        /* 角速度内环：期望角速度 - 云台IMU实测角速度。 */
        rate_feedback = motion_feedback.gimbal_yaw_rate_dps;
        pid_set_target(&X_rate_pid, X_Target_Rate);
        motor_output =
            pid_calculate_positional(&X_rate_pid, rate_feedback);

        /* 编码器差速估计车体转动趋势，作为独立前馈叠加至执行量。 */
        motor_output += motion_feedback.encoder_feedforward_output;
    }
    else
    {
        /* 遥测失联时退化为原视觉位置环，避免用无效角速度闭环。 */
        if (rate_loop_active)
        {
            pid_reset(&X_rate_pid);
            rate_loop_active = false;
        }
        motor_output = X_Target_Rate;
    }

    return pid_constrain(motor_output,
                         -X_rate_para.limit,
                         X_rate_para.limit);
}




void Pid_Init(void)
{
    pid_init(&X_pid,X_para.kp,X_para.ki,X_para.kd,X_para.target,X_para.limit);
    pid_init(&X_big_jin_pid,X_big_jin_para.kp,X_big_jin_para.ki,X_big_jin_para.kd,X_big_jin_para.target,X_big_jin_para.limit);
    pid_init(&X_big_yuan_pid,X_big_yuan_para.kp,X_big_yuan_para.ki,X_big_yuan_para.kd,X_big_yuan_para.target,X_big_yuan_para.limit);
    pid_init(&X_rate_pid,X_rate_para.kp,X_rate_para.ki,X_rate_para.kd,X_rate_para.target,X_rate_para.limit);
    rate_loop_active = false;
    motion_feedback.gimbal_yaw_rate_dps = 0.0f;
    motion_feedback.encoder_feedforward_output = 0.0f;
    motion_feedback.update_tick_ms = HAL_GetTick();
    motion_feedback.valid = false;
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
    X_Output = Cascade_Gimbal_Control(&X_pid);
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
    X_Output = Cascade_Gimbal_Control(&X_pid);
    Motor_Position_Control(&X_huart, X_Output);
}

/**
 * @brief PID模式0处理函数
 * @note  如果X轴差值小于5，立即启动激光；否则进行X轴PID控制
 */
static void Pid_Mode_2_Handler(void)
{
 
    // 执行X轴PID控制
    X_Output = Cascade_Gimbal_Control(&X_big_jin_pid);
    Motor_Position_Control(&X_huart, X_Output);
}
/**
 * @brief PID模式0处理函数
 * @note  如果X轴差值小于5，立即启动激光；否则进行X轴PID控制
 */
static void Pid_Mode_3_Handler(void)
{
 
    // 执行X轴PID控制
    X_Output = Cascade_Gimbal_Control(&X_big_yuan_pid);
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
        if (rate_loop_active) {
            pid_reset(&X_rate_pid);
            rate_loop_active = false;
        }
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


