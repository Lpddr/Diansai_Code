/*
 * Copyright (c) 2021, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "Mydefine.h"


volatile bool ADC_flag;
volatile uint16_t ADC_value ;

int main(void)
{
    SYSCFG_DL_init();
    SysTick_Init();
    OLED_Init();
    WIT_Init();
    Uart_CH340_Init();
    Motor_Init(); 
    Encoder_Init();
    PID_Init();
    Scheduler_Init();
    NVIC_EnableIRQ(ADC1_INST_INT_IRQN);

    // 新增：初始化灰度传感器
    No_MCU_Ganv_Sensor_Init_Frist(&GraySensor); // 首次初始化
    // 使用预设的黑白阈值初始化（来自gpio_toggle_output.c）
    unsigned short white[8] = {1800,1800,1800,1800,1800,1800,1800,1800};
    unsigned short black[8] = {300,300,300,300,300,300,300,300};
    No_MCU_Ganv_Sensor_Init(&GraySensor, white, black);

    OLED_ShowString(0,0,(uint8_t *)"Vl",8);
    OLED_ShowString(0,2,(uint8_t *)" Vr",8);
    OLED_ShowString(0,4,(uint8_t *)"  Yaw",8);
    OLED_ShowString(16*6,3,(uint8_t *)"Accel",8);
    OLED_ShowString(17*6,4,(uint8_t *)"Gyro",8);

    while (1) {
        
        Scheduler_Run();
    }
}

void ADC1_INST_IRQHandler(void)
{
    switch(DL_ADC12_getPendingInterrupt(ADC1_INST))  //读取adc的最优先的中断，并且去掉标志位
    {
        case DL_ADC12_IIDX_MEM0_RESULT_LOADED:  //这个时我们在配置里面的那个中断的标志
        ADC_value = DL_ADC12_getMemResult(ADC1_INST,ADC1_ADCMEM_0); //将得到的值放在ADC_value里面
        DL_ADC12_enableConversions(ADC1_INST);
            break;
        default:
            break;
    }
}

