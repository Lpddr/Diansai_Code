#include "oled_app.h"



void Oled_Task(void)
{
    // «Â∆¡
    char display_buf[64];
    sprintf((char *)display_buf,"e1:%d",motor1.encoder);
    OLED_PrintString(0,0,display_buf, &font16x16, OLED_COLOR_NORMAL);

    sprintf((char *)display_buf,"e2:%d",motor2.encoder);
    OLED_PrintString(0,16,display_buf, &font16x16, OLED_COLOR_NORMAL);

    sprintf((char *)display_buf,"p1:%d",motor1.position);
    OLED_PrintString(0,32,display_buf, &font16x16, OLED_COLOR_NORMAL);

    sprintf((char *)display_buf,"p2:%d",motor2.position);
    OLED_PrintString(64,0,display_buf, &font16x16, OLED_COLOR_NORMAL);

    sprintf((char *)display_buf,"t1:%d",motor1.temperature);
    OLED_PrintString(64,16,display_buf, &font16x16, OLED_COLOR_NORMAL);

    sprintf((char *)display_buf,"t2:%d",motor2.temperature);
    OLED_PrintString(64,32,display_buf, &font16x16, OLED_COLOR_NORMAL);
    
    OLED_ShowFrame();
}

