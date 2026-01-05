#include "interrupt.h"

void SysTick_Handler(void)
{
    tick_ms++;
}


#if defined UART_BNO08X_INST_IRQHandler
void UART_BNO08X_INST_IRQHandler(void)
{
    uint8_t checkSum = 0;
    extern uint8_t bno08x_dmaBuffer[19];

    DL_DMA_disableChannel(DMA, DMA_BNO08X_CHAN_ID);
    uint8_t rxSize = 18 - DL_DMA_getTransferSize(DMA, DMA_BNO08X_CHAN_ID);

    if(DL_UART_isRXFIFOEmpty(UART_BNO08X_INST) == false)
        bno08x_dmaBuffer[rxSize++] = DL_UART_receiveData(UART_BNO08X_INST);

    for(int i=2; i<=14; i++)
        checkSum += bno08x_dmaBuffer[i];

    if((rxSize == 19) && (bno08x_dmaBuffer[0] == 0xAA) && (bno08x_dmaBuffer[1] == 0xAA) && (checkSum == bno08x_dmaBuffer[18]))
    {
        bno08x_data.index = bno08x_dmaBuffer[2];
        bno08x_data.yaw = (int16_t)((bno08x_dmaBuffer[4]<<8)|bno08x_dmaBuffer[3]) / 100.0;
        bno08x_data.pitch = (int16_t)((bno08x_dmaBuffer[6]<<8)|bno08x_dmaBuffer[5]) / 100.0;
        bno08x_data.roll = (int16_t)((bno08x_dmaBuffer[8]<<8)|bno08x_dmaBuffer[7]) / 100.0;
        bno08x_data.ax = (bno08x_dmaBuffer[10]<<8)|bno08x_dmaBuffer[9];
        bno08x_data.ay = (bno08x_dmaBuffer[12]<<8)|bno08x_dmaBuffer[11];
        bno08x_data.az = (bno08x_dmaBuffer[14]<<8)|bno08x_dmaBuffer[13];
    }
    
    uint8_t dummy[4];
    DL_UART_drainRXFIFO(UART_BNO08X_INST, dummy, 4);

    DL_DMA_setDestAddr(DMA, DMA_BNO08X_CHAN_ID, (uint32_t) &bno08x_dmaBuffer[0]);
    DL_DMA_setTransferSize(DMA, DMA_BNO08X_CHAN_ID, 18);
    DL_DMA_enableChannel(DMA, DMA_BNO08X_CHAN_ID);
}
#endif

#if defined UART_WIT_INST_IRQHandler

// 角度归一化函数：将角度限制在-180到180度范围内
float normalize_angle(float angle) {
    if (angle > 180.0f) {
        angle -= 360.0f;
    }
    if (angle < -180.0f) {
        angle += 360.0f;
    }
    return angle;
}

// 使用静态变量来保存上一次的状态
static float g_last_yaw = 0.0f;
static int g_revolution_count = 0;
static bool g_is_yaw_initialized = false;

/**
 * @brief 将一个在[-180, 180]范围内的yaw角度转换为连续的角度值。
 * 
 * @param current_yaw 从传感器读取的当前yaw值 (-180 to 180)。
 * @return float 连续的yaw角度值 (例如 370, -450 等)。
 */
float convert_to_continuous_yaw(float current_yaw) 
{
    // 定义一个阈值来检测“跳变”。这个值应该大于180，通常取270或300比较安全。
    const float WRAP_AROUND_THRESHOLD = 300.0f;

    // 首次调用时进行初始化
    if (!g_is_yaw_initialized) {
        g_last_yaw = current_yaw;
        g_is_yaw_initialized = true;
        g_revolution_count = 0;
    }

    // 计算与上次读数的差异
    float diff = current_yaw - g_last_yaw;

    // 检测是否发生了“跳变”
    if (diff > WRAP_AROUND_THRESHOLD) {
        // 从正角度跳到负角度 (例如, 从 170° 到 -175°), 实际是向右转, 圈数应该增加
        // 此时 diff 接近 -360 (例如 -175 - 170 = -345)
        // 这段逻辑处理的是从-180跳变到+180的情况，说明是向左转过界
        g_revolution_count--;
    } else if (diff < -WRAP_AROUND_THRESHOLD) {
        // 从负角度跳到正角度 (例如, 从 -170° 到 175°), 实际是向左转, 圈数应该减小
        // 此时 diff 接近 360 (例如 175 - (-170) = 345)
        // 这段逻辑处理的是从+180跳变到-180的情况，说明是向右转过界
        g_revolution_count++;
    }

    // 更新上次的yaw值以备下次调用
    g_last_yaw = current_yaw;

    // 计算连续的yaw值
    float continuous_yaw = current_yaw + (float)g_revolution_count * 360.0f;

    return continuous_yaw;
}


