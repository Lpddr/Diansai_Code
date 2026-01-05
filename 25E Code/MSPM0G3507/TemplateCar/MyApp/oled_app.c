#include "oled_app.h"


uint8_t oled_buffer[32];

void Oled_Task(void)
{
    sprintf((char *)oled_buffer, "%.1f",left_encoder.speed_cm_s);
    OLED_ShowString(5*8,0,oled_buffer,16);
    sprintf((char *)oled_buffer, "%.1f",right_encoder.speed_cm_s);
    OLED_ShowString(5*8,2,oled_buffer,16);
    sprintf((char *)oled_buffer, "%c%c%c%c%c%c%c%c", 
        (Digital & 0x80) ? '1' : '0',
        (Digital & 0x40) ? '1' : '0', 
        (Digital & 0x20) ? '1' : '0',
        (Digital & 0x10) ? '1' : '0',
        (Digital & 0x08) ? '1' : '0',
        (Digital & 0x04) ? '1' : '0',
        (Digital & 0x02) ? '1' : '0',
        (Digital & 0x01) ? '1' : '0');
    OLED_ShowString(0, 4, oled_buffer, 8);

    sprintf((char *)oled_buffer, "Quan:%d", car_turns);
    OLED_ShowString(15*6,0,oled_buffer,8);
    sprintf((char *)oled_buffer, "Dian:%d",car_point);
    OLED_ShowString(15*6,1,oled_buffer,8);
    sprintf((char *)oled_buffer, "V:%u",ADC_value);
    OLED_ShowString(15*6,2,oled_buffer,8);

    sprintf((char *)oled_buffer, "%.2f", left_encoder.speed_cm_s);
    OLED_ShowString(15*6,5,oled_buffer,8);
    sprintf((char *)oled_buffer, "%.2f", right_encoder.speed_cm_s);
    OLED_ShowString(15*6,6,oled_buffer,8);
    sprintf((char *)oled_buffer, "err:%.1f",g_line_position_error);
    OLED_ShowString(15*6,7,oled_buffer,8);
    sprintf((char *)oled_buffer, "Mode:%d",system_mode);
    OLED_ShowString(0,7,oled_buffer,8);
}