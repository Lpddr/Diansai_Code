/**
 ******************************************************************************
 * @file    imu_wit.h
 * @author  lingxing
 * @brief   维特智能 IMU 组件层 (手动解析版)
 ******************************************************************************
 * @usage 使用说明:
 * 1. 初始化: IMU_Init();
 * 2. 数据输入 :
 *    IMU_ParsePacket(uart->rx_buffer, uart->rx_len);
 *    // 参数 1: 串口接收缓冲区的首地址
 *    // 参数 2: 本次实际收到的字节长度
 * 3. 获取数据: float yaw = IMU_GetYaw();
 ******************************************************************************
 */

#ifndef __IMU_WIT_H
#define __IMU_WIT_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/* IMU 数据结构体 */
typedef struct
{
    float roll;
    float pitch;
    float yaw;
    float yaw_continuous; /* 连续角度 (360°累加) */

    struct
    {
        float x, y, z;
    } acc;

    struct
    {
        float x, y, z;
    } gyro;

    float temp;
} IMU_Data_t;

extern IMU_Data_t g_imu_data;

/* --- 核心接口 --- */
void IMU_Init(void);
/**
 * @brief  解析维特智能原始数据序列
 * @param  p_data: 串口接收到的原始字节数组 (通常传入串口 DMA 的缓冲区指针)
 * @param  len:    本次解析的数据字节数
 * @note   函数会自动寻找 0x55 包头，如校验通过则更新全局角度、加速度、角速度数据。
 */
void IMU_ParsePacket(uint8_t *p_data, uint16_t len);

/* --- 数据获取 --- */
float IMU_GetYaw(void);
float IMU_GetPitch(void);
float IMU_GetRoll(void);

#endif /* __IMU_WIT_H */
