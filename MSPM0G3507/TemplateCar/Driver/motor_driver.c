#include "motor_driver.h"

#define Motor_ABS(x) ((x) >= 0 ? (x) : -(x))

// 电机初始化函数
void Motor_Config_Init(MOTOR* motor, GPTIMER_Regs *timer_inst, uint32_t pwm_channel,
                       GPIO_Regs *in1_port, uint32_t in1_pin, GPIO_Regs *in2_port, uint32_t in2_pin,
                       unsigned char reverse, int dead_band_speed) {
    motor->config.timer_inst = timer_inst;
    motor->config.pwm_channel = pwm_channel;
    motor->config.in1.port = in1_port;
    motor->config.in1.pin = in1_pin;
    motor->config.in2.port = in2_port;
    motor->config.in2.pin = in2_pin;
    motor->config.reverse = reverse;
    
    motor->dead_band_speed = dead_band_speed;
    motor->speed = 0;
    
    // 设置GPIO初始状态
    DL_GPIO_setPins(motor->config.in1.port, motor->config.in1.pin);
    DL_GPIO_clearPins(motor->config.in2.port, motor->config.in2.pin);
    
    // 启动PWM定时器
    DL_TimerG_startCounter(motor->config.timer_inst);
    
    // 设置初始PWM占空比为0
    DL_TimerG_setCaptureCompareValue(motor->config.timer_inst, motor->speed, motor->config.pwm_channel);
}

// 电机速度限制
int Motor_Limit_Speed(MOTOR* motor, int speed, int max_speed, int min_speed)
{
    if(speed > max_speed)
        speed = max_speed;
    else if(speed < min_speed)
        speed = min_speed;

    return speed;
}

// 电机死区补偿
int Motor_Dead_Compensation(MOTOR* motor)
{
    if(motor->speed > 0 && motor->speed < motor->dead_band_speed)
        return motor->dead_band_speed;
    else if(motor->speed < 0 && motor->speed > -motor->dead_band_speed) 
        return -motor->dead_band_speed;
    else
        return motor->speed;
}

// 设置控制
void Motor_Set_Speed(MOTOR* motor, int speed)
{
    // 获取定时器周期值
    uint32_t timer_period = DL_TimerG_getLoadValue(motor->config.timer_inst);
    
    // 电机速度限制
    motor->speed = Motor_Limit_Speed(motor, speed, (int)timer_period, -(int)timer_period);
  
    // 电机死区补偿
    motor->speed = Motor_Dead_Compensation(motor);

    // 根据速度设置正转或反转方向
    if(motor->speed >= 0) // 正转
    {
        if(motor->config.reverse == 0) {
            DL_GPIO_setPins(motor->config.in1.port, motor->config.in1.pin);
            DL_GPIO_clearPins(motor->config.in2.port, motor->config.in2.pin);
        } else {
            DL_GPIO_clearPins(motor->config.in1.port, motor->config.in1.pin);
            DL_GPIO_setPins(motor->config.in2.port, motor->config.in2.pin);
        }
    }
    else // 反转
    {
        if(motor->config.reverse == 0) {
            DL_GPIO_clearPins(motor->config.in1.port, motor->config.in1.pin);
            DL_GPIO_setPins(motor->config.in2.port, motor->config.in2.pin);
        } else {
            DL_GPIO_setPins(motor->config.in1.port, motor->config.in1.pin);
            DL_GPIO_clearPins(motor->config.in2.port, motor->config.in2.pin);
        }
    }

    // 根据速度设置PWM占空比
    DL_TimerG_setCaptureCompareValue(motor->config.timer_inst, Motor_ABS(motor->speed), motor->config.pwm_channel);
}

// 电机停止  
void Motor_Stop(MOTOR* motor)
{
    DL_GPIO_clearPins(motor->config.in1.port, motor->config.in1.pin);
    DL_GPIO_clearPins(motor->config.in2.port, motor->config.in2.pin);

    motor->speed = 0;
    DL_TimerG_setCaptureCompareValue(motor->config.timer_inst, Motor_ABS(motor->speed), motor->config.pwm_channel);
}    

// 电机刹车
void Motor_Brake(MOTOR* motor)
{
    DL_GPIO_setPins(motor->config.in1.port, motor->config.in1.pin);
    DL_GPIO_setPins(motor->config.in2.port, motor->config.in2.pin);

    motor->speed = 0;
    DL_TimerG_setCaptureCompareValue(motor->config.timer_inst, Motor_ABS(motor->speed), motor->config.pwm_channel);
}