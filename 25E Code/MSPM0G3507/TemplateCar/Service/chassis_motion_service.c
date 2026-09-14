#include "chassis_motion_service.h"

#include "Mydefine.h"

static chassis_motion_state_t chassis_motion_state;

void ChassisMotion_Update5ms(void)
{
    /* V1.1：先缓存IMU角速度和左右轮速差，再由周期任务统一上报。 */
    /* IMU物理安装在云台上，gz作为云台角速度内环反馈。 */
    chassis_motion_state.imu_yaw_rate_dps = (float)wit_data.gz;
    chassis_motion_state.encoder_speed_diff_cm_s =
        right_encoder.speed_cm_s - left_encoder.speed_cm_s;
    chassis_motion_state.corner_index = car_point;
}

void ChassisMotion_TelemetryTask(void)
{
    int32_t gyro_x10 =
        (int32_t)(chassis_motion_state.imu_yaw_rate_dps * CHASSIS_TELEMETRY_SCALE);
    int32_t encoder_diff_x10 =
        (int32_t)(chassis_motion_state.encoder_speed_diff_cm_s * CHASSIS_TELEMETRY_SCALE);

    /* 板间协议：$M,IMU角速度x10,左右轮速差x10,拐点序号# */
    /* 第二个运动量来自底盘左右轮编码器差速，用于车体转动前馈。 */
    my_printf(UART_CH340_INST,
              "$M,%ld,%ld,%u#\r\n",
              (long)gyro_x10,
              (long)encoder_diff_x10,
              chassis_motion_state.corner_index);
}

const chassis_motion_state_t *ChassisMotion_GetState(void)
{
    return &chassis_motion_state;
}
