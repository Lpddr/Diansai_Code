#include "uart_motor.h"
//X轴电机
uint8_t uart2_dma_rx_buffer[UART_RX_BUFFER_SIZE];
uint8_t uart2_read_buffer[UART_RX_BUFFER_SIZE];
uint8_t uart2_flag;
//Y轴电机
uint8_t uart4_dma_rx_buffer[UART_RX_BUFFER_SIZE];
uint8_t uart4_read_buffer[UART_RX_BUFFER_SIZE];
uint8_t uart4_flag;
//视觉
uint8_t uart6_dma_rx_buffer[UART_RX_BUFFER_SIZE];
uint8_t uart6_read_buffer[UART_RX_BUFFER_SIZE];
uint8_t uart6_flag;

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
    //开启下一次DMA传输
    HAL_UARTEx_ReceiveToIdle_DMA(&huart2, uart2_dma_rx_buffer, sizeof(uart2_dma_rx_buffer));
    //如果关闭了半满中断，需再次打开
    __HAL_DMA_DISABLE_IT(&hdma_usart2_rx, DMA_IT_HT);
    
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

void Uart_Proc(void)
{
	//如果无数据直接返回
	if (uart2_flag)
      {
        //如果有数据进行解析
        Emm_V5_Parse_Response(uart2_read_buffer,sizeof(uart2_read_buffer),&motor1);
//        my_printf(&huart6,"X:%s\r\n",uart6_read_buffer);
        memset(uart2_read_buffer, 0, sizeof(uart2_read_buffer));
        uart2_flag = 0;
      }
      if (uart4_flag)
      {
        //如果有数据进行解析
        Emm_V5_Parse_Response(uart4_read_buffer,sizeof(uart4_read_buffer),&motor2);
//        my_printf(&huart6,"Y:%s\r\n",uart6_read_buffer);
        memset(uart4_read_buffer, 0, sizeof(uart4_read_buffer));
        uart4_flag = 0;
      }
      if (uart6_flag)
      {
        //如果有数据进行解析
        my_printf(&huart6,"receive:%s\r\n",uart6_read_buffer);
        memset(uart6_read_buffer, 0, sizeof(uart6_read_buffer));
        uart6_flag = 0;
      }
}


void Stepmotor_Task(void)
{
//    Emm_V5_Pos_Control(&huart2, 0x01, 0, 50, 10, 2864,0,0);
//    Emm_V5_Vel_Control(&huart4, 0x01, 0, 200, 0, 0);
//    Emm_V5_Read_Sys_Params(&huart2,0x01,S_CPOS);
//    Emm_V5_Read_Sys_Params(&huart2,0x01,S_CPOS);
} 


