#ifndef MYDEFINE_H
#define MYDEFINE_H

/*系统头文件*/
#include "ti_msp_dl_config.h"
#include "clock.h"
/*C语言头文件*/
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <stdarg.h>

/*应用层头文件*/
#include "scheduler.h"
#include "clock.h"
#include "wit_imu.h"
#include "oled_app.h"
#include "uart_app.h"
#include "key_app.h"
#include "motor_app.h"
#include "gray_app.h"
#include "pid_app.h"
#include "ADC.h"
/*底层驱动头文件*/
#include "oled_driver.h"
#include "motor_driver.h"
#include "encoder_driver.h"
#include "pid_driver.h"
#include "No_Mcu_Ganv_Grayscale_Sensor_Config.h"

#define PRINT(window, fmt, args...) my_printf(UART_CH340_INST, "{"#window"}"fmt"\n", ##args)

extern float initialize_yaw;
extern bool chushihua_yaw;
extern volatile uint16_t ADC_value ;
#endif
