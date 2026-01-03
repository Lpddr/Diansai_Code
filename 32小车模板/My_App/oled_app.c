#include "oled_app.h"


void Oled_Task(void)
{
    char display_buf[64];
    OLED_NewFrame();
    sprintf((char *)display_buf,"yaw:%.2f",wit_data.yaw);
    OLED_PrintString(0,0,display_buf, &font16x16, OLED_COLOR_NORMAL);

    sprintf((char *)display_buf,"run:%d",pid_running);
    OLED_PrintString(0,16,display_buf, &font16x16, OLED_COLOR_NORMAL);

    sprintf((char *)display_buf,"mode:%d",pid_control_mode);
    OLED_PrintString(0,32,display_buf, &font16x16, OLED_COLOR_NORMAL);

    sprintf((char *)display_buf,"L:%.1f",left_encoder.speed_cm_s);
    OLED_PrintString(64,0,display_buf, &font16x16, OLED_COLOR_NORMAL);

    sprintf((char *)display_buf,"R:%.1f",right_encoder.speed_cm_s);
    OLED_PrintString(64,16,display_buf, &font16x16, OLED_COLOR_NORMAL);

    sprintf((char *)display_buf,"V:%d",basic_speed);
    OLED_PrintString(64,32,display_buf, &font16x16, OLED_COLOR_NORMAL);

    OLED_ShowFrame();
}

