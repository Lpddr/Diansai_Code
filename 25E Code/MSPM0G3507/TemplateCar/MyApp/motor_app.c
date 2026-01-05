#include "motor_app.h"
#include "encoder_driver.h" // [ADD] 引入编码器驱动，用于获取车速
#include "uart_app.h"       // [ADD] 引入串口驱动，用于发送数据
// 全局电机变量
MOTOR motor_left;
MOTOR motor_right;
// 双电机初始化函数
void Motor_Init(void)
{
    // 初始化电机A (左电机 - 使用PWM通道0, AIN1/AIN2引脚)
    Motor_Config_Init(&motor_left,
                      PWM_MOTOR_INST,        // TIMG6
                      GPIO_PWM_MOTOR_C0_IDX, // DL_TIMER_CC_0_INDEX
                      AIN_PORT,              // GPIOB (使用AIN_PORT宏)
                      AIN_AIN1_PIN,          // DL_GPIO_PIN_20
                      AIN_PORT,              // GPIOB
                      AIN_AIN2_PIN,          // DL_GPIO_PIN_24
                      1,                     // 不反转
                      5);                    // 死区速度值

    // 初始化电机B (右电机 - 使用PWM通道1, BIN1/BIN2引脚)
    Motor_Config_Init(&motor_right,
                      PWM_MOTOR_INST,        // TIMG6
                      GPIO_PWM_MOTOR_C1_IDX, // DL_TIMER_CC_1_INDEX
                      BIN_PORT,              // GPIOB
                      BIN_BIN1_PIN,          // DL_GPIO_PIN_18
                      BIN_PORT,              // GPIOB
                      BIN_BIN2_PIN,          // DL_GPIO_PIN_19
                      1,                     // 不反转
                      5);                    // 死区速度值

    // 启用STBY引脚
    DL_GPIO_setPins(STBY_PORT, STBY_ebl_PIN);

    // 添加短暂延时让系统稳定
    DL_Common_delayCycles(1000);
}

// ================= [前馈数据发送函数] =================
// 轮距: 两个轮子中心之间的距离 (单位: cm)  调这个参数即可
#define WHEEL_TRACK_CM 16.0f

void Send_Chassis_Gyro_To_Gimbal(void)
{
    // 1. 获取编码器测得的真实线速度 (cm/s)
    // 变量名来自 encoder_driver.h
    float v_L = left_encoder.speed_cm_s;
    float v_R = right_encoder.speed_cm_s;

    // 2. 核心公式：差速模型解算底盘角速度
    // 假设：右轮比左轮快 = 车身左转 (正方向)
    // 结果单位：弧度/s (rad/s)
    float w_rad_s = (v_R - v_L) / WHEEL_TRACK_CM;

    // 3. 换算成 度/s (deg/s)
    // 云台 PID 通常以 度 为单位，所以统一成 度/s
    // 1 rad = 57.296 度
    float w_deg_s = w_rad_s * 57.296f;

    // 4. 组装数据包 (协议: AA + 4字节float + BB)
    uint8_t tx_buf[6];
    tx_buf[0] = 0xAA; // 包头

    // 将 float 拆成 4个字节
    // 使用指针强转的方式直接读取 float 的内存
    uint8_t *p = (uint8_t *)&w_deg_s;
    tx_buf[1] = p[0];
    tx_buf[2] = p[1];
    tx_buf[3] = p[2];
    tx_buf[4] = p[3];

    tx_buf[5] = 0xBB; // 包尾

    // 5. 调用串口发送
    UART_Transmit(UART_CH340_INST, tx_buf, 6);
}

void Motor_Task(void)
{
    // Motor_Set_Speed(&motor_left, 8000);
    // Motor_Set_Speed(&motor_right,8000);
    // Motor_Brake(&motor_right);
    // Motor_Brake(&motor_left);

    // [ADD] 新增：计算并发送底盘角速度给云台
    // 利用这个被定时器调用的“空闲任务”进行前馈数据发送
    Send_Chassis_Gyro_To_Gimbal();
}