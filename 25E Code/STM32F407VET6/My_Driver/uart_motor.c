/**
 * @file uart_motor.c
 * @brief UART DMA接收及本项目板间通信协议处理。
 *
 * 该文件沿用原工程命名：中断中只接收和转存，协议解析由周期任务调用。
 */
#include "uart_motor.h"

#include <stdbool.h>
#include <string.h>

#include "interrupt.h"
#include "pid_app.h"
#include "usart.h"

extern DMA_HandleTypeDef hdma_uart4_rx;
extern DMA_HandleTypeDef hdma_usart6_rx;

static uint8_t uart4_dma_rx_buffer[UART_RX_BUFFER_SIZE];
uint8_t uart4_read_buffer[UART_RX_BUFFER_SIZE];
volatile uint8_t uart4_flag;
volatile uint16_t uart4_size;

static uint8_t uart6_dma_rx_buffer[UART_RX_BUFFER_SIZE];
uint8_t uart6_read_buffer[UART_RX_BUFFER_SIZE];
volatile uint8_t uart6_flag;
volatile uint16_t uart6_size;

static uint8_t car_point;

static void start_receive_to_idle(UART_HandleTypeDef *huart,
                                  DMA_HandleTypeDef *hdma,
                                  uint8_t *dma_buffer)
{
    HAL_UARTEx_ReceiveToIdle_DMA(huart, dma_buffer, UART_RX_BUFFER_SIZE);
    __HAL_DMA_DISABLE_IT(hdma, DMA_IT_HT);
}

static void save_received_data(UART_HandleTypeDef *huart,
                               DMA_HandleTypeDef *hdma,
                               uint8_t *dma_buffer,
                               uint8_t *read_buffer,
                               volatile uint16_t *received_size,
                               volatile uint8_t *ready_flag,
                               uint16_t size)
{
    HAL_UART_DMAStop(huart);

    if (size > UART_RX_BUFFER_SIZE)
    {
        size = UART_RX_BUFFER_SIZE;
    }

    memcpy(read_buffer, dma_buffer, size);
    *received_size = size;
    *ready_flag = 1U;

    memset(dma_buffer, 0, UART_RX_BUFFER_SIZE);
    start_receive_to_idle(huart, hdma, dma_buffer);
}

void My_UartInit(void)
{
    uart4_flag = 0U;
    uart4_size = 0U;
    uart6_flag = 0U;
    uart6_size = 0U;

    start_receive_to_idle(&huart4, &hdma_uart4_rx, uart4_dma_rx_buffer);
    start_receive_to_idle(&huart6, &hdma_usart6_rx, uart6_dma_rx_buffer);
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t size)
{
    if (huart->Instance == UART4)
    {
        save_received_data(huart,
                           &hdma_uart4_rx,
                           uart4_dma_rx_buffer,
                           uart4_read_buffer,
                           &uart4_size,
                           &uart4_flag,
                           size);
    }
    else if (huart->Instance == USART6)
    {
        save_received_data(huart,
                           &hdma_usart6_rx,
                           uart6_dma_rx_buffer,
                           uart6_read_buffer,
                           &uart6_size,
                           &uart6_flag,
                           size);
    }
}

static bool parse_integer(const char **cursor,
                          const char *end,
                          int32_t *value)
{
    int32_t result = 0;
    int32_t sign = 1;
    bool has_digit = false;
    const char *p = *cursor;

    if (p < end && (*p == '-' || *p == '+'))
    {
        sign = (*p == '-') ? -1 : 1;
        p++;
    }

    while (p < end && *p >= '0' && *p <= '9')
    {
        has_digit = true;
        result = result * 10 + (*p - '0');
        p++;
    }

    if (!has_digit)
    {
        return false;
    }

    *cursor = p;
    *value = result * sign;
    return true;
}

void Parse_M0_Data(const char *data, uint16_t length)
{
    const char *cursor = data;
    const char *end = data + length;
    int32_t gimbal_rate_x10;
    int32_t encoder_diff_x10;
    int32_t point;

    while ((cursor + 3) < end &&
           !(cursor[0] == '$' && cursor[1] == 'M' && cursor[2] == ','))
    {
        cursor++;
    }

    if ((cursor + 3) >= end)
    {
        return;
    }

    cursor += 3;
    if (!parse_integer(&cursor, end, &gimbal_rate_x10) ||
        cursor >= end || *cursor++ != ',' ||
        !parse_integer(&cursor, end, &encoder_diff_x10) ||
        cursor >= end || *cursor++ != ',' ||
        !parse_integer(&cursor, end, &point) ||
        cursor >= end || *cursor != '#')
    {
        return;
    }

    Pid_UpdateMotionFeedback((float)gimbal_rate_x10 / 10.0f,
                             (float)encoder_diff_x10 / 10.0f);

    if (point >= 1 && point <= 9 && (uint8_t)point != car_point)
    {
        car_point = (uint8_t)point;
        pid_mode = (car_point == 1U || car_point == 4U ||
                    car_point == 5U || car_point == 8U) ? 2U : 3U;
        Turn_Timer_Start();
    }
}
