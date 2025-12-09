#ifndef __ENCODER_DRIVER_H__
#define __ENCODER_DRIVER_H__

#include "bsp_system.h"

// --- 用户需要根据实际情况修改这里的宏定义 ---
// 编码器每转一圈的脉冲数 (PPR)
#define ENCODER_PPR (13 * 20 * 4) // 13线/相, 20倍减速比, 4倍频
// 车轮直径 (单位: 厘米)
#define WHEEL_DIAMETER_CM 4.8f
// --- 修改结束 ---

// 自动计算周长和采样时间
#define PI 3.14159265f
#define WHEEL_CIRCUMFERENCE_CM (WHEEL_DIAMETER_CM * PI)
#define SAMPLING_TIME_S 0.005f // 采样时间, 与 Scheduler 中的任务周期一致 (5ms)
// 低通滤波器参数
#define SPEED_FILTER_ALPHA 0.3f  // 滤波系数，范围0-1，越小滤波效果越强
/**
 * @brief 编码器数据结构体
 */
typedef struct
{
  TIM_HandleTypeDef *htim; // 定时器
  unsigned char reverse; // 编码器的方向是否反转。0-正常，1-反转
  int16_t count;          // 当前采样周期内的原始计数值
  int32_t total_count;    // 累计总计数值
  float speed_cm_s;     // 计算出的速度 (cm/s)
  float distance;
} Encoder;

extern Encoder left_encoder;
extern Encoder right_encoder;

void Encoder_Driver_Init(Encoder* encoder, TIM_HandleTypeDef *htim, unsigned char reverse);
void Encoder_Driver_Update(Encoder* encoder);

#endif

