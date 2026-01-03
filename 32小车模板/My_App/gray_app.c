#include "gray_app.h"

// 全局变量
uint8_t Digital;                    // 开关量
float gray_weights[7] = {-3.0f, -2.0f, -1.0f, 0.0f, 1.0f, 2.0f, 3.0f}; // 7路灰度通道权重表
float g_line_position_error;        // 循迹误差值

/**
 * @brief 读取7路灰度传感器数据
 * @return 传感器数据字节（bit0-bit6对应sensor0-sensor6）
 */
uint8_t Gray_Read_Sensors(void) 
{
    uint8_t sensor_data = 0;
    // 读取每个传感器的状态
    // 传感器检测到黑线时输出低电平(0)，检测到白线时输出高电平(1)
    // 这里取反，使得检测到黑线时对应位为1
    
    if (!HAL_GPIO_ReadPin(Gray_Pin_0_GPIO_Port, Gray_Pin_0_Pin)) {
        sensor_data |= (1 << 0);  // sensor0 对应最左边
    }
    if (!HAL_GPIO_ReadPin(Gray_Pin_1_GPIO_Port, Gray_Pin_1_Pin)) {
        sensor_data |= (1 << 1);  // sensor1
    }
    if (!HAL_GPIO_ReadPin(Gray_Pin_2_GPIO_Port, Gray_Pin_2_Pin)) {
        sensor_data |= (1 << 2);  // sensor2
    }
    if (!HAL_GPIO_ReadPin(Gray_Pin_3_GPIO_Port, Gray_Pin_3_Pin)) {
        sensor_data |= (1 << 3);  // sensor3 对应中间
    }
    if (!HAL_GPIO_ReadPin(Gray_Pin_4_GPIO_Port, Gray_Pin_4_Pin)) {
        sensor_data |= (1 << 4);  // sensor4
    }
    if (!HAL_GPIO_ReadPin(Gray_Pin_5_GPIO_Port, Gray_Pin_5_Pin)) {
        sensor_data |= (1 << 5);  // sensor5
    }
    if (!HAL_GPIO_ReadPin(Gray_Pin_6_GPIO_Port, Gray_Pin_6_Pin)) {
        sensor_data |= (1 << 6);  // sensor6 对应最右边
    }
    
    return sensor_data;
}

/**
 * @brief 灰度传感器任务处理
 */
void Gray_Task(void)
{
    // 获取传感器开关量结果
    Digital = Gray_Read_Sensors();
    
    float weighted_sum = 0;
    uint8_t black_line_count = 0;
    
    // 计算加权和
    for(uint8_t i = 0; i < GRAY_SENSOR_COUNT; i++)
    {
        if((Digital >> i) & 0x01)
        {
            weighted_sum += gray_weights[i];
            black_line_count++;
        }
    }
    
    // 计算循迹误差
    if(black_line_count > 0)
    {
        g_line_position_error = weighted_sum / (float)black_line_count;
    }
    else
    {
        // 没有检测到黑线时，保持上一次的误差值
        // 可选：设为0或根据历史数据推测
        // g_line_position_error = 0.0f;
    }
}


