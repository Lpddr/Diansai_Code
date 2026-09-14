#ifndef CHASSIS_MOTION_SERVICE_H
#define CHASSIS_MOTION_SERVICE_H

#include <stdint.h>

#define CHASSIS_TELEMETRY_SCALE 10.0f

typedef struct
{
    /* IMU安装在云台上；编码器数据来自底盘左右轮。 */
    float imu_yaw_rate_dps;
    float encoder_speed_diff_cm_s;
    uint8_t corner_index;
} chassis_motion_state_t;

void ChassisMotion_Update5ms(void);
void ChassisMotion_TelemetryTask(void);
const chassis_motion_state_t *ChassisMotion_GetState(void);

#endif
