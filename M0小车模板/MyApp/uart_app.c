#include "uart_app.h"


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