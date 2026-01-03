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



/* Defines for PWM_X */
#define PWM_X_INST                                                         TIMG8
#define PWM_X_INST_IRQHandler                                   TIMG8_IRQHandler
#define PWM_X_INST_INT_IRQN                                     (TIMG8_INT_IRQn)
#define PWM_X_INST_CLK_FREQ                                             40000000
/* GPIO defines for channel 0 */
#define GPIO_PWM_X_C0_PORT                                                 GPIOA
#define GPIO_PWM_X_C0_PIN                                          DL_GPIO_PIN_1
#define GPIO_PWM_X_C0_IOMUX                                       (IOMUX_PINCM2)
#define GPIO_PWM_X_C0_IOMUX_FUNC                      IOMUX_PINCM2_PF_TIMG8_CCP0
#define GPIO_PWM_X_C0_IDX                                    DL_TIMER_CC_0_INDEX

/* Defines for PWM_Y */
#define PWM_Y_INST                                                         TIMG0
#define PWM_Y_INST_IRQHandler                                   TIMG0_IRQHandler
#define PWM_Y_INST_INT_IRQN                                     (TIMG0_INT_IRQn)
#define PWM_Y_INST_CLK_FREQ                                             40000000
/* GPIO defines for channel 0 */
#define GPIO_PWM_Y_C0_PORT                                                 GPIOA
#define GPIO_PWM_Y_C0_PIN                                         DL_GPIO_PIN_12
#define GPIO_PWM_Y_C0_IOMUX                                      (IOMUX_PINCM34)
#define GPIO_PWM_Y_C0_IOMUX_FUNC                     IOMUX_PINCM34_PF_TIMG0_CCP0
#define GPIO_PWM_Y_C0_IDX                                    DL_TIMER_CC_0_INDEX




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


/* Defines for UART2 */
#define UART2_INST                                                         UART2
#define UART2_INST_FREQUENCY                                            40000000
#define UART2_INST_IRQHandler                                   UART2_IRQHandler
#define UART2_INST_INT_IRQN                                       UART2_INT_IRQn
#define GPIO_UART2_RX_PORT                                                 GPIOB
#define GPIO_UART2_TX_PORT                                                 GPIOB
#define GPIO_UART2_RX_PIN                                         DL_GPIO_PIN_16
#define GPIO_UART2_TX_PIN                                         DL_GPIO_PIN_15
#define GPIO_UART2_IOMUX_RX                                      (IOMUX_PINCM33)
#define GPIO_UART2_IOMUX_TX                                      (IOMUX_PINCM32)
#define GPIO_UART2_IOMUX_RX_FUNC                       IOMUX_PINCM33_PF_UART2_RX
#define GPIO_UART2_IOMUX_TX_FUNC                       IOMUX_PINCM32_PF_UART2_TX
#define UART2_BAUD_RATE                                                 (115200)
#define UART2_IBRD_40_MHZ_115200_BAUD                                       (21)
#define UART2_FBRD_40_MHZ_115200_BAUD                                       (45)





/* Defines for X_Dir: GPIOA.2 with pinCMx 7 on package pin 42 */
#define Motor_X_Dir_PORT                                                 (GPIOA)
#define Motor_X_Dir_PIN                                          (DL_GPIO_PIN_2)
#define Motor_X_Dir_IOMUX                                         (IOMUX_PINCM7)
/* Defines for Y_Dir: GPIOB.13 with pinCMx 30 on package pin 1 */
#define Motor_Y_Dir_PORT                                                 (GPIOB)
#define Motor_Y_Dir_PIN                                         (DL_GPIO_PIN_13)
#define Motor_Y_Dir_IOMUX                                        (IOMUX_PINCM30)
/* Defines for X_En: GPIOA.0 with pinCMx 1 on package pin 33 */
#define Motor_X_En_PORT                                                  (GPIOA)
#define Motor_X_En_PIN                                           (DL_GPIO_PIN_0)
#define Motor_X_En_IOMUX                                          (IOMUX_PINCM1)
/* Defines for Y_En: GPIOB.12 with pinCMx 29 on package pin 64 */
#define Motor_Y_En_PORT                                                  (GPIOB)
#define Motor_Y_En_PIN                                          (DL_GPIO_PIN_12)
#define Motor_Y_En_IOMUX                                         (IOMUX_PINCM29)



/* clang-format on */

void SYSCFG_DL_init(void);
void SYSCFG_DL_initPower(void);
void SYSCFG_DL_GPIO_init(void);
void SYSCFG_DL_SYSCTL_init(void);
void SYSCFG_DL_PWM_X_init(void);
void SYSCFG_DL_PWM_Y_init(void);
void SYSCFG_DL_I2C_OLED_init(void);
void SYSCFG_DL_UART2_init(void);

void SYSCFG_DL_SYSTICK_init(void);

bool SYSCFG_DL_saveConfiguration(void);
bool SYSCFG_DL_restoreConfiguration(void);

#ifdef __cplusplus
}
#endif

#endif /* ti_msp_dl_config_h */
