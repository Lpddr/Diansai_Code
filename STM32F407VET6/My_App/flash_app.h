#ifndef FLASH_APP_H
#define FLASH_APP_H

#include "bsp_system.h"

// 坐标数据结构
typedef struct {
    uint16_t x;
    uint16_t y;
} Coordinate_t;

// 坐标存储基地址(从第1个扇区开始，地址0x1000)
#define COORDINATE_BASE_ADDR    0x1000

// 每个坐标占用的字节数(4字节: X(2) + Y(2))
#define COORDINATE_SIZE         4

// 最大可存储的坐标数量(一个扇区4096字节，可存储1024个坐标)
#define MAX_COORDINATE_COUNT    1024

// 函数声明
uint8_t W25QXX_WriteCoordinate(uint16_t index, uint16_t x, uint16_t y);
uint8_t W25QXX_ReadCoordinate(uint16_t index, uint16_t *x, uint16_t *y);
uint8_t W25QXX_EraseCoordinateArea(void);
extern Coordinate_t test_flash;
#endif
