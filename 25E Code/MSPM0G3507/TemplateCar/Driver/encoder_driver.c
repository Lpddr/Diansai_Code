#include "encoder_driver.h"
Encoder left_encoder;
Encoder right_encoder;

void Encoder_Driver_Init(Encoder* encoder, Encoder_GPIO_Config* gpio_config) {
    // 复制GPIO配置
    encoder->gpio_config = *gpio_config;

    // 初始化计数器和状态
    encoder->pulse_count = 0;
    encoder->last_pulse_count = 0;
    encoder->count = 0;
    encoder->total_count = 0;
    encoder->speed_cm_s = 0.0f;
    encoder->last_a_state = 0;
    encoder->last_b_state = 0;
}

/**
 * @brief 配置编码器GPIO和初始化
 */
void Encoder_Init(void) {
    // 左编码器GPIO配置 (根据宏定义修改)
    Encoder_GPIO_Config left_config = {
        .gpio_port_a = Encoder_Left_PORT,                    // GPIOA
        .gpio_pin_a = Encoder_Left_front_l_PIN,             // DL_GPIO_PIN_12 (A12/E1_A)
        .gpio_port_b = Encoder_Left_PORT,                    // GPIOA
        .gpio_pin_b = Encoder_Left_back_l_PIN               // DL_GPIO_PIN_13 (A13/E1_B)
    };
    
    // 右编码器GPIO配置 (根据宏定义修改)
    Encoder_GPIO_Config right_config = {
        .gpio_port_a = Encoder_Right_PORT,                   // GPIOA
        .gpio_pin_a = Encoder_Right_front_r_PIN,            // DL_GPIO_PIN_15 (A15/E2_A)
        .gpio_port_b = Encoder_Right_PORT,                   // GPIOA
        .gpio_pin_b = Encoder_Right_back_r_PIN              // DL_GPIO_PIN_24 (A24/E2_B)
    };
    
    // 初始化编码器
    Encoder_Driver_Init(&left_encoder, &left_config);   // 左编码器
    Encoder_Driver_Init(&right_encoder, &right_config); // 右编码器

    //打开外部中断
    NVIC_EnableIRQ(GPIOA_INT_IRQn);
    NVIC_EnableIRQ(TIMER_10ms_INST_INT_IRQN);
}


void Update_Encoder_Data_Filtered(Encoder *encoder) {
    // 1. 计算本周期内的脉冲增量
    int32_t current_pulse = encoder->pulse_count;
    int32_t pulse_delta = current_pulse - encoder->last_pulse_count;
    
    // 2. 更新count（本周期增量）
    encoder->count = (int16_t)pulse_delta;
    
    // 3. 更新总计数
    encoder->total_count += pulse_delta;
    
    // 4. 计算瞬时速度
    float instant_speed = ((float)pulse_delta / ENCODER_PPR) 
                         * WHEEL_CIRCUMFERENCE_CM / SAMPLING_TIME_S;
    
    // 5. 速度低通滤波（平滑处理）
    encoder->speed_cm_s = SPEED_FILTER_ALPHA * encoder->speed_cm_s + 
                         (1.0f - SPEED_FILTER_ALPHA) * instant_speed;
    
    // 6. 更新上次脉冲计数
    encoder->last_pulse_count = current_pulse;
    
    // 7. 更新GPIO状态
    encoder->last_a_state = (DL_GPIO_readPins(encoder->gpio_config.gpio_port_a, 
                                             encoder->gpio_config.gpio_pin_a) != 0) ? 1 : 0;
    encoder->last_b_state = (DL_GPIO_readPins(encoder->gpio_config.gpio_port_b, 
                                             encoder->gpio_config.gpio_pin_b) != 0) ? 1 : 0;
}