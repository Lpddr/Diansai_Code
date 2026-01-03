#include "wit.h"

void Wit_Parse(uint8_t* data,uint16_t Size)
{
    // WIT传感器数据解析
        uint8_t checkSum, packCnt = 0;
        uint16_t rxSize = Size;
        // 处理接收到的数据包
        while(rxSize >= 11)
        {
            checkSum = 0;
            // 计算校验和(前10个字节)
            for(int i = packCnt * 11; i < (packCnt + 1) * 11 - 1; i++)
                checkSum += data[i];
            
            // 验证包头和校验和
            if((data[packCnt * 11] == 0x55) && 
               (checkSum == data[packCnt * 11 + 10]))
            {
                // 解析加速度数据包 (0x51)
                if(data[packCnt * 11 + 1] == 0x51)
                {
                    wit_data.ax = (int16_t)((data[packCnt * 11 + 3] << 8) | 
                                           data[packCnt * 11 + 2]) / 2.048f; // mg
                    wit_data.ay = (int16_t)((data[packCnt * 11 + 5] << 8) | 
                                           data[packCnt * 11 + 4]) / 2.048f; // mg
                    wit_data.az = (int16_t)((data[packCnt * 11 + 7] << 8) | 
                                           data[packCnt * 11 + 6]) / 2.048f; // mg
                    wit_data.temperature = (int16_t)((data[packCnt * 11 + 9] << 8) | 
                                                    data[packCnt * 11 + 8]) / 100.0f; // °C
                }
                // 解析陀螺仪数据包 (0x52)
                else if(data[packCnt * 11 + 1] == 0x52)
                {
                    wit_data.gx = (int16_t)((data[packCnt * 11 + 3] << 8) | 
                                           data[packCnt * 11 + 2]) / 16.384f; // °/s
                    wit_data.gy = (int16_t)((data[packCnt * 11 + 5] << 8) | 
                                           data[packCnt * 11 + 4]) / 16.384f; // °/s
                    wit_data.gz = (int16_t)((data[packCnt * 11 + 7] << 8) | 
                                           data[packCnt * 11 + 6]) / 16.384f; // °/s
                }
                // 解析姿态角数据包 (0x53)
                else if(data[packCnt * 11 + 1] == 0x53)
                {
                    wit_data.roll = (int16_t)((data[packCnt * 11 + 3] << 8) | 
                                             data[packCnt * 11 + 2]) / 32768.0f * 180.0f; // °
                    wit_data.pitch = (int16_t)((data[packCnt * 11 + 5] << 8) | 
                                              data[packCnt * 11 + 4]) / 32768.0f * 180.0f; // °
                    wit_data.yaw = (int16_t)((data[packCnt * 11 + 7] << 8) | 
                                             data[packCnt * 11 + 6]) / 32768.0f * 180.0f;
                    wit_data.version = (int16_t)((data[packCnt * 11 + 9] << 8) | 
                                                data[packCnt * 11 + 8]);
                }
            }
            rxSize -= 11;
            packCnt++;
        }
}

