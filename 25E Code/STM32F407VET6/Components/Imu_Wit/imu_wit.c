/**
 ******************************************************************************
 * @file    imu_wit.c
 * @author  lingxing
 * @brief   维特智能 IMU 极简解析实现
 ******************************************************************************
 */

#include "imu_wit.h"

/* 全局数据 */
IMU_Data_t g_imu_data;

/* 内部状态变量 (归零与连续化) */
static float g_yaw_offset = 0.0f;
static bool g_is_offset_ready = false;
static float g_last_raw_yaw = 0.0f;
static int g_rev_count = 0;

/**
 * @brief  归一化连续角度逻辑 (对标 MSPM0)
 */
static float _Update_Continuous_Yaw(float current_raw)
{
    if (!g_is_offset_ready)
    {
        g_last_raw_yaw = current_raw;
        return current_raw;
    }

    float diff = current_raw - g_last_raw_yaw;

    /* 检测越界 (180 -> -180 跳变) */
    if (diff > 300.0f)
    {
        g_rev_count--;
    }
    else if (diff < -300.0f)
    {
        g_rev_count++;
    }

    g_last_raw_yaw = current_raw;
    return current_raw + (float)g_rev_count * 360.0f;
}

void IMU_Init(void)
{
    g_is_offset_ready = false;
    g_rev_count = 0;
}

/**
 * @brief  核心解析函数 (手动解析 0x55 协议)
 * @param  p_data: 串口原始数据指针
 * @param  len:    数据长度
 */
void IMU_ParsePacket(uint8_t *p_data, uint16_t len)
{
    if (p_data == NULL || len < 11)
        return;

    /* 寻找包头 0x55 */
    for (uint16_t i = 0; i <= (len - 11); i++)
    {
        if (p_data[i] == 0x55)
        {
            /* 1. 校验和验证 */
            uint8_t sum = 0;
            for (uint8_t j = 0; j < 10; j++)
                sum += p_data[i + j];

            if (sum != p_data[i + 10])
                continue;

            /* 2. 类型分拣 */
            switch (p_data[i + 1])
            {
            case 0x51: /* 加速度 */
                g_imu_data.acc.x = (int16_t)(p_data[i + 3] << 8 | p_data[i + 2]) / 32768.0f * 16.0f;
                g_imu_data.acc.y = (int16_t)(p_data[i + 5] << 8 | p_data[i + 4]) / 32768.0f * 16.0f;
                g_imu_data.acc.z = (int16_t)(p_data[i + 7] << 8 | p_data[i + 6]) / 32768.0f * 16.0f;
                break;

            case 0x52: /* 角速度 */
                g_imu_data.gyro.x = (int16_t)(p_data[i + 3] << 8 | p_data[i + 2]) / 32768.0f * 2000.0f;
                g_imu_data.gyro.y = (int16_t)(p_data[i + 5] << 8 | p_data[i + 4]) / 32768.0f * 2000.0f;
                g_imu_data.gyro.z = (int16_t)(p_data[i + 7] << 8 | p_data[i + 6]) / 32768.0f * 2000.0f;
                break;

            case 0x53: /* 角度 */
            {
                float raw_roll = (int16_t)(p_data[i + 3] << 8 | p_data[i + 2]) / 32768.0f * 180.0f;
                float raw_pitch = (int16_t)(p_data[i + 5] << 8 | p_data[i + 4]) / 32768.0f * 180.0f;
                float raw_yaw = (int16_t)(p_data[i + 7] << 8 | p_data[i + 6]) / 32768.0f * 180.0f;

                /* 连续化处理 */
                float continuous = _Update_Continuous_Yaw(raw_yaw);

                /* 首次启动捕捉：强制归零 */
                if (!g_is_offset_ready)
                {
                    g_yaw_offset = continuous;
                    g_is_offset_ready = true;
                }

                g_imu_data.roll = raw_roll;
                g_imu_data.pitch = raw_pitch;
                g_imu_data.yaw = continuous - g_yaw_offset;
                g_imu_data.yaw_continuous = continuous;
                break;
            }
            }
            i += 10; /* 跳过已处理包 */
        }
    }
}

float IMU_GetYaw(void) { return g_imu_data.yaw; }
float IMU_GetPitch(void) { return g_imu_data.pitch; }
float IMU_GetRoll(void) { return g_imu_data.roll; }
