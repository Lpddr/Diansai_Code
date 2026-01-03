/*
 * Copyright (c) 2023, Texas Instruments Incorporated - http://www.ti.com
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

/*
 *  ============ ti_msp_dl_config.h =============
 *  Configured MSPM0 DriverLib module declarations
 *
 *  DO NOT EDIT - This file is generated for the MSPM0G350X
 *  by the SysConfig tool.
 */
#ifndef ti_msp_dl_config_h
#define ti_msp_dl_config_h

#define CONFIG_MSPM0G350X
#define CONFIG_MSPM0G3507

#if defined(__ti_version__) || defined(__TI_COMPILER_VERSION__)
#define SYSCONFIG_WEAK __attribute__((weak))
#elif defined(__IAR_SYSTEMS_ICC__)
#define SYSCONFIG_WEAK __weak
#elif defined(__GNUC__)
#define SYSCONFIG_WEAK __attribute__((weak))
#endif

#include <ti/devices/msp/msp.h>
#include <ti/driverlib/driverlib.h>
#include <ti/driverlib/m0p/dl_core.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  ======== SYSCFG_DL_init ========
 *  Perform all required MSP DL initialization
 *
 *  This function should be called once at a point before any use of
 *  MSP DL.
 */


/* clang-format off */

#define POWER_STARTUP_DELAY                                                (16)



#define CPUCLK_FREQ                                                     80000000



/* Defines for PWM_MOTOR */
#define PWM_MOTOR_INST                                                     TIMG6
#define PWM_MOTOR_INST_IRQHandler                               TIMG6_IRQHandler
#define PWM_MOTOR_INST_INT_IRQN                                 (TIMG6_INT_IRQn)
#define PWM_MOTOR_INST_CLK_FREQ                                         80000000
/* GPIO defines for channel 0 */
#define GPIO_PWM_MOTOR_C0_PORT                                             GPIOB
#define GPIO_PWM_MOTOR_C0_PIN                                      DL_GPIO_PIN_6
#define GPIO_PWM_MOTOR_C0_IOMUX                                  (IOMUX_PINCM23)
#define GPIO_PWM_MOTOR_C0_IOMUX_FUNC                 IOMUX_PINCM23_PF_TIMG6_CCP0
#define GPIO_PWM_MOTOR_C0_IDX                                DL_TIMER_CC_0_INDEX
/* GPIO defines for channel 1 */
#define GPIO_PWM_MOTOR_C1_PORT                                             GPIOB
#define GPIO_PWM_MOTOR_C1_PIN                                      DL_GPIO_PIN_7
#define GPIO_PWM_MOTOR_C1_IOMUX                                  (IOMUX_PINCM24)
#define GPIO_PWM_MOTOR_C1_IOMUX_FUNC                 IOMUX_PINCM24_PF_TIMG6_CCP1
#define GPIO_PWM_MOTOR_C1_IDX                                DL_TIMER_CC_1_INDEX



/* Defines for TIMER_10ms */
#define TIMER_10ms_INST                                                  (TIMG0)
#define TIMER_10ms_INST_IRQHandler                              TIMG0_IRQHandler
#define TIMER_10ms_INST_INT_IRQN                                (TIMG0_INT_IRQn)
#define TIMER_10ms_INST_LOAD_VALUE                                         (19U)




/* Defines for I2C_OLED */
#define I2C_OLED_INST                                                       I2C1
#define I2C_OLED_INST_IRQHandler                                 I2C1_IRQHandler
#define I2C_OLED_INST_INT_IRQN                                     I2C1_INT_IRQn
#define I2C_OLED_BUS_SPEED_HZ                                             400000
#define GPIO_I2C_OLED_SDA_PORT                                             GPIOB
#define GPIO_I2C_OLED_SDA_PIN                                      DL_GPIO_PIN_3
#define GPIO_I2C_OLED_IOMUX_SDA                                  (IOMUX_PINCM16)
#define GPIO_I2C_OLED_IOMUX_SDA_FUNC                   IOMUX_PINCM16_PF_I2C1_SDA
#define GPIO_I2C_OLED_SCL_PORT                                             GPIOB
#define GPIO_I2C_OLED_SCL_PIN                                      DL_GPIO_PIN_2
#define GPIO_I2C_OLED_IOMUX_SCL                                  (IOMUX_PINCM15)
#define GPIO_I2C_OLED_IOMUX_SCL_FUNC                   IOMUX_PINCM15_PF_I2C1_SCL


