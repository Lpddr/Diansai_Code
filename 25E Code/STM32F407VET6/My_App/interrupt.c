#include "interrupt.h"
#include "imu_wit.h"
uint8_t Timer_10ms;
uint16_t Timer_Turn;
void My_TIM_Init(void)
{
    HAL_TIM_Base_Start_IT(&htim2);
    __HAL_TIM_SET_AUTORELOAD(&htim2, 1999);
    __HAL_TIM_SET_COUNTER(&htim2, 0);
}

void Uart_Parse(void)
{
    if (uart6_flag)
    {
        parse_serial_data((char *)uart6_read_buffer, uart6_size);
        memset(uart6_read_buffer, 0, sizeof(uart6_read_buffer));
        uart6_flag = 0;
    }
}

// TIM2 中断服务函数（1ms 中断）
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == htim2.Instance)
    {
        if (Turn_Flag)
        {
            if (++Timer_Turn >= TIME_TURN)
            {
                Pid_Running = 1;
                pid_mode = 0;
                Turn_Flag = 0;
                Timer_Turn = 0;
            }
        }

        if (uart4_flag)
        {
            // [新增] IMU 解析 (0x55 协议)
            IMU_ParsePacket((uint8_t *)uart4_read_buffer, UART_RX_BUFFER_SIZE);

            // 如果有数据进行解析
            Parse_M0_Data((const char *)uart4_read_buffer);
            memset(uart4_read_buffer, 0, strlen((char *)uart4_read_buffer));
            uart4_flag = 0;
        }

        if (++Timer_10ms >= 10)
        {
            Uart_Parse();
            Pid_Task();
        }
    }
}
