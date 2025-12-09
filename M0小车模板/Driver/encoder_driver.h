#ifndef __ENCODER_DRIVER_H__
#define __ENCODER_DRIVER_H__

#include "ti_msp_dl_config.h"

#define ENCODER_PPR (30)
#define WHEEL_DIAMETER_CM 3.7f
#define PI 3.14159265f
#define WHEEL_CIRCUMFERENCE_CM (WHEEL_DIAMETER_CM * PI)
#define SAMPLING_TIME_S 0.01f
#define SPEED_FILTER_ALPHA  0.8f    // 低通滤波系数 (0-1)

typedef struct {
    GPIO_Regs *gpio_port_a;
    uint32_t gpio_pin_a;
    GPIO_Regs *gpio_port_b;
    uint32_t gpio_pin_b;
} Encoder_GPIO_Config;

typedef struct {
    Encoder_GPIO_Config gpio_config;
    volatile int32_t pulse_count;
    int32_t last_pulse_count;
    int16_t count;
    int32_t total_count;
    float speed_cm_s;
    unsigned char last_a_state;
    unsigned char last_b_state;
} Encoder;


extern Encoder left_encoder;
extern Encoder right_encoder;

void Encoder_Init(void);
void Update_Encoder_Data_Filtered(Encoder *encoder);
#endif
