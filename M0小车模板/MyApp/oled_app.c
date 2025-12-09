#include "oled_app.h"


uint8_t oled_buffer[32];

void Oled_Task(void)
{
    sprintf((char *)oled_buffer, "%-6.1f", wit_data.pitch);
    OLED_ShowString(5*8,0,oled_buffer,16);
    sprintf((char *)oled_buffer, "%-6.1f", wit_data.roll);
    OLED_ShowString(5*8,2,oled_buffer,16);
    sprintf((char *)oled_buffer, "%-6.1f", wit_data.yaw);
    OLED_ShowString(5*8,4,oled_buffer,16);

    sprintf((char *)oled_buffer, "%6d", wit_data.ax);
    OLED_ShowString(15*6,0,oled_buffer,8);
    sprintf((char *)oled_buffer, "%6d", wit_data.ay);
    OLED_ShowString(15*6,1,oled_buffer,8);
    sprintf((char *)oled_buffer, "%6d", wit_data.az);
    OLED_ShowString(15*6,2,oled_buffer,8);

    sprintf((char *)oled_buffer, "%6d", left_encoder.pulse_count);
    OLED_ShowString(15*6,5,oled_buffer,8);
    sprintf((char *)oled_buffer, "%6d", right_encoder.pulse_count);
    OLED_ShowString(15*6,6,oled_buffer,8);
    sprintf((char *)oled_buffer, "%c%c%c%c%c%c%c%c",
        (Digital & 0x80) ? '1' : '0',
        (Digital & 0x40) ? '1' : '0',
        (Digital & 0x20) ? '1' : '0',
        (Digital & 0x10) ? '1' : '0',
        (Digital & 0x08) ? '1' : '0',
        (Digital & 0x04) ? '1' : '0',
        (Digital & 0x02) ? '1' : '0',
        (Digital & 0x01) ? '1' : '0');
OLED_ShowString(15*6,7,oled_buffer,8);
}