#include "uart_app.h"

uint8_t uart_buffer[UART_MAX_BUFFER];
uint8_t uart_index;
bool uart_flag;
uint32_t uart_tick;

void Uart_Init(void)
{
    NVIC_ClearPendingIRQ(UART2_INST_INT_IRQN);
    NVIC_EnableIRQ(UART2_INST_INT_IRQN);
}

void UART_Transmit(UART_Regs *huart, uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
    uint16_t i = 0;
    uint32_t timeout_counter = 0;
    
    // 参数检查
    if (huart == NULL || pData == NULL || Size == 0) {
        return;
    }
    
    // 逐字节发送数据
    for (i = 0; i < Size; i++) {
        // 等待发送FIFO有空间
        timeout_counter = 0;
        while (DL_UART_isTXFIFOFull(huart)) {
            if (Timeout != 0) {
                timeout_counter++;
                // 简单的软件延时计数，实际应用中可以使用更精确的时间机制
                if (timeout_counter > (Timeout * 1000)) {
                    return;
                }
            }
            // 让出CPU时间片
            __WFI();
        }
        
        // 发送数据
        DL_UART_transmitData(huart, pData[i]);
    }
    
    // 等待所有数据发送完成
    timeout_counter = 0;
    while (DL_UART_isBusy(huart)) {
        if (Timeout != 0) {
            timeout_counter++;
            if (timeout_counter > (Timeout * 1000)) {
                return;
            }
        }
        __WFI();
    }
}

/**
 * @brief 不定长字符串发送函数 (类似printf)
 * 
 * @param huart     UART实例指针
 * @param format    格式化字符串
 * @param ...       可变参数
 * @return int      实际发送的字符数量
 */
int my_printf(UART_Regs *huart, const char *format, ...)
{
    char buffer[256];
    va_list arg;
    int len;
    
    va_start(arg, format);
    len = vsnprintf(buffer, sizeof(buffer), format, arg);
    va_end(arg);
    
    UART_Transmit(huart, (uint8_t *)buffer, (uint16_t)len, 0xFF);
    return len;
}

void UART2_INST_IRQHandler(void)
{
    switch (DL_UART_Main_getPendingInterrupt(UART2_INST)) {
    case DL_UART_MAIN_IIDX_RX:
        // 防止缓冲区溢出，留一个位置给'\0'
        if(uart_index < (UART_MAX_BUFFER - 1)) {
            uart_buffer[uart_index] = DL_UART_Main_receiveData(UART2_INST);
            uart_index++;
        } else {
            // 缓冲区满了，丢弃数据或者处理溢出
            DL_UART_Main_receiveData(UART2_INST); // 读取但丢弃
        }
        
        // 设置标志和更新时间戳
        uart_flag = true;
        uart_tick = tick_ms;  // 每接收一个字符都更新时间戳
        break;
    default:
        break;
    }
}


void Uart_Proc(void)
{
    if(uart_flag == false) return;
    
    // 检查超时：如果距离最后一次接收数据超过100ms，认为一帧数据接收完成
    if(tick_ms - uart_tick >= 10)
    {
        // 确保字符串以'\0'结尾
        if(uart_index < UART_MAX_BUFFER) {
            uart_buffer[uart_index] = '\0';
        } else {
            uart_buffer[UART_MAX_BUFFER-1] = '\0';
        }
        
        // 打印接收到的数据
        my_printf(UART2_INST,"%s\r\n", uart_buffer);
        
        // 正确清零整个缓冲区
        memset(uart_buffer, 0, UART_MAX_BUFFER);
        uart_flag = false;  // 使用false而不是0
        uart_index = 0;
    }
}
