#include "key_app.h"
#include "oled_app.h"

uint8_t Key_Down, Key_Val, Key_Up, Key_Old;

uint8_t Key_Read(void)
{
    uint8_t temp = 0;
    if (!HAL_GPIO_ReadPin(KEY1_GPIO_Port, KEY1_Pin))
        temp = 1;
    if (!HAL_GPIO_ReadPin(KEY2_GPIO_Port, KEY2_Pin))
        temp = 2;
    if (!HAL_GPIO_ReadPin(KEY3_GPIO_Port, KEY3_Pin))
        temp = 3;

    return temp;
}

void Key_Task(void)
{
    Key_Val = Key_Read();
    Key_Down = Key_Val & (Key_Val ^ Key_Old);
    Key_Up = ~Key_Val & (Key_Val ^ Key_Old);
    Key_Old = Key_Val;

    // 注入菜单系统
    if (Key_Down != 0)
    {
        Menu_ProcessKey(Key_Down);
    }

    // 保留部分核心后台逻辑 (如保存坐标)
    if (Key_Down == 3)
    {
        W25QXX_WriteCoordinate(0, g_laser.x, g_laser.y);
        W25QXX_ReadCoordinate(0, &test_flash.x, &test_flash.y);
    }
}
