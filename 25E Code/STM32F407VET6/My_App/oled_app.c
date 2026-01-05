/**
 * @file oled_app.c
 * @brief 基于链表的多级菜单系统实现
 *
 * 优势：
 * 1. 结构化解耦：UI逻辑与业务参数分离。
 * 2. 动态扩展：增加功能只需添加静态节点，无需修改状态机。
 */

#include "oled_app.h"
#include "oled.h"    // 引用现有 OLED 驱动
#include "key_app.h" // 引用按键

// --- 外部变量引用 (调参对象) ---
extern PID_para X_para;
extern PID_para Y_para;
extern uint8_t pid_mode;

// --- 静态菜单节点定义 (静态链表) ---
static MenuNode_t Main_Root, Menu_PID, Menu_Calibration, Menu_System;
static MenuNode_t PID_X_Kp, PID_X_Ki, PID_X_Kd;
static MenuNode_t SYS_Mode;

// 当前处于哪个节点
static MenuNode_t *Current_Node = &Main_Root;
// 是否处于数值编辑模式
static uint8_t Edit_Mode = 0;

/**
 * @brief 简单的辅助函数：节点挂载逻辑
 *
 * @param node   要初始化的菜单节点指针
 * @param name   菜单显示的字符串名称
 * @param type   节点类型 (目录/数据/动作)
 * @param parent 指向父节点的指针 (用于返回上一级)
 * @param prev   指向同级上一个节点的指针
 * @param next   指向同级下一个节点的指针
 * @param child  指向子节点起始位的指针 (如果是目录类型)
 */
static void Menu_Bind(MenuNode_t *node, const char *name, MenuType_t type, MenuNode_t *parent, MenuNode_t *prev, MenuNode_t *next, MenuNode_t *child)
{
    node->Name = name;
    node->Type = type;
    node->Parent = parent;
    node->Prev = prev;
    node->Next = next;
    node->Child = child;
    node->ValuePtr = NULL;
    node->Callback = NULL;
    node->Step = 0.1f;
}

/**
 * @brief 初始化菜单树结构 (手动连线)
 */
void Menu_Init(void)
{
    // 1. 一级目录: 主根节点 -> PID设置, 系统设置, 传感器校准
    Menu_Bind(&Main_Root, "  [Menu Root]", TYPE_FOLDER, NULL, NULL, NULL, &Menu_PID);

    Menu_Bind(&Menu_PID, " 1.PID Tuning", TYPE_FOLDER, &Main_Root, NULL, &Menu_System, &PID_X_Kp);
    Menu_Bind(&Menu_System, " 2.Sys Control", TYPE_FOLDER, &Main_Root, &Menu_PID, &Menu_Calibration, &SYS_Mode);
    Menu_Bind(&Menu_Calibration, " 3.Calibrate", TYPE_FOLDER, &Main_Root, &Menu_System, NULL, NULL);

    // 2. 二级目录: PID Tuning 内容
    Menu_Bind(&PID_X_Kp, " X_Kp", TYPE_DATA, &Menu_PID, NULL, &PID_X_Ki, NULL);
    PID_X_Kp.ValuePtr = &X_para.kp;
    PID_X_Kp.Step = 1.0f;

    Menu_Bind(&PID_X_Ki, " X_Ki", TYPE_DATA, &Menu_PID, &PID_X_Kp, &PID_X_Kd, NULL);
    PID_X_Ki.ValuePtr = &X_para.ki;
    PID_X_Ki.Step = 0.01f;

    Menu_Bind(&PID_X_Kd, " X_Kd", TYPE_DATA, &Menu_PID, &PID_X_Ki, NULL, NULL);
    PID_X_Kd.ValuePtr = &X_para.kd;
    PID_X_Kd.Step = 0.1f;

    // 3. 二级目录: System Control 内容
    Menu_Bind(&SYS_Mode, " Run Mode", TYPE_DATA, &Menu_System, NULL, NULL, NULL);
    // 注意: 这里也可以绑定 uint8_t，只需在显示时强制转换
    SYS_Mode.ValuePtr = (float *)&pid_mode;
    SYS_Mode.Step = 1.0f;

    Current_Node = &Menu_PID; // 初始停留在第一个功能项
}

/**
 * @brief 处理按键逻辑
 */
void Menu_ProcessKey(uint8_t key)
{
    if (key == 0)
        return;

    if (!Edit_Mode)
    {
        // 非编辑模式：上下翻页，左右进出
        switch (key)
        {
        case 1: // 下一项 (或上一项，根据硬件习惯)
            if (Current_Node->Next)
                Current_Node = Current_Node->Next;
            break;
        case 2: // 进入子菜单或数据项
            if (Current_Node->Type == TYPE_FOLDER && Current_Node->Child)
            {
                Current_Node = Current_Node->Child;
            }
            else if (Current_Node->Type == TYPE_DATA)
            {
                Edit_Mode = 1; // 进入编辑模式
            }
            else if (Current_Node->Callback)
            {
                Current_Node->Callback();
            }
            break;
        case 3: // 返回上一级
            if (Current_Node->Parent)
                Current_Node = Current_Node->Parent;
            break;
        }
    }
    else
    {
        // 编辑模式：调整数值
        switch (key)
        {
        case 1: // 数值增加
            if (Current_Node->ValuePtr)
                *(Current_Node->ValuePtr) += Current_Node->Step;
            break;
        case 3: // 数值减少
            if (Current_Node->ValuePtr)
                *(Current_Node->ValuePtr) -= Current_Node->Step;
            break;
        case 2: // 退出编辑模式
            Edit_Mode = 0;
            break;
        }
    }
}

/**
 * @brief 菜单渲染主函数
 */
void Menu_Run(void)
{
    char buf[32];

    // 1. 绘制引导栏
    OLED_PrintString(0, 0, "--- MENU SYSTEM ---", &font16x16, OLED_COLOR_NORMAL);

    // 2. 绘制当前选中的名称
    if (Edit_Mode)
    {
        OLED_PrintString(0, 20, "> Editing:", &font16x16, OLED_COLOR_NORMAL);
    }
    else
    {
        OLED_PrintString(0, 20, " Select:", &font16x16, OLED_COLOR_NORMAL);
    }

    OLED_PrintString(0, 36, (char *)Current_Node->Name, &font16x16, OLED_COLOR_REVERSED);

    // 3. 如果是数据节点，显示数值
    if (Current_Node->Type == TYPE_DATA && Current_Node->ValuePtr)
    {
        if (Edit_Mode)
        {
            sprintf(buf, "VAL: [%.2f]", *(Current_Node->ValuePtr));
        }
        else
        {
            sprintf(buf, "VAL: %.2f", *(Current_Node->ValuePtr));
        }
        OLED_PrintString(0, 50, buf, &font16x16, OLED_COLOR_NORMAL);
    }
    else if (Current_Node->Type == TYPE_FOLDER)
    {
        // 如果是在根目录显示实时坐标监控
        if (Current_Node == &Menu_PID || Current_Node == &Menu_System)
        {
            sprintf(buf, "X:%d Y:%d", g_circle.center_x, g_circle.center_y);
            OLED_PrintString(0, 50, buf, &font16x16, OLED_COLOR_NORMAL);
        }
        else
        {
            OLED_PrintString(0, 50, "[Sub-folder...]", &font16x16, OLED_COLOR_NORMAL);
        }
    }

    OLED_ShowFrame();
}
