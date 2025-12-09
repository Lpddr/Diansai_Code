#include "ADC.h"

// 灰度传感器模拟值、数字值、归一化值
unsigned short Gray_Anolog[8] = {0};
unsigned short Gray_Normal[8] = {0};
unsigned char Gray_Digtal = 0;

unsigned int adc_getValue(void)
{
    unsigned int gAdcResult = 0;

    //使能ADC转换
    DL_ADC12_enableConversions(ADC1_INST);
    //软件触发ADC开始转换
    DL_ADC12_startConversion(ADC1_INST);

    //如果当前状态 不是 空闲状态
    while (DL_ADC12_getStatus(ADC1_INST) != DL_ADC12_STATUS_CONVERSION_IDLE );

    //清除触发转换状态
    DL_ADC12_stopConversion(ADC1_INST);
    //失能ADC转换
    DL_ADC12_disableConversions(ADC1_INST);

    //获取数据
    gAdcResult = DL_ADC12_getMemResult(ADC1_INST, ADC1_ADCMEM_0);

    return gAdcResult;
}

/**
 * @brief 灰度传感器数据读取任务（20ms）
 * 读取模拟值、数字值、归一化值并更新到全局变量
 */
void Task_GraySensor(void)
{
    // // 调用传感器处理函数（无时基版本，与gpio_toggle_output.c逻辑一致）
    // No_Mcu_Ganv_Sensor_Task_Without_tick(&GraySensor);

    // // 获取数字值（二值化结果）
    // Gray_Digtal = Get_Digtal_For_User(&GraySensor);

    // // 获取模拟值（原始ADC值）
    // Get_Anolog_Value(&GraySensor, Gray_Anolog);

    // // 获取归一化值（0~4095，基于12位ADC）
    // Get_Normalize_For_User(&GraySensor, Gray_Normal);
    DL_ADC12_startConversion(ADC1_INST);//开始转换adc值
}
