#ifndef KEY_APP_H
#define KEY_APP_H

#include "bsp_system.h"
#define BTN_MODE1_ID    1
#define BTN_MODE2_ID    2
#define BTN_MODE3_ID    3
#define BTN_COMBO_ID    100  // MODE1+MODE2×éºÏ¼ü
void button_system_init(void);
void Key_Task(void);
#endif

