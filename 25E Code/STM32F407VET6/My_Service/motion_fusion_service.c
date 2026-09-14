#include "motion_fusion_service.h"

#include "main.h"

static motion_fusion_state_t motion_state;

static float constrain_float(float value, float minimum, float maximum)
{
    if (value < minimum)
    {
        return minimum;
    }
    if (value > maximum)
    {
        return maximum;
    }
    return value;
}

static bool telemetry_is_fresh(void)
{
    return motion_state.valid &&
           (uint32_t)(HAL_GetTick() - motion_state.update_tick_ms) <=
               MOTION_TELEMETRY_TIMEOUT_MS;
}

void MotionFusion_Init(void)
{
    motion_state.gimbal_yaw_rate_dps = 0.0f;
    motion_state.encoder_speed_diff_cm_s = 0.0f;
    motion_state.encoder_feedforward_output = 0.0f;
    motion_state.corner_index = 0U;
    motion_state.update_tick_ms = HAL_GetTick();
    motion_state.valid = false;
}

void MotionFusion_Update(float gimbal_yaw_rate_dps,
                         float encoder_speed_diff_cm_s,
                         uint8_t corner_index)
{
    float raw_encoder_feedforward;

    motion_state.gimbal_yaw_rate_dps =
        GIMBAL_IMU_RATE_SIGN * gimbal_yaw_rate_dps;
    motion_state.encoder_speed_diff_cm_s = encoder_speed_diff_cm_s;
    motion_state.corner_index = corner_index;

    /* 编码器差速只形成车体转动前馈，不与云台IMU角速度反馈混合。 */
    raw_encoder_feedforward = ENCODER_BODY_FF_SIGN *
                              ENCODER_BODY_FF_GAIN *
                              encoder_speed_diff_cm_s;
    raw_encoder_feedforward =
        constrain_float(raw_encoder_feedforward,
                        -ENCODER_BODY_FF_LIMIT,
                        ENCODER_BODY_FF_LIMIT);

    if (motion_state.valid)
    {
        motion_state.encoder_feedforward_output =
            ENCODER_BODY_FF_LPF_ALPHA *
                motion_state.encoder_feedforward_output +
            (1.0f - ENCODER_BODY_FF_LPF_ALPHA) *
                raw_encoder_feedforward;
    }
    else
    {
        motion_state.encoder_feedforward_output = raw_encoder_feedforward;
    }

    motion_state.update_tick_ms = HAL_GetTick();
    motion_state.valid = true;
}

bool MotionFusion_IsValid(void)
{
    return telemetry_is_fresh();
}

float MotionFusion_GetGimbalYawRate(void)
{
    return telemetry_is_fresh() ? motion_state.gimbal_yaw_rate_dps : 0.0f;
}

float MotionFusion_GetEncoderFeedforward(void)
{
    return telemetry_is_fresh() ?
               motion_state.encoder_feedforward_output :
               0.0f;
}

const motion_fusion_state_t *MotionFusion_GetState(void)
{
    return &motion_state;
}

void MotionFusion_HealthTask(void)
{
    if (motion_state.valid && !telemetry_is_fresh())
    {
        motion_state.valid = false;
        motion_state.encoder_feedforward_output = 0.0f;
    }
}
