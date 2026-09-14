#ifndef MOTION_FUSION_SERVICE_H
#define MOTION_FUSION_SERVICE_H

#include <stdbool.h>
#include <stdint.h>

/*
 * IMU安装在云台上，gz作为角速度内环反馈。
 * 编码器位于底盘，通过左右轮速差反映车体转动趋势，仅用于前馈补偿。
 * 符号和增益需要根据IMU安装方向、编码器方向及机械结构完成实车标定。
 */
#define GIMBAL_IMU_RATE_SIGN             1.0f
#define ENCODER_BODY_FF_GAIN             0.12f
#define ENCODER_BODY_FF_SIGN             1.0f
#define ENCODER_BODY_FF_LPF_ALPHA        0.70f
#define ENCODER_BODY_FF_LIMIT            30.0f
#define MOTION_TELEMETRY_TIMEOUT_MS      50U

typedef struct
{
    float gimbal_yaw_rate_dps;
    float encoder_speed_diff_cm_s;
    float encoder_feedforward_output;
    uint8_t corner_index;
    uint32_t update_tick_ms;
    bool valid;
} motion_fusion_state_t;

void MotionFusion_Init(void);
void MotionFusion_Update(float gimbal_yaw_rate_dps,
                         float encoder_speed_diff_cm_s,
                         uint8_t corner_index);
bool MotionFusion_IsValid(void);
float MotionFusion_GetGimbalYawRate(void);
float MotionFusion_GetEncoderFeedforward(void);
const motion_fusion_state_t *MotionFusion_GetState(void);
void MotionFusion_HealthTask(void);

#endif
