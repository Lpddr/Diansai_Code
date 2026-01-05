/**
 * @file oled_app.h
 * @brief 基于链表的多级菜单系统（面试增强版）
 * @author Antigravity
 */
#ifndef __OLED_APP_H__
#define __OLED_APP_H__

#include "bsp_system.h"

// 菜单项类型定义
typedef enum
{
    TYPE_FOLDER, // 目录：点击进入子菜单
    TYPE_DATA,   // 数据：点击开始调整数值
    TYPE_ACTION  // 动作：点击执行回调函数
} MenuType_t;

// 菜单节点结构体 (核心：双向链表 + 树状结构)
typedef struct MenuNode
{
    const char *Name;        // 菜单项名称
    MenuType_t Type;         // 节点类型
    struct MenuNode *Parent; // 父节点指针
    struct MenuNode *Child;  // 子节点指针
    struct MenuNode *Next;   // 同级下一个指针
    struct MenuNode *Prev;   // 同级上一个指针

    void (*Callback)(void); // 回调函数指针
    float *ValuePtr;        // 绑定的数据指针 (调参关键)
    float Step;             // 步长
} MenuNode_t;

// 公开接口
void Menu_Init(void);              // 初始化菜单树
void Menu_Run(void);               // 菜单运行主循环（显示+按键处理）
void Menu_ProcessKey(uint8_t key); // 外部按键注入接口

#endif /* __OLED_APP_H__ */
