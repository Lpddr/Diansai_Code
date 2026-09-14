/**
 * @file uart_motor.c
 * @brief UART通信适配与板间协议处理。
 *
 * 职责边界：
 * - My_UartInit/HAL_UARTEx_RxEventCallback：底层适配，负责DMA接收、转存和事件通知；
 * - Parse_M0_Data：服务逻辑，负责解析MSPM0运动遥测并更新融合状态；
 * - 本工程规模较小，暂时保留在同一文件；中断回调不执行控制算法和复杂解析。
 */
#include "uart_motor.h"
//X轴电机
uint8_t uart2_dma_rx_buffer[UART_RX_BUFFER_SIZE];
uint8_t uart2_read_buffer[UART_RX_BUFFER_SIZE];
volatile uint8_t uart2_flag;
//MSPM0通信
uint8_t uart4_dma_rx_buffer[UART_RX_BUFFER_SIZE];
uint8_t uart4_read_buffer[UART_RX_BUFFER_SIZE];
volatile uint8_t uart4_flag;
volatile uint16_t uart4_size;
bool Turn_Flag;
uint8_t car_point;
//视觉
uint8_t uart6_dma_rx_buffer[UART_RX_BUFFER_SIZE];
uint8_t uart6_read_buffer[UART_RX_BUFFER_SIZE];
volatile uint8_t uart6_flag;
volatile uint16_t uart6_size;

int my_printf(UART_HandleTypeDef *huart, const char *format, ...)
{
   char buffer[512];
   va_list arg;
   int len;
   
   va_start(arg,format);
   len=vsnprintf(buffer,sizeof(buffer),format,arg);
   va_end(arg);
   HAL_UART_Transmit(huart, (uint8_t *)buffer, (uint16_t)len, 0xFF);
   return len;
}



void My_UartInit(void)
{
//    //开启下一次DMA传输
//    HAL_UARTEx_ReceiveToIdle_DMA(&huart2, uart2_dma_rx_buffer, sizeof(uart2_dma_rx_buffer));
//    //如果关闭了半满中断，需再次打开
//    __HAL_DMA_DISABLE_IT(&hdma_usart2_rx, DMA_IT_HT);
//    
    //开启下一次DMA传输
    HAL_UARTEx_ReceiveToIdle_DMA(&huart4, uart4_dma_rx_buffer, sizeof(uart4_dma_rx_buffer));
    //如果关闭了半满中断，需再次打开
    __HAL_DMA_DISABLE_IT(&hdma_uart4_rx, DMA_IT_HT);
    
    //开启下一次DMA传输
    HAL_UARTEx_ReceiveToIdle_DMA(&huart6, uart6_dma_rx_buffer, sizeof(uart6_dma_rx_buffer));
    //如果关闭了半满中断，需再次打开
    __HAL_DMA_DISABLE_IT(&hdma_usart6_rx, DMA_IT_HT);
}


void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef* huart, uint16_t Size)
{
	if (huart->Instance == USART2)
	{
		//关闭DMA传输
		HAL_UART_DMAStop(huart);

		//复制到缓冲区
		memcpy((char*)uart2_read_buffer,(char*)uart2_dma_rx_buffer,Size);
		//标志位拉高
		uart2_flag = 1;

		//清空DMA缓冲区
		memset(uart2_dma_rx_buffer, 0, sizeof(uart2_dma_rx_buffer));

		//开启下一次DMA传输
		HAL_UARTEx_ReceiveToIdle_DMA(&huart2, uart2_dma_rx_buffer, sizeof(uart2_dma_rx_buffer));

		//如果关闭了半满中断，需再次打开
		__HAL_DMA_DISABLE_IT(&hdma_usart2_rx, DMA_IT_HT);
	}
    
    else if(huart->Instance == UART4)
    {
        //关闭DMA传输
		HAL_UART_DMAStop(huart);

		//复制到缓冲区
		memcpy((char*)uart4_read_buffer,(char*)uart4_dma_rx_buffer,Size);
		uart4_size = Size;
		//标志位拉高
		uart4_flag = 1;

		//清空DMA缓冲区
		memset(uart4_dma_rx_buffer, 0, sizeof(uart4_dma_rx_buffer));

		//开启下一次DMA传输
		HAL_UARTEx_ReceiveToIdle_DMA(&huart4, uart4_dma_rx_buffer, sizeof(uart4_dma_rx_buffer));

		//如果关闭了半满中断，需再次打开
		__HAL_DMA_DISABLE_IT(&hdma_uart4_rx, DMA_IT_HT);
    }
    else if(huart->Instance == USART6)
    {
        //关闭DMA传输
		HAL_UART_DMAStop(huart);
		//复制到缓冲区
		memcpy((char*)uart6_read_buffer,(char*)uart6_dma_rx_buffer,Size);
        uart6_size=Size;
		//标志位拉高
		uart6_flag = 1;

		//清空DMA缓冲区
		memset(uart6_dma_rx_buffer, 0, sizeof(uart6_dma_rx_buffer));

		//开启下一次DMA传输
		HAL_UARTEx_ReceiveToIdle_DMA(&huart6, uart6_dma_rx_buffer, sizeof(uart6_dma_rx_buffer));

		//如果关闭了半满中断，需再次打开
		__HAL_DMA_DISABLE_IT(&hdma_usart6_rx, DMA_IT_HT);
}
}

static bool parse_scaled_integer(const char **cursor,
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
    if (!parse_scaled_integer(&cursor, end, &gimbal_rate_x10) ||
        cursor >= end || *cursor++ != ',' ||
        !parse_scaled_integer(&cursor, end, &encoder_diff_x10) ||
        cursor >= end || *cursor++ != ',' ||
        !parse_scaled_integer(&cursor, end, &point) ||
        cursor >= end || *cursor != '#')
    {
        return;
    }

    /* IMU安装在云台上：角速度供内环反馈，编码器差速供车体前馈。 */
    MotionFusion_Update((float)gimbal_rate_x10 / 10.0f,
                        (float)encoder_diff_x10 / 10.0f,
                        (uint8_t)point);

    if (point >= 1 && point <= 9 && (uint8_t)point != car_point)
    {
        car_point = (uint8_t)point;
        Turn_Flag = true;
        Timer_Turn = 0U;
        pid_mode = (car_point == 1U || car_point == 4U ||
                    car_point == 5U || car_point == 8U) ? 2U : 3U;
    }
}


void Uart_Proc(void)
{
	//如果无数据直接返回
	if (uart2_flag)
      {
        //如果有数据进行解析
//        Emm_V5_Parse_Response(uart2_read_buffer,sizeof(uart2_read_buffer),&motor1);
//        my_printf(&huart6,"X:%s\r\n",uart6_read_buffer);
        memset(uart2_read_buffer, 0, sizeof(uart2_read_buffer));
        uart2_flag = 0;
      }

}