/* Defines for UART_WIT */
#define UART_WIT_INST                                                      UART1
#define UART_WIT_INST_FREQUENCY                                         40000000
#define UART_WIT_INST_IRQHandler                                UART1_IRQHandler
#define UART_WIT_INST_INT_IRQN                                    UART1_INT_IRQn
#define GPIO_UART_WIT_RX_PORT                                              GPIOA
#define GPIO_UART_WIT_RX_PIN                                       DL_GPIO_PIN_9
#define GPIO_UART_WIT_IOMUX_RX                                   (IOMUX_PINCM20)
#define GPIO_UART_WIT_IOMUX_RX_FUNC                    IOMUX_PINCM20_PF_UART1_RX
#define UART_WIT_BAUD_RATE                                              (115200)
#define UART_WIT_IBRD_40_MHZ_115200_BAUD                                    (21)
#define UART_WIT_FBRD_40_MHZ_115200_BAUD                                    (45)
/* Defines for UART_CH340 */
#define UART_CH340_INST                                                    UART0
#define UART_CH340_INST_FREQUENCY                                       40000000
#define UART_CH340_INST_IRQHandler                              UART0_IRQHandler
#define UART_CH340_INST_INT_IRQN                                  UART0_INT_IRQn
#define GPIO_UART_CH340_RX_PORT                                            GPIOA
#define GPIO_UART_CH340_TX_PORT                                            GPIOA
#define GPIO_UART_CH340_RX_PIN                                    DL_GPIO_PIN_11
#define GPIO_UART_CH340_TX_PIN                                    DL_GPIO_PIN_10
#define GPIO_UART_CH340_IOMUX_RX                                 (IOMUX_PINCM22)
#define GPIO_UART_CH340_IOMUX_TX                                 (IOMUX_PINCM21)
#define GPIO_UART_CH340_IOMUX_RX_FUNC                  IOMUX_PINCM22_PF_UART0_RX
#define GPIO_UART_CH340_IOMUX_TX_FUNC                  IOMUX_PINCM21_PF_UART0_TX
#define UART_CH340_BAUD_RATE                                            (115200)
#define UART_CH340_IBRD_40_MHZ_115200_BAUD                                  (21)
#define UART_CH340_FBRD_40_MHZ_115200_BAUD                                  (45)





/* Defines for ADC1 */
#define ADC1_INST                                                           ADC0
#define ADC1_INST_IRQHandler                                     ADC0_IRQHandler
#define ADC1_INST_INT_IRQN                                       (ADC0_INT_IRQn)
#define ADC1_ADCMEM_0                                         DL_ADC12_MEM_IDX_0
#define ADC1_ADCMEM_0_REF                        DL_ADC12_REFERENCE_VOLTAGE_VDDA
#define ADC1_ADCMEM_0_REF_VOLTAGE_V                                          3.3
#define GPIO_ADC1_C0_PORT                                                  GPIOA
#define GPIO_ADC1_C0_PIN                                          DL_GPIO_PIN_27



/* Defines for DMA_WIT */
#define DMA_WIT_CHAN_ID                                                      (1)
#define UART_WIT_INST_DMA_TRIGGER                            (DMA_UART1_RX_TRIG)
/* Defines for DMA_CH340 */
#define DMA_CH340_CHAN_ID                                                    (0)
#define UART_CH340_INST_DMA_TRIGGER                          (DMA_UART0_RX_TRIG)


/* Port definition for Pin Group STBY */
#define STBY_PORT                                                        (GPIOB)

/* Defines for ebl: GPIOB.16 with pinCMx 33 on package pin 4 */
#define STBY_ebl_PIN                                            (DL_GPIO_PIN_16)
#define STBY_ebl_IOMUX                                           (IOMUX_PINCM33)
/* Port definition for Pin Group AIN */
#define AIN_PORT                                                         (GPIOB)

/* Defines for AIN1: GPIOB.20 with pinCMx 48 on package pin 19 */
#define AIN_AIN1_PIN                                            (DL_GPIO_PIN_20)
#define AIN_AIN1_IOMUX                                           (IOMUX_PINCM48)
/* Defines for AIN2: GPIOB.24 with pinCMx 52 on package pin 23 */
#define AIN_AIN2_PIN                                            (DL_GPIO_PIN_24)
#define AIN_AIN2_IOMUX                                           (IOMUX_PINCM52)
/* Port definition for Pin Group BIN */
#define BIN_PORT                                                         (GPIOB)

/* Defines for BIN1: GPIOB.18 with pinCMx 44 on package pin 15 */
#define BIN_BIN1_PIN                                            (DL_GPIO_PIN_18)
#define BIN_BIN1_IOMUX                                           (IOMUX_PINCM44)
/* Defines for BIN2: GPIOB.19 with pinCMx 45 on package pin 16 */
#define BIN_BIN2_PIN                                            (DL_GPIO_PIN_19)
#define BIN_BIN2_IOMUX                                           (IOMUX_PINCM45)
/* Port definition for Pin Group Encoder_Left */
#define Encoder_Left_PORT                                                (GPIOA)

