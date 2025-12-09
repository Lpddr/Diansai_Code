#include "uart_app.h"
uint8_t uart2_dma_rx_buffer[UART_RX_BUFFER_SIZE];
uint8_t uart2_read_buffer[UART_RX_BUFFER_SIZE];
uint16_t uart2_size;
uint8_t uart2_flag;
uint8_t uart4_dma_rx_buffer[UART_RX_BUFFER_SIZE];
uint8_t uart4_read_buffer[UART_RX_BUFFER_SIZE];
uint8_t uart4_flag;
WIT_Data_t wit_data;
void My_Uart_Init(void)
{
    //开启下一次DMA传输
    HAL_UARTEx_ReceiveToIdle_DMA(&huart2, uart2_dma_rx_buffer, sizeof(uart2_dma_rx_buffer));
    //如果关闭了半满中断，需再次打开
    __HAL_DMA_DISABLE_IT(&hdma_usart2_rx, DMA_IT_HT);
    
    //开启下一次DMA传输
    HAL_UARTEx_ReceiveToIdle_DMA(&huart4, uart4_dma_rx_buffer, sizeof(uart4_dma_rx_buffer));
    //如果关闭了半满中断，需再次打开
    __HAL_DMA_DISABLE_IT(&hdma_uart4_rx, DMA_IT_HT);
    
    
}

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


void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef* huart, uint16_t Size)
{
    if (huart->Instance == USART2)
    {
        // 关闭DMA传输
        HAL_UART_DMAStop(huart);
        memcpy(uart2_read_buffer,uart2_dma_rx_buffer,Size);
        uart2_size=Size;
        // 标志位拉高，表示数据已解析完成
        uart2_flag = 1;
        
        // 清空DMA缓冲区
        memset(uart2_dma_rx_buffer, 0, sizeof(uart2_dma_rx_buffer));
        
        // 开启下一次DMA传输
        HAL_UARTEx_ReceiveToIdle_DMA(&huart2, uart2_dma_rx_buffer, sizeof(uart2_dma_rx_buffer));
        
        // 如果关闭了半满中断，需再次打开
        __HAL_DMA_DISABLE_IT(&hdma_usart2_rx, DMA_IT_HT);
    }
    
    if (huart->Instance == UART4)
	{
		//关闭DMA传输
		HAL_UART_DMAStop(huart);

		//复制到缓冲区
		memcpy(uart4_read_buffer,uart4_dma_rx_buffer,Size);
		//标志位拉高
		uart4_flag = 1;

		//清空DMA缓冲区
		memset(uart4_dma_rx_buffer, 0, sizeof(uart4_dma_rx_buffer));

		//开启下一次DMA传输
		HAL_UARTEx_ReceiveToIdle_DMA(&huart4, uart4_dma_rx_buffer, sizeof(uart4_dma_rx_buffer));

		//如果关闭了半满中断，需再次打开
		__HAL_DMA_DISABLE_IT(&hdma_uart4_rx, DMA_IT_HT);
	}
}




void Uart_Proc(void)
{
	if(uart2_flag)
    {
        Wit_Parse(uart2_read_buffer,uart2_size);
        memset(uart2_read_buffer, 0, sizeof(uart2_read_buffer));
        uart2_flag = 0;
    }
    if(uart4_flag)
    {
        my_printf(&huart4, "receive:%s", uart4_read_buffer);
        memset(uart4_read_buffer, 0, sizeof(uart4_read_buffer));
        uart4_flag = 0;
    }
}

