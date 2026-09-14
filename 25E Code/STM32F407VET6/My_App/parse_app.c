/**
 * @file parse_app.c
 * @brief 视觉坐标协议解析服务。
 *
 * 该文件虽然因历史原因位于My_App，但逻辑上属于服务层：负责把串口帧
 * 转换为g_circle、g_laser和cross等应用可使用的数据，不直接操作UART硬件。
 */
#include "parse_app.h"

// 全局结构体变量，实时更新
Circle_t g_circle = {0};  // 当前圆数据
Laser_t g_laser = {0};    // 当前激光数据
Laser_t cross = {0};      // 当前数据

// 快速字符串转整数函数 - 替代sscanf
static inline int fast_atoi(const char** str) {
    int result = 0;
    int sign = 1;
    const char* p = *str;
    
    // 处理符号
    if (*p == '-') {
        sign = -1;
        p++;
    } else if (*p == '+') {
        p++;
    }
    
    // 转换数字
    while (*p >= '0' && *p <= '9') {
        result = result * 10 + (*p - '0');
        p++;
    }
    
    *str = p; // 更新指针位置
    return result * sign;
}

// 快速解析一行数据
static inline void parse_packet(const char* start, const char* end) {
    // 检查最小长度和格式
    if (end - start < 4 || start[0] != '$') return;
    
    char type = start[1];
    const char* data_start = start + 3; // 跳过 "$X,"
    
    int x, y;
    const char* p = data_start;
    
    // 解析第一个数字
    x = fast_atoi(&p);
    
    // 检查是否有第二个数字
    if (*p == ',') {
        p++; // 跳过逗号
        y = fast_atoi(&p);
    } else {
        y = 0;
    }
    
    // 根据类型更新对应的数据结构
    switch (type) {
        case 'O': // 圆心数据
            g_circle.center_x = x;
            g_circle.center_y = y;
            break;
            
        case 'L': // 激光数据
            g_laser.x = (int16_t)x;
            g_laser.y = (int16_t)y;
            break;
            
        case 'C': // 十字架数据
            cross.x = (int16_t)x;
            cross.y = (int16_t)y;
            break;
    }
}

// 优化的串口数据解析主函数
void parse_serial_data(const char *buffer, uint16_t length)
{
    static char packet[UART_RX_BUFFER_SIZE];
    static uint16_t packet_length = 0;
    static bool receiving = false;

    for (uint16_t i = 0; i < length; i++)
    {
        char c = buffer[i];

        /* 遇到新帧头：丢弃之前未完成的残帧，重新接收。 */
        if (c == '$')
        {
            packet_length = 0;
            receiving = true;
            packet[packet_length++] = c;
            continue;
        }

        /* 没找到帧头之前，忽略无效数据。 */
        if (!receiving)
        {
            continue;
        }

        /* 缓冲区已满仍未收到帧尾，丢弃异常帧。 */
        if (packet_length >= sizeof(packet))
        {
            packet_length = 0;
            receiving = false;
            continue;
        }

        /* 保存实际数据，不再保存临时缓冲区中的地址。 */
        packet[packet_length++] = c;

        /* 收到完整帧后解析并复位，继续扫描后面的粘包数据。 */
        if (c == '#')
        {
            parse_packet(packet, packet + packet_length - 1);
            packet_length = 0;
            receiving = false;
        }
    }
}

