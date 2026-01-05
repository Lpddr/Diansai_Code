#include "flash_app.h"
Coordinate_t test_flash;

/**
 * @brief  写入坐标到Flash
 * @param  index: 坐标索引 (0 ~ MAX_COORDINATE_COUNT-1)
 * @param  x: X坐标值
 * @param  y: Y坐标值
 * @retval 0: 成功, 1: 失败(索引超出范围)
 */
uint8_t W25QXX_WriteCoordinate(uint16_t index, uint16_t x, uint16_t y)
{
    uint32_t addr;
    uint8_t data[COORDINATE_SIZE];
    
    // 检查索引范围
    if(index >= MAX_COORDINATE_COUNT)
    {
        return 1;  // 索引超出范围
    }
    
    // 计算存储地址
    addr = COORDINATE_BASE_ADDR + (index * COORDINATE_SIZE);
    
    // 将坐标数据打包到缓冲区(小端序)
    data[0] = (uint8_t)(x & 0xFF);         // X坐标低字节
    data[1] = (uint8_t)((x >> 8) & 0xFF);  // X坐标高字节
    data[2] = (uint8_t)(y & 0xFF);         // Y坐标低字节
    data[3] = (uint8_t)((y >> 8) & 0xFF);  // Y坐标高字节
    
    // 写入Flash
    W25QXX_Write(data, addr, COORDINATE_SIZE);
    
    return 0;  // 成功
}

/**
 * @brief  从Flash读取坐标
 * @param  index: 坐标索引 (0 ~ MAX_COORDINATE_COUNT-1)
 * @param  x: 指向X坐标的指针
 * @param  y: 指向Y坐标的指针
 * @retval 0: 成功, 1: 失败(索引超出范围或指针为空)
 */
uint8_t W25QXX_ReadCoordinate(uint16_t index, uint16_t *x, uint16_t *y)
{
    uint32_t addr;
    uint8_t data[COORDINATE_SIZE];
    
    // 检查参数
    if(index >= MAX_COORDINATE_COUNT || x == NULL || y == NULL)
    {
        return 1;  // 参数错误
    }
    
    // 计算存储地址
    addr = COORDINATE_BASE_ADDR + (index * COORDINATE_SIZE);
    
    // 从Flash读取数据
    W25QXX_Read(data, addr, COORDINATE_SIZE);
    
    // 解析坐标数据(小端序)
    *x = (uint16_t)(data[0] | (data[1] << 8));  // 重构X坐标
    *y = (uint16_t)(data[2] | (data[3] << 8));  // 重构Y坐标
    
    return 0;  // 成功
}

/**
 * @brief  擦除坐标存储区域
 * @param  无
 * @retval 0: 成功
 * @note   这会清除所有存储的坐标，谨慎使用
 */
uint8_t W25QXX_EraseCoordinateArea(void)
{
    uint32_t sector_num;
    
    // 计算起始扇区号 (每个扇区4096字节)
    sector_num = COORDINATE_BASE_ADDR / 4096;
    
    // 擦除扇区
    W25QXX_Erase_Sector(sector_num);
    
    return 0;  // 成功
}