void UART_WIT_INST_IRQHandler(void)
{
    uint8_t checkSum, packCnt = 0;
    extern uint8_t wit_dmaBuffer[33];

    DL_DMA_disableChannel(DMA, DMA_WIT_CHAN_ID);
    uint8_t rxSize = 32 - DL_DMA_getTransferSize(DMA, DMA_WIT_CHAN_ID);

    if(DL_UART_isRXFIFOEmpty(UART_WIT_INST) == false)
        wit_dmaBuffer[rxSize++] = DL_UART_receiveData(UART_WIT_INST);

    while(rxSize >= 11)
    {
        checkSum=0;
        for(int i=packCnt*11; i<(packCnt+1)*11-1; i++)
            checkSum += wit_dmaBuffer[i];

        if((wit_dmaBuffer[packCnt*11] == 0x55) && (checkSum == wit_dmaBuffer[packCnt*11+10]))
        {
            if(wit_dmaBuffer[packCnt*11+1] == 0x51)
            {
                wit_data.ax = (int16_t)((wit_dmaBuffer[packCnt*11+3]<<8)|wit_dmaBuffer[packCnt*11+2]) / 2.048; //mg
                wit_data.ay = (int16_t)((wit_dmaBuffer[packCnt*11+5]<<8)|wit_dmaBuffer[packCnt*11+4]) / 2.048; //mg
                wit_data.az = (int16_t)((wit_dmaBuffer[packCnt*11+7]<<8)|wit_dmaBuffer[packCnt*11+6]) / 2.048; //mg
                wit_data.temperature =  (int16_t)((wit_dmaBuffer[packCnt*11+9]<<8)|wit_dmaBuffer[packCnt*11+8]) / 100.0; //°C
            }
            else if(wit_dmaBuffer[packCnt*11+1] == 0x52)
            {
                wit_data.gx = (int16_t)((wit_dmaBuffer[packCnt*11+3]<<8)|wit_dmaBuffer[packCnt*11+2]) / 16.384; //°/S
                wit_data.gy = (int16_t)((wit_dmaBuffer[packCnt*11+5]<<8)|wit_dmaBuffer[packCnt*11+4]) / 16.384; //°/S
                wit_data.gz = (int16_t)((wit_dmaBuffer[packCnt*11+7]<<8)|wit_dmaBuffer[packCnt*11+6]) / 16.384; //°/S
            }
            else if(wit_dmaBuffer[packCnt*11+1] == 0x53)
            {
                wit_data.roll  = (int16_t)((wit_dmaBuffer[packCnt*11+3]<<8)|wit_dmaBuffer[packCnt*11+2]) / 32768.0 * 180.0; //°
                wit_data.pitch = (int16_t)((wit_dmaBuffer[packCnt*11+5]<<8)|wit_dmaBuffer[packCnt*11+4]) / 32768.0 * 180.0; //°
                wit_data.yaw=convert_to_continuous_yaw((int16_t)((wit_dmaBuffer[packCnt*11+7]<<8)|wit_dmaBuffer[packCnt*11+6]) / 32768.0 * 180.0);
                wit_data.version = (int16_t)((wit_dmaBuffer[packCnt*11+9]<<8)|wit_dmaBuffer[packCnt*11+8]);
            }
        }

        rxSize -= 11;
        packCnt++;
    }
    
    uint8_t dummy[4];
    DL_UART_drainRXFIFO(UART_WIT_INST, dummy, 4);

    DL_DMA_setDestAddr(DMA, DMA_WIT_CHAN_ID, (uint32_t) &wit_dmaBuffer[0]);
    DL_DMA_setTransferSize(DMA, DMA_WIT_CHAN_ID, 32);
    DL_DMA_enableChannel(DMA, DMA_WIT_CHAN_ID);
}
#endif
#ifdef UART_CH340_INST
uint8_t rxch340Size;
void UART_CH340_INST_IRQHandler(void)
{
// 禁用DMA通道
    DL_DMA_disableChannel(DMA, DMA_CH340_CHAN_ID);
    
    // 计算实际接收到的数据长度
    rxch340Size = UART_MAX_BUFFER - DL_DMA_getTransferSize(DMA, DMA_CH340_CHAN_ID);

    // 检查UART FIFO中是否还有数据
    if(DL_UART_isRXFIFOEmpty(UART_CH340_INST) == false) {
        uart_dma_buffer[rxch340Size++] = DL_UART_receiveData(UART_CH340_INST);
    }
    
    // 如果有接收到数据
    if (rxch340Size > 0) {
        // 复制数据到处理缓冲区
        memcpy(uart_buffer, uart_dma_buffer, rxch340Size);
        //uart_buffer[rxch340Size] = '\0';  // 添加字符串结束符
        uart_flag = 1;  // 设置数据就绪标志
        
        // 现在清空DMA缓冲区，准备下次接收
        memset(uart_dma_buffer, 0, UART_MAX_BUFFER);
    }
    
    // 清空UART FIFO中的剩余数据
    uint8_t dummy[4];
    DL_UART_drainRXFIFO(UART_CH340_INST, dummy, 4);

    // 重新配置DMA准备下次接收
    DL_DMA_setDestAddr(DMA, DMA_CH340_CHAN_ID, (uint32_t) &uart_dma_buffer[0]);
    DL_DMA_setTransferSize(DMA, DMA_CH340_CHAN_ID, UART_MAX_BUFFER);
    DL_DMA_enableChannel(DMA, DMA_CH340_CHAN_ID);
}
#endif



