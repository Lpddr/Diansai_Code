#include "oled_app.h"



//void Oled_Task(void)
//{
//    // 清屏
//    char display_buf[64];
//    sprintf((char *)display_buf,"Ox:%d",g_circle.center_x);
//    OLED_PrintString(0,0,display_buf, &font16x16, OLED_COLOR_NORMAL);

//    sprintf((char *)display_buf,"Oy:%d",g_circle.center_y);
//    OLED_PrintString(0,16,display_buf, &font16x16, OLED_COLOR_NORMAL);

//    sprintf((char *)display_buf,"XOut:%.1f",X_Output);
//    OLED_PrintString(0,32,display_buf, &font16x16, OLED_COLOR_NORMAL);

//    sprintf((char *)display_buf,"Lx:%d",g_laser.x);
//    OLED_PrintString(64,0,display_buf, &font16x16, OLED_COLOR_NORMAL);

//    sprintf((char *)display_buf,"Ly:%d",g_laser.y);
//    OLED_PrintString(64,16,display_buf, &font16x16, OLED_COLOR_NORMAL);
//    
//    sprintf((char *)display_buf,"cnt:%d",count_test);
//    OLED_PrintString(64,32,display_buf, &font16x16, OLED_COLOR_NORMAL);

//    OLED_ShowFrame();
//}



// 快速整数转字符串函数 - 替代sprintf
static char* fast_itoa(int value, char* str) {
    char* ptr = str;
    char* ptr1 = str;
    char tmp_char;
    int tmp_value;
    
    // 处理负数
    if (value < 0) {
        *ptr++ = '-';
        value = -value;
        ptr1++;
    }
    
    // 转换数字
    do {
        tmp_value = value;
        value /= 10;
        *ptr++ = "0123456789"[tmp_value - value * 10];
    } while (value);
    
    *ptr-- = '\0';
    
    // 反转字符串
    while (ptr1 < ptr) {
        tmp_char = *ptr;
        *ptr-- = *ptr1;
        *ptr1++ = tmp_char;
    }
    
    return str;
}

// 快速浮点转字符串（1位小数）
static char* fast_ftoa1(float value, char* str) {
    int int_part = (int)value;
    int dec_part = (int)((value - int_part) * 10);
    if (dec_part < 0) dec_part = -dec_part;
    
    char* ptr = fast_itoa(int_part, str);
    while (*ptr) ptr++;  // 找到字符串末尾
    *ptr++ = '.';
    *ptr++ = '0' + dec_part;
    *ptr = '\0';
    
    return str;
}

// 优化的OLED显示函数
void Oled_Task(void)
{
    static char display_buf[16];  // 减小缓冲区
    
    // 使用快速转换函数替代sprintf
    strcpy(display_buf, "Ox:");
    fast_itoa(g_circle.center_x, display_buf + 3);
    OLED_PrintString(0, 0, display_buf, &font16x16, OLED_COLOR_NORMAL);
    
    strcpy(display_buf, "Oy:");
    fast_itoa(g_circle.center_y, display_buf + 3);
    OLED_PrintString(0, 16, display_buf, &font16x16, OLED_COLOR_NORMAL);
    
    strcpy(display_buf, "Mode:");
    fast_itoa(pid_mode, display_buf + 5);
    OLED_PrintString(0, 32, display_buf, &font16x16, OLED_COLOR_NORMAL);
    
    strcpy(display_buf, "Lx:");
    fast_itoa(g_laser.x, display_buf + 3);
    OLED_PrintString(64, 0, display_buf, &font16x16, OLED_COLOR_NORMAL);
    
    strcpy(display_buf, "fOK:");
    fast_itoa(test_flash.x, display_buf + 4);
    OLED_PrintString(64, 16, display_buf, &font16x16, OLED_COLOR_NORMAL);
    
    strcpy(display_buf, "run:");
    fast_itoa(Pid_Running, display_buf + 2);
    OLED_PrintString(64, 32, display_buf, &font16x16, OLED_COLOR_NORMAL);
    
    OLED_ShowFrame();
}


