// parse_app.h
#ifndef PARSE_APP_H
#define PARSE_APP_H
#include "bsp_system.h"

// 限幅滤波配置宏定义
#define CIRCLE_LIMIT_THRESHOLD  50    // 圆心坐标限幅阈值
#define LASER_LIMIT_THRESHOLD   50    // 激光坐标限幅阈值

// 圆结构体，包含圆心和圆上的点
typedef struct {
    int center_x;     // 圆心X坐标
    int center_y;     // 圆心Y坐标
} Circle_t;

// 激光结构体
typedef struct {
    int x;  // 激光X坐标
    int y;  // 激光Y坐标
} Laser_t;

void parse_serial_data(const char* buffer, uint16_t length);
extern Circle_t g_circle;  // 当前圆数据
extern Laser_t g_laser;    // 当前激光数据
extern Laser_t cross;      // 当前十字架坐标

#endif