//EXTI中断 
void GROUP1_IRQHandler(void) {
    // 获取触发中断的GPIO状态
    uint32_t GPIOA_Status = DL_GPIO_getEnabledInterruptStatus(GPIOA, 
        Encoder_Left_front_l_PIN | Encoder_Right_front_r_PIN);
    
    // 左编码器处理 (A相上升沿触发)
    if(GPIOA_Status & Encoder_Left_front_l_PIN) {
        if(DL_GPIO_readPins(GPIOA, Encoder_Left_back_l_PIN) == 0) {
            left_encoder.pulse_count--;    // B相低电平，正转
        } else {
            left_encoder.pulse_count++;    // B相高电平，反转
        }
        
        // 清除触发中断的引脚状态
        DL_GPIO_clearInterruptStatus(GPIOA, Encoder_Left_front_l_PIN);
    }
    
    // 右编码器处理 (A相上升沿触发)
    if(GPIOA_Status & Encoder_Right_front_r_PIN) {
        if(DL_GPIO_readPins(GPIOA, Encoder_Right_back_r_PIN) == 0) {
            right_encoder.pulse_count++;   // B相低电平，正转
        } else {
            right_encoder.pulse_count--;   // B相高电平，反转
        }
        
        // 清除触发中断的引脚状态
        DL_GPIO_clearInterruptStatus(GPIOA, Encoder_Right_front_r_PIN);
    }
}






//DEBUG
void NMI_Handler(void)
{
    __BKPT();
}

void HardFault_Handler(void)
{
    __BKPT();
}

void SVC_Handler(void)
{
    __BKPT();
}

void PendSV_Handler(void)
{
    __BKPT();
}


void GROUP0_IRQHandler(void)
{
    __BKPT();
}


void TIMG8_IRQHandler(void)
{
    __BKPT();
}

void UART3_IRQHandler(void)
{
    __BKPT();
}

// void ADC0_IRQHandler(void)
// {
//     __BKPT();
// }

void ADC1_IRQHandler(void)
{
    __BKPT();
}

void CANFD0_IRQHandler(void)
{
    __BKPT();
}

void DAC0_IRQHandler(void)
{
    __BKPT();
}

void SPI0_IRQHandler(void)
{
    __BKPT();
}

void SPI1_IRQHandler(void)
{
    __BKPT();
}


void UART2_IRQHandler(void)
{
    __BKPT();
}




void TIMG6_IRQHandler(void)
{
    __BKPT();
}

void TIMA0_IRQHandler(void)
{
    __BKPT();
}

void TIMA1_IRQHandler(void)
{
    __BKPT();
}

void TIMG7_IRQHandler(void)
{
    __BKPT();
}

void TIMG12_IRQHandler(void)
{
    __BKPT();
}

void I2C0_IRQHandler(void)
{
    __BKPT();
}

void I2C1_IRQHandler(void)
{
    __BKPT();
}

void AES_IRQHandler(void)
{
    __BKPT();
}

void RTC_IRQHandler(void)
{
    __BKPT();
}

void DMA_IRQHandler(void)
{
    __BKPT();
}