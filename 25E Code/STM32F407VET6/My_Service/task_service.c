#include "task_service.h"

#include "bsp_system.h"

static uint16_t take_uart_frame(volatile uint8_t *ready_flag,
                                volatile uint16_t *received_size,
                                uint8_t *shared_buffer,
                                uint8_t *local_buffer,
                                uint16_t capacity)
{
    uint16_t size = 0U;
    uint32_t primask = __get_PRIMASK();

    __disable_irq();
    if (*ready_flag != 0U)
    {
        size = *received_size;
        if (size > capacity)
        {
            size = capacity;
        }
        memcpy(local_buffer, shared_buffer, size);
        *ready_flag = 0U;
    }
    if (primask == 0U)
    {
        __enable_irq();
    }

    return size;
}

void Vision_Service_Task(void)
{
    uint8_t frame[UART_RX_BUFFER_SIZE];
    uint16_t size = take_uart_frame(&uart6_flag,
                                    &uart6_size,
                                    uart6_read_buffer,
                                    frame,
                                    sizeof(frame));

    if (size > 0U)
    {
        parse_serial_data((const char *)frame, size);
    }
}

void Chassis_Service_Task(void)
{
    uint8_t frame[UART_RX_BUFFER_SIZE];
    uint16_t size = take_uart_frame(&uart4_flag,
                                    &uart4_size,
                                    uart4_read_buffer,
                                    frame,
                                    sizeof(frame));

    if (size > 0U)
    {
        Parse_M0_Data((const char *)frame, size);
    }
}

void Gimbal_Control_Service_Task(void)
{
    Pid_Task();
}
