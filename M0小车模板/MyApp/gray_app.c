#include "gray_app.h"
// 全局变量
uint8_t Digital;                    // 开关量
float gray_weights[7] = {-3.0f, -2.0f, -1.0f, 0.0f, 1.0f, 2.0f, 3.0f}; // 7路灰度通道权重表
float g_line_position_error;        // 循迹误差值

// 灰度传感器引脚定义
#define GRAY_SENSOR_COUNT 7

/**
 * @brief 灰度传感器初始化
 */
void Gray_Init(void) 
{
    // 配置各个引脚为输入模式，启用内部上拉电阻
    DL_GPIO_initDigitalInput(Gray_PIN_0_IOMUX);
    DL_GPIO_initDigitalInput(Gray_PIN_1_IOMUX);
    DL_GPIO_initDigitalInput(Gray_PIN_2_IOMUX);
    DL_GPIO_initDigitalInput(Gray_PIN_3_IOMUX);
    DL_GPIO_initDigitalInput(Gray_PIN_4_IOMUX);
    DL_GPIO_initDigitalInput(Gray_PIN_5_IOMUX);
    DL_GPIO_initDigitalInput(Gray_PIN_6_IOMUX);
    
    // 使能内部上拉电阻
    DL_GPIO_setPins(Gray_PIN_0_PORT, Gray_PIN_0_PIN);
    DL_GPIO_setPins(Gray_PIN_1_PORT, Gray_PIN_1_PIN);
    DL_GPIO_setPins(Gray_PIN_2_PORT, Gray_PIN_2_PIN);
    DL_GPIO_setPins(Gray_PIN_3_PORT, Gray_PIN_3_PIN);
    DL_GPIO_setPins(Gray_PIN_4_PORT, Gray_PIN_4_PIN);
    DL_GPIO_setPins(Gray_PIN_5_PORT, Gray_PIN_5_PIN);
    DL_GPIO_setPins(Gray_PIN_6_PORT, Gray_PIN_6_PIN);
    
    // 初始化误差值
    g_line_position_error = 0.0f;
}

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
    
    if (!DL_GPIO_readPins(Gray_PIN_0_PORT, Gray_PIN_0_PIN)) {
        sensor_data |= (1 << 0);  // sensor0 对应最左边
    }
    if (!DL_GPIO_readPins(Gray_PIN_1_PORT, Gray_PIN_1_PIN)) {
        sensor_data |= (1 << 1);  // sensor1
    }
    if (!DL_GPIO_readPins(Gray_PIN_2_PORT, Gray_PIN_2_PIN)) {
        sensor_data |= (1 << 2);  // sensor2
    }
    if (!DL_GPIO_readPins(Gray_PIN_3_PORT, Gray_PIN_3_PIN)) {
        sensor_data |= (1 << 3);  // sensor3 对应中间
    }
    if (!DL_GPIO_readPins(Gray_PIN_4_PORT, Gray_PIN_4_PIN)) {
        sensor_data |= (1 << 4);  // sensor4
    }
    if (!DL_GPIO_readPins(Gray_PIN_5_PORT, Gray_PIN_5_PIN)) {
        sensor_data |= (1 << 5);  // sensor5
    }
    if (!DL_GPIO_readPins(Gray_PIN_6_PORT, Gray_PIN_6_PIN)) {
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

