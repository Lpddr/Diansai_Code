#include "EmmV5.h"

// 全局步进电机实例定义
STEPPER_MOTOR MotorX;
STEPPER_MOTOR MotorY;

/**
 * @brief 步进电机配置初始化
 */
void Stepper_Config_Init(STEPPER_MOTOR *motor, GPTIMER_Regs *timer_inst,
                         GPIO_Regs *direction_port, uint32_t direction_pin,
                         GPIO_Regs *pulse_port, uint32_t pulse_pin,
                         bool reverse) {
  if (motor == NULL)
    return;

  // 配置参数初始化
  motor->config.timer_inst = timer_inst;
  motor->config.direction_port = direction_port;
  motor->config.direction_pin = direction_pin;
  motor->config.pulse_port = pulse_port;
  motor->config.pulse_pin = pulse_pin;
  motor->config.reverse = reverse;

  // 控制参数初始化
  motor->control.current_rpm = 0.0f;
  motor->control.current_period = 0;
  motor->control.direction = STEPPER_DIR_CW;
  motor->Motor_running = 0;
  // 初始状态：默认方向，脉冲引脚低电平
  DL_GPIO_clearPins(direction_port, direction_pin); // 默认方向
  DL_GPIO_clearPins(pulse_port, pulse_pin);         // 脉冲引脚低电平
}

/**
 * @brief 停止步进电机
 */
void Stepper_Stop(STEPPER_MOTOR *motor) {
  if (motor == NULL)
    return;

  // 停止定时器
  DL_TimerG_stopCounter(motor->config.timer_inst);

  // 确保脉冲引脚为低电平
  DL_GPIO_clearPins(motor->config.pulse_port, motor->config.pulse_pin);

  // 重置控制参数
  motor->control.current_rpm = 0.0f;
  motor->control.current_period = 0;

  // 设置运行状态
  motor->Motor_running = 0;
}

/**
 * @brief 设置步进电机转速并启动
 */
bool Stepper_Set_Speed(STEPPER_MOTOR *motor, float rpm) {
  if (motor == NULL) {
    return false;
  }
  if (rpm == 0.0f) {
    Stepper_Stop(motor);
    return true;
  }

  // 如果电机正在运行，先停止
  // if (Stepper_Is_Running(motor)) {
  //     Stepper_Stop(motor);
  // }

  // 确定运行方向和绝对转速
  STEPPER_DIRECTION target_dir;
  float abs_rpm;

  if (rpm > 0) {
    target_dir = motor->config.reverse ? STEPPER_DIR_CCW : STEPPER_DIR_CW;
    abs_rpm = rpm;
  } else {
    target_dir = motor->config.reverse ? STEPPER_DIR_CW : STEPPER_DIR_CCW;
    abs_rpm = -rpm;
  }

  // RPM转换为PWM定时器周期值
  // 计算所需脉冲频率 (Hz)
  float pulse_freq_hz = (abs_rpm * PULSES_PER_REV) / 60.0f;

  // 检查频率是否超出定时器能力
  float max_freq = TIMER_FREQ_HZ / 10.0f; // 最小周期为10个定时器周期
  if (pulse_freq_hz > max_freq) {
    return false; // 频率过高，超出定时器能力
  }

  // 计算PWM周期值
  uint32_t period = (uint32_t)((TIMER_FREQ_HZ / pulse_freq_hz) - 1);

  // 确保周期值在合理范围内
  if (period < 10) {
    return false; // 周期太小
  }

  // 设置控制参数
  motor->control.current_rpm = rpm;
  motor->control.current_period = period;
  motor->control.direction = target_dir;


  // 设置方向引脚
  if (target_dir == STEPPER_DIR_CW) {
    DL_GPIO_clearPins(motor->config.direction_port,
                      motor->config.direction_pin);
  } else {
    DL_GPIO_setPins(motor->config.direction_port, motor->config.direction_pin);
  }

  // 配置定时器
  // DL_TimerG_stopCounter(motor->config.timer_inst);
  DL_TimerG_setLoadValue(motor->config.timer_inst, period);
  DL_TimerG_setCaptureCompareValue(motor->config.timer_inst, period / 2,
                                   DL_TIMER_CC_0_INDEX);

  // 启动定时器
  if (motor->Motor_running == 0) {
    DL_TimerG_startCounter(motor->config.timer_inst);
    motor->Motor_running=1;
  }

  return true;
}

/**
 * @brief 获取当前转速
 */
float Stepper_Get_Current_RPM(STEPPER_MOTOR *motor) {
  if (motor == NULL)
    return 0.0f;
  return motor->control.current_rpm;
}

/**
 * @brief 步进电机系统初始化
 */
void Stepper_System_Init(void) {
  // 这里可以添加系统级的初始化代码
  // 例如时钟配置、GPIO组配置等
  Stepper_Config_Init(&MotorX, PWM_X_INST, Motor_X_Dir_PORT, Motor_X_Dir_PIN,
                      GPIO_PWM_X_C0_PORT, GPIO_PWM_X_C0_PIN, 0);
  Stepper_Config_Init(&MotorY, PWM_Y_INST, Motor_Y_Dir_PORT, Motor_Y_Dir_PIN,
                      GPIO_PWM_Y_C0_PORT, GPIO_PWM_Y_C0_PIN, 0);

}