/* Defines for front_l: GPIOA.12 with pinCMx 34 on package pin 5 */
// groups represented: ["Encoder_Right","Encoder_Left"]
// pins affected: ["front_r","front_l"]
#define GPIO_MULTIPLE_GPIOA_INT_IRQN                            (GPIOA_INT_IRQn)
#define GPIO_MULTIPLE_GPIOA_INT_IIDX            (DL_INTERRUPT_GROUP1_IIDX_GPIOA)
#define Encoder_Left_front_l_IIDX                           (DL_GPIO_IIDX_DIO12)
#define Encoder_Left_front_l_PIN                                (DL_GPIO_PIN_12)
#define Encoder_Left_front_l_IOMUX                               (IOMUX_PINCM34)
/* Defines for back_l: GPIOA.13 with pinCMx 35 on package pin 6 */
#define Encoder_Left_back_l_PIN                                 (DL_GPIO_PIN_13)
#define Encoder_Left_back_l_IOMUX                                (IOMUX_PINCM35)
/* Port definition for Pin Group Encoder_Right */
#define Encoder_Right_PORT                                               (GPIOA)

/* Defines for front_r: GPIOA.15 with pinCMx 37 on package pin 8 */
#define Encoder_Right_front_r_IIDX                          (DL_GPIO_IIDX_DIO15)
#define Encoder_Right_front_r_PIN                               (DL_GPIO_PIN_15)
#define Encoder_Right_front_r_IOMUX                              (IOMUX_PINCM37)
/* Defines for back_r: GPIOA.24 with pinCMx 54 on package pin 25 */
#define Encoder_Right_back_r_PIN                                (DL_GPIO_PIN_24)
#define Encoder_Right_back_r_IOMUX                               (IOMUX_PINCM54)
/* Defines for Pin0: GPIOB.21 with pinCMx 49 on package pin 20 */
#define Key_Pin0_PORT                                                    (GPIOB)
#define Key_Pin0_PIN                                            (DL_GPIO_PIN_21)
#define Key_Pin0_IOMUX                                           (IOMUX_PINCM49)
/* Defines for Pin1: GPIOA.17 with pinCMx 39 on package pin 10 */
#define Key_Pin1_PORT                                                    (GPIOA)
#define Key_Pin1_PIN                                            (DL_GPIO_PIN_17)
#define Key_Pin1_IOMUX                                           (IOMUX_PINCM39)
/* Port definition for Pin Group LED_Port */
#define LED_Port_PORT                                                    (GPIOB)

/* Defines for Led: GPIOB.26 with pinCMx 57 on package pin 28 */
#define LED_Port_Led_PIN                                        (DL_GPIO_PIN_26)
#define LED_Port_Led_IOMUX                                       (IOMUX_PINCM57)
/* Defines for Beep: GPIOB.8 with pinCMx 25 on package pin 60 */
#define LED_Port_Beep_PIN                                        (DL_GPIO_PIN_8)
#define LED_Port_Beep_IOMUX                                      (IOMUX_PINCM25)
/* Port definition for Pin Group ADC */
#define ADC_PORT                                                         (GPIOB)

/* Defines for PIN0: GPIOB.12 with pinCMx 29 on package pin 64 */
#define ADC_PIN0_PIN                                            (DL_GPIO_PIN_12)
#define ADC_PIN0_IOMUX                                           (IOMUX_PINCM29)
/* Defines for PIN1: GPIOB.17 with pinCMx 43 on package pin 14 */
#define ADC_PIN1_PIN                                            (DL_GPIO_PIN_17)
#define ADC_PIN1_IOMUX                                           (IOMUX_PINCM43)
/* Defines for PIN2: GPIOB.4 with pinCMx 17 on package pin 52 */
#define ADC_PIN2_PIN                                             (DL_GPIO_PIN_4)
#define ADC_PIN2_IOMUX                                           (IOMUX_PINCM17)



/* clang-format on */

void SYSCFG_DL_init(void);
void SYSCFG_DL_initPower(void);
void SYSCFG_DL_GPIO_init(void);
void SYSCFG_DL_SYSCTL_init(void);
void SYSCFG_DL_PWM_MOTOR_init(void);
void SYSCFG_DL_TIMER_10ms_init(void);
void SYSCFG_DL_I2C_OLED_init(void);
void SYSCFG_DL_UART_WIT_init(void);
void SYSCFG_DL_UART_CH340_init(void);
void SYSCFG_DL_ADC1_init(void);
void SYSCFG_DL_DMA_init(void);

void SYSCFG_DL_SYSTICK_init(void);

bool SYSCFG_DL_saveConfiguration(void);
bool SYSCFG_DL_restoreConfiguration(void);

#ifdef __cplusplus
}
#endif

#endif /* ti_msp_dl_config_h */
