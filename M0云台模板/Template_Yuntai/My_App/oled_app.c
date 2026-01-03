#include "oled_app.h"
uint8_t count;
uint8_t oled_buffer[32];
void Oled_Task(void)
{
    
    count++;
    sprintf((char *)oled_buffer, "test:%d", count);
    OLED_ShowString(5*8,0,oled_buffer,16);
}