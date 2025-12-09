#include "key_app.h"
unsigned char Key_Down,Key_Val,Key_Up,Key_Old;
unsigned char Key_Read()
{
    unsigned char temp=0;
    if(!DL_GPIO_readPins(Key_Pin0_PORT,Key_Pin0_PIN)) temp=1;
    if(!DL_GPIO_readPins(Key_Pin1_PORT,Key_Pin1_PIN)) temp=2;
 //   if(!DL_GPIO_readPins(Key_Pin2_PORT,Key_Pin2_PIN)) temp=3;
    return temp;
}

void Key_Task(void)
{
    Key_Val=Key_Read();
    Key_Down=Key_Val&(Key_Val^Key_Old);
    Key_Up=~Key_Val&(Key_Val^Key_Old);
    Key_Old=Key_Val;

    switch(Key_Down)
    {
        case 1:
        pid_running^=1;
        break;
        case 2:
        if(++car_turns>=6)
        car_turns=1;
        break;
        case 3:
        
        break;
    }
}