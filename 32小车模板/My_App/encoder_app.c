#include "encoder_app.h"


// 左右编码器电机
Encoder left_encoder;
Encoder right_encoder;

/**
 * @brief 初始化编码器应用
 */
void Encoder_Init(void)
{
  HAL_TIM_Base_Start_IT(&htim2);
  Encoder_Driver_Init(&left_encoder, &htim3, 1);
  Encoder_Driver_Init(&right_encoder, &htim4, 0);
}

/**
 * @brief 编码器应用运行任务 (应由调度器周期性调用)
 */
void Encoder_Task(void)
{
  Encoder_Driver_Update(&left_encoder);
  Encoder_Driver_Update(&right_encoder);
  
//  Uart_Printf(&huart1, "Left:%.2fcm/s  Right:%.2fcm/s\r\n", left_encoder.speed_cm_s, right_encoder.speed_cm_s);
}
