#ifndef _BIT_ARRAY_H_
#define _BIT_ARRAY_H_

#include <stdint.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

// #define BIT_ARRAY_CONFIG_64

// 如果你的系统是 64 位的，这里可以改为 uint64_t
#ifdef BIT_ARRAY_CONFIG_64
typedef uint64_t bit_array_t;
#define BIT_ARRAY_BIT(n) (1ULL << (n))
#else
typedef uint32_t bit_array_t;
#define BIT_ARRAY_BIT(n) (1UL << (n))
#endif
typedef bit_array_t bit_array_val_t;

#define BIT_ARRAY_BITS (sizeof(bit_array_val_t) * 8) /* 每个 bit_array_val_t 的位数 */

#define BIT_ARRAY_BIT_WORD(bit)  ((bit) / BIT_ARRAY_BITS)      /* 计算位所在的字索引 */
#define BIT_ARRAY_BIT_INDEX(bit) ((bit_array_val_t)(bit) & (BIT_ARRAY_BITS - 1U)) /* 计算位在字内的索引 */

#define BIT_ARRAY_MASK(bit)       BIT_ARRAY_BIT(BIT_ARRAY_BIT_INDEX(bit)) /* 获取位的掩码 */
#define BIT_ARRAY_ELEM(addr, bit) ((addr)[BIT_ARRAY_BIT_WORD(bit)])      /* 获取位所在的元素 */

// 全为 1 的字
#define BIT_ARRAY_WORD_MAX (~(bit_array_val_t)0)

#define BIT_ARRAY_SUB_MASK(nbits) ((nbits) ? BIT_ARRAY_WORD_MAX >> (BIT_ARRAY_BITS - (nbits)) : (bit_array_val_t)0) /* 获取 nbits 的子掩码 */

// 一种可能更快的方式来合并两个带有掩码的字
// #define bitmask_merge(a,b,abits) ((a & abits) | (b & ~abits))
#define bitmask_merge(a, b, abits) (b ^ ((a ^ b) & abits)) /* 使用掩码合并两个字 */

/**
 * @brief 这个宏计算表示 @a num_bits 位图所需的 bit array 变量数量。
 *
 * @param num_bits 位的数量。
 */
#define BIT_ARRAY_BITMAP_SIZE(num_bits) (1 + ((num_bits)-1) / BIT_ARRAY_BITS)

/**
 * @brief 定义一个 bit array 变量数组。
 *
 * 这个宏定义了一个包含至少 @a num_bits 位的 bit array 变量数组。
 *
 * @note
 * 如果在文件作用域使用，数组的位初始化为零；
 * 如果在函数内部使用，位保持未初始化。
 *
 * @cond INTERNAL_HIDDEN
 * @note
 * 这个宏应该在文档 Doxyfile 的 PREDEFINED 字段中复制。
 * @endcond
 *
 * @param name bit array 变量数组的名称。
 * @param num_bits 所需的位数。
 */
#define BIT_ARRAY_DEFINE(name, num_bits) bit_array_t name[BIT_ARRAY_BITMAP_SIZE(num_bits)]

#if 1
// 参见 http://graphics.stanford.edu/~seander/bithacks.html#CountBitsSetParallel
static inline bit_array_val_t _windows_popcount(bit_array_val_t w) /* 计算字中置位的数量 (Hamming weight) */
{
    w = w - ((w >> 1) & (bit_array_val_t) ~(bit_array_val_t)0 / 3);
    w = (w & (bit_array_val_t) ~(bit_array_val_t)0 / 15 * 3) + ((w >> 2) & (bit_array_val_t) ~(bit_array_val_t)0 / 15 * 3);
    w = (w + (w >> 4)) & (bit_array_val_t) ~(bit_array_val_t)0 / 255 * 15;
    return (bit_array_val_t)(w * ((bit_array_val_t) ~(bit_array_val_t)0 / 255)) >> (sizeof(bit_array_val_t) - 1) * 8;
}

#define POPCOUNT(x) _windows_popcount(x)
#else
#define POPCOUNT(x) (unsigned)__builtin_popcountll(x) /* 使用 GCC 内建函数计算置位数 */
#endif

#define bits_in_top_word(nbits) ((nbits) ? BIT_ARRAY_BIT_INDEX((nbits)-1) + 1 : 0) /* 最高有效字中的位数 */

static inline void _bit_array_mask_top_word(bit_array_t *target, int num_bits) /* 屏蔽最高有效字中未使用的位 */
{
    // 屏蔽最高有效字
    int num_of_words = BIT_ARRAY_BITMAP_SIZE(num_bits); /* 总字数 */
    int bits_active = bits_in_top_word(num_bits); /* 最高有效字中的有效位数 */
    target[num_of_words - 1] &= BIT_ARRAY_SUB_MASK(bits_active); /* 应用掩码 */
}

/**
 * @brief 测试 Bit Array 中的一个位。
 *
 * 这个例程测试 @a target 的位号 @a bit 是否被设置。
 *
 * @param target bit array 变量或数组的地址。
 * @param bit 位号（从 0 开始）。
 *
 * @return 如果位被设置则返回 true，否则返回 false。
 */
static inline int bit_array_get(const bit_array_t *target, int bit)
{
    bit_array_val_t val = BIT_ARRAY_ELEM(target, bit); /* 获取包含该位的字 */

    return (1 & (val >> (bit & (BIT_ARRAY_BITS - 1)))) != 0; /* 检查该位是否为 1 */
}

/**
 * @brief 清除 Bit Array 中的一个位。
 *
 * 清除 @a target 的位号 @a bit。
 *
 * @param target bit array 变量或数组的地址。
 * @param bit 位号（从 0 开始）。
 */
static inline void bit_array_clear(bit_array_t *target, int bit)
{
    bit_array_val_t mask = BIT_ARRAY_MASK(bit); /* 获取该位的掩码 */

    BIT_ARRAY_ELEM(target, bit) &= ~mask; /* 清除该位 */
}

/**
 * @brief 设置 Bit Array 中的一个位。
 *
 * 设置 @a target 的位号 @a bit。
 *
 * @param target bit array 变量或数组的地址。
 * @param bit 位号（从 0 开始）。
 */
static inline void bit_array_set(bit_array_t *target, int bit)
{
    bit_array_val_t mask = BIT_ARRAY_MASK(bit); /* 获取该位的掩码 */

    BIT_ARRAY_ELEM(target, bit) |= mask; /* 设置该位 */
}

/**
 * @brief 切换 Bit Array 中的一个位。
 *
 * 切换 @a target 的位号 @a bit。
 *
 * @param target bit array 变量或数组的地址。
 * @param bit 位号（从 0 开始）。
 */
static inline void bit_array_toggle(bit_array_t *target, int bit)
{
    bit_array_val_t mask = BIT_ARRAY_MASK(bit); /* 获取该位的掩码 */

    BIT_ARRAY_ELEM(target, bit) ^= mask; /* 切换该位 */
}

/**
 * @brief 将 Bit Array 中的一个位设置为给定值。
 *
 * 将 @a target 的位号 @a bit 设置为值 @a val。
 *
 * @param target bit array 变量或数组的地址。
 * @param bit 位号（从 0 开始）。
 * @param val true 表示 1，false 表示 0。
 */
static inline void bit_array_assign(bit_array_t *target, int bit, int val)
{
    bit_array_val_t mask = BIT_ARRAY_MASK(bit); /* 获取该位的掩码 */

    if (val)
    {
        BIT_ARRAY_ELEM(target, bit) |= mask; /* 设置为 1 */
    }
    else
    {
        BIT_ARRAY_ELEM(target, bit) &= ~mask; /* 设置为 0 */
    }
}

/* 清除所有位 */
static inline void bit_array_clear_all(bit_array_t *target, int num_bits)
{
    memset((void *)target, 0, BIT_ARRAY_BITMAP_SIZE(num_bits) * sizeof(bit_array_val_t));
}

/* 设置所有位 */
static inline void bit_array_set_all(bit_array_t *target, int num_bits)
{
    memset((void *)target, 0xff, BIT_ARRAY_BITMAP_SIZE(num_bits) * sizeof(bit_array_val_t));
    _bit_array_mask_top_word(target, num_bits); /* 屏蔽最高有效字中未使用的位 */
}

/* 切换所有位 */
static inline void bit_array_toggle_all(bit_array_t *target, int num_bits)
{
    for (int i = 0; i < BIT_ARRAY_BITMAP_SIZE(num_bits); i++)
    {
        target[i] ^= BIT_ARRAY_WORD_MAX;
    }
    _bit_array_mask_top_word(target, num_bits); /* 屏蔽最高有效字中未使用的位 */
}

//
// 字符串和打印
//

// 从给定的开/关字符的子字符串构造 BIT_ARRAY。

// 从字符串方法
static inline void bit_array_from_str(bit_array_t *bitarr, const char *str) /* 从字符串转换 */
{
    int i, index;
    int space = 0;
    int len = strlen(str);

    for (i = 0; i < len; i++)
    {
        index = i - space;
        if (strchr("1", str[i]) != NULL)
        {
            bit_array_set(bitarr, index);
        }
        else if (strchr("0", str[i]) != NULL)
        {
            bit_array_clear(bitarr, index);
        }
        else
        {
            // 错误。
            space++;
        }
    }
}

// 接受一个 char 数组用于写入。`str` 的长度必须为 bitarr->num_of_bits+1
// 用 '\0' 终止字符串
static inline char *bit_array_to_str(const bit_array_t *bitarr, int num_bits, char *str) /* 转换为字符串 */
{
    int i;

    for (i = 0; i < num_bits; i++)
    {
        str[i] = bit_array_get(bitarr, i) ? '1' : '0';
    }

    str[num_bits] = '\0'; /* 添加字符串结束符 */

    return str;
}

// 接受一个 char 数组用于写入。`str` 的长度必须为 bitarr->num_of_bits + (num_bits/8) + 1
// 用 '\0' 终止字符串，每 8 位添加一个空格
static inline char *bit_array_to_str_8(const bit_array_t *bitarr, int num_bits, char *str) /* 转换为每 8 位带空格的字符串 */
{
    int i;
    int space = 0;

    for (i = 0; i < num_bits; i++)
    {
        str[i + space] = bit_array_get(bitarr, i) ? '1' : '0';

        if ((i + 1) % 8 == 0 && (i+1) != num_bits) /* 每 8 位且不是最后一位时添加空格 */
        {
            space++;
            str[i + space] = ' ';
        }
    }

    str[num_bits + space] = '\0'; /* 添加字符串结束符 */

    return str;
}

//
// 获取和设置字（仅限内部使用 -- 无边界检查）
//

/* 获取从指定起始位置开始的一个字 */
static inline bit_array_val_t _bit_array_get_word(const bit_array_t *target, int num_bits, int start)
{
    int word_index = BIT_ARRAY_BIT_WORD(start); /* 字索引 */
    int word_offset = BIT_ARRAY_BIT_INDEX(start); /* 字内偏移 */

    bit_array_val_t result = target[word_index] >> word_offset; /* 获取当前字的部分 */

    int bits_taken = BIT_ARRAY_BITS - word_offset; /* 已获取的位数 */

    // word_offset 现在是我们需要从下一个字获取的位数
    // 检查下一个字是否至少有一些位
    if (word_offset > 0 && start + bits_taken < num_bits)
    {
        result |= target[word_index + 1] << (BIT_ARRAY_BITS - word_offset); /* 从下一个字获取剩余位 */
    }

    return result;
}

// 从特定的起始位置设置 64 位
// 不扩展 bit array
/* 设置从指定起始位置开始的一个字 */
static inline void _bit_array_set_word(bit_array_t *target, int num_bits, int start, bit_array_val_t word)
{
    int word_index = BIT_ARRAY_BIT_WORD(start); /* 字索引 */
    int word_offset = BIT_ARRAY_BIT_INDEX(start); /* 字内偏移 */

    if (word_offset == 0) /* 如果偏移为 0，直接设置整个字 */
    {
        target[word_index] = word;
    }
    else /* 如果有偏移 */
    {
        /* 设置当前字的部分 */
        target[word_index] = (word << word_offset) | (target[word_index] & BIT_ARRAY_SUB_MASK(word_offset));

        /* 如果下一个字在范围内，设置下一个字的部分 */
        if (word_index + 1 < BIT_ARRAY_BITMAP_SIZE(num_bits))
        {
            target[word_index + 1] = (word >> (BIT_ARRAY_BITS - word_offset)) | (target[word_index + 1] & (BIT_ARRAY_WORD_MAX << word_offset));
        }
    }

    // 屏蔽最高有效字
    _bit_array_mask_top_word(target, num_bits);
}

//
// 填充区域（仅限内部使用）
//

// 填充操作：用 0、1 或切换填充
typedef enum
{
    ZERO_REGION, /* 用 0 填充 */
    FILL_REGION, /* 用 1 填充 */
    SWAP_REGION  /* 切换区域 */
} FillAction;

/* 设置区域的值 */
static inline void _bit_array_set_region(bit_array_t *target, int start, int length, FillAction action)
{
    if (length == 0)
        return;

    int first_word = BIT_ARRAY_BIT_WORD(start); /* 起始字索引 */
    int last_word = BIT_ARRAY_BIT_WORD(start + length - 1); /* 结束字索引 */
    int foffset = BIT_ARRAY_BIT_INDEX(start); /* 起始字内偏移 */
    int loffset = BIT_ARRAY_BIT_INDEX(start + length - 1); /* 结束字内偏移 */

    if (first_word == last_word) /* 如果在同一个字内 */
    {
        bit_array_val_t mask = BIT_ARRAY_SUB_MASK(length) << foffset; /* 计算掩码 */

        switch (action)
        {
        case ZERO_REGION:
            target[first_word] &= ~mask;
            break;
        case FILL_REGION:
            target[first_word] |= mask;
            break;
        case SWAP_REGION:
            target[first_word] ^= mask;
            break;
        }
    }
    else /* 如果跨越多个字 */
    {
        // 设置第一个字
        switch (action)
        {
        case ZERO_REGION:
            target[first_word] &= BIT_ARRAY_SUB_MASK(foffset);
            break;
        case FILL_REGION:
            target[first_word] |= ~BIT_ARRAY_SUB_MASK(foffset);
            break;
        case SWAP_REGION:
            target[first_word] ^= ~BIT_ARRAY_SUB_MASK(foffset);
            break;
        }

        int i;

        // 设置中间的完整字
        switch (action)
        {
        case ZERO_REGION:
            for (i = first_word + 1; i < last_word; i++)
                target[i] = (bit_array_val_t)0;
            break;
        case FILL_REGION:
            for (i = first_word + 1; i < last_word; i++)
                target[i] = BIT_ARRAY_WORD_MAX;
            break;
        case SWAP_REGION:
            for (i = first_word + 1; i < last_word; i++)
                target[i] ^= BIT_ARRAY_WORD_MAX;
            break;
        }

        // 设置最后一个字
        switch (action)
        {
        case ZERO_REGION:
            target[last_word] &= ~BIT_ARRAY_SUB_MASK(loffset + 1);
            break;
        case FILL_REGION:
            target[last_word] |= BIT_ARRAY_SUB_MASK(loffset + 1);
            break;
        case SWAP_REGION:
            target[last_word] ^= BIT_ARRAY_SUB_MASK(loffset + 1);
            break;
        }
    }
}

// 获取设置的位数（汉明权重）
static inline int bit_array_num_bits_set(bit_array_t *target, int num_bits)
{
    int i;

    int num_of_bits_set = 0;

    for (i = 0; i < BIT_ARRAY_BITMAP_SIZE(num_bits); i++)
    {
        if (target[i] > 0) /* 优化：如果字为 0，则跳过计算 */
        {
            num_of_bits_set += POPCOUNT(target[i]);
        }
    }

    return num_of_bits_set;
}

// 获取未设置的位数（1 - 汉明权重）
static inline int bit_array_num_bits_cleared(bit_array_t *target, int num_bits)
{
    return num_bits - bit_array_num_bits_set(target, num_bits);
}

// 将位从一个数组复制到另一个数组
// 注意：使用宏 bit_array_copy
// 目标和源可以是同一个 bit_array，并且
// src/dst 区域可以重叠
static inline void bit_array_copy(bit_array_t *dst, int dstindx, const bit_array_t *src, int srcindx, int length, int src_num_bits, int dst_num_bits)
{
    // 要复制的完整字数
    int num_of_full_words = length / BIT_ARRAY_BITS;
    int i;

    int bits_in_last_word = bits_in_top_word(length); /* 最后一个字中的位数 */

    if (dst == src && srcindx > dstindx) /* 如果源和目标相同且源索引大于目标索引 */
    {
        // 从左到右工作
        for (i = 0; i < num_of_full_words; i++)
        {
            bit_array_val_t word = _bit_array_get_word(src, src_num_bits, srcindx + i * BIT_ARRAY_BITS);
            _bit_array_set_word(dst, dst_num_bits, dstindx + i * BIT_ARRAY_BITS, word);
        }

        if (bits_in_last_word > 0) /* 处理最后一个不完整的字 */
        {
            bit_array_val_t src_word = _bit_array_get_word(src, src_num_bits, srcindx + i * BIT_ARRAY_BITS);
            bit_array_val_t dst_word = _bit_array_get_word(dst, dst_num_bits, dstindx + i * BIT_ARRAY_BITS);

            bit_array_val_t mask = BIT_ARRAY_SUB_MASK(bits_in_last_word);
            bit_array_val_t word = bitmask_merge(src_word, dst_word, mask); /* 合并源字和目标字 */

            _bit_array_set_word(dst, dst_num_bits, dstindx + num_of_full_words * BIT_ARRAY_BITS, word);
        }
    }
    else /* 其他情况（包括源和目标不同，或源索引小于等于目标索引）*/
    {
        // 从右到左工作
        // 先处理最后一个不完整的字
        if (bits_in_last_word > 0)
        {
            bit_array_val_t src_word = _bit_array_get_word(src, src_num_bits, srcindx + num_of_full_words * BIT_ARRAY_BITS);
            bit_array_val_t dst_word = _bit_array_get_word(dst, dst_num_bits, dstindx + num_of_full_words * BIT_ARRAY_BITS);

            bit_array_val_t mask = BIT_ARRAY_SUB_MASK(bits_in_last_word);
            bit_array_val_t word = bitmask_merge(src_word, dst_word, mask); /* 合并源字和目标字 */
            _bit_array_set_word(dst, dst_num_bits, dstindx + num_of_full_words * BIT_ARRAY_BITS, word);
        }
        // 处理完整的字
        for (i = num_of_full_words - 1; i >= 0; i--) // 从右到左复制完整字
        {
            bit_array_val_t word = _bit_array_get_word(src, src_num_bits, srcindx + i * BIT_ARRAY_BITS);
            _bit_array_set_word(dst, dst_num_bits, dstindx + i * BIT_ARRAY_BITS, word);
        }
    }

    _bit_array_mask_top_word(dst, dst_num_bits); /* 屏蔽目标数组最高有效字中未使用的位 */
}

// 将 src 的所有位复制到 dst。dst 的大小调整为与 src 匹配。
static inline void bit_array_copy_all(bit_array_t *dst, const bit_array_t *src, int num_bits)
{
    memcpy(dst, src, BIT_ARRAY_BITMAP_SIZE(num_bits) * sizeof(bit_array_val_t)); /* 使用 memcpy 提高效率 */
    // for (int i = 0; i < BIT_ARRAY_BITMAP_SIZE(num_bits); i++)
    // {
    //     dst[i] = src[i];
    // }
}

//
// 逻辑运算符
//

// 目标可以与一个或两个源相同
/* 按位与 */
static inline void bit_array_and(bit_array_t *dest, const bit_array_t *src1, const bit_array_t *src2, int num_bits)
{
    for (int i = 0; i < BIT_ARRAY_BITMAP_SIZE(num_bits); i++)
    {
        dest[i] = src1[i] & src2[i];
    }
}

/* 按位或 */
static inline void bit_array_or(bit_array_t *dest, const bit_array_t *src1, const bit_array_t *src2, int num_bits)
{
    for (int i = 0; i < BIT_ARRAY_BITMAP_SIZE(num_bits); i++)
    {
        dest[i] = src1[i] | src2[i];
    }
}

/* 按位异或 */
static inline void bit_array_xor(bit_array_t *dest, const bit_array_t *src1, const bit_array_t *src2, int num_bits)
{
    for (int i = 0; i < BIT_ARRAY_BITMAP_SIZE(num_bits); i++)
    {
        dest[i] = src1[i] ^ src2[i];
    }
}

/* 按位取反 */
static inline void bit_array_not(bit_array_t *dest, const bit_array_t *src, int num_bits)
{
    for (int i = 0; i < BIT_ARRAY_BITMAP_SIZE(num_bits); i++)
    {
        dest[i] = ~src[i];
    }
    _bit_array_mask_top_word(dest, num_bits); /* 取反后需要屏蔽最高有效字 */
}

//
// 数组左移/右移。如果 fill 为零，则填充 0，否则填充 1
//

// 向 LSB / 低索引移位
static inline void bit_array_shift_right(bit_array_t *target, int num_bits, int shift_dist, int fill)
{
    if (shift_dist <= 0) return; /* 无需移位 */

    if (shift_dist >= num_bits)
    {
        fill ? bit_array_set_all(target, num_bits) : bit_array_clear_all(target, num_bits); /* 移位距离大于等于总位数，直接全部填充 */
        return;
    }

    FillAction action = fill ? FILL_REGION : ZERO_REGION; /* 确定填充操作 */

    int cpy_length = num_bits - shift_dist; /* 需要复制的位数 */
    // 注意：这里直接调用内部函数 _bit_array_copy 可能更高效，但为了接口一致性，使用 bit_array_copy
    // 优化：直接内存移动，避免逐位操作
    memmove(target, (uint8_t*)target + shift_dist / 8, (cpy_length + 7) / 8);
    // bit_array_copy(target, 0, target, shift_dist, cpy_length, num_bits, num_bits);

    _bit_array_set_region(target, cpy_length, shift_dist, action); /* 填充移位后空出的位 */
    _bit_array_mask_top_word(target, num_bits); /* 确保最高位被正确屏蔽 */
}

// 向 MSB / 高索引移位
static inline void bit_array_shift_left(bit_array_t *target, int num_bits, int shift_dist, int fill)
{
    if (shift_dist <= 0) return; /* 无需移位 */

    if (shift_dist >= num_bits)
    {
        fill ? bit_array_set_all(target, num_bits) : bit_array_clear_all(target, num_bits); /* 移位距离大于等于总位数，直接全部填充 */
        return;
    }

    FillAction action = fill ? FILL_REGION : ZERO_REGION; /* 确定填充操作 */

    int cpy_length = num_bits - shift_dist; /* 需要复制的位数 */
    // 注意：这里直接调用内部函数 _bit_array_copy 可能更高效，但为了接口一致性，使用 bit_array_copy
    // 优化：直接内存移动，避免逐位操作
    memmove((uint8_t*)target + shift_dist / 8, target, (cpy_length + 7) / 8);
    // bit_array_copy(target, shift_dist, target, 0, cpy_length, num_bits, num_bits);

    _bit_array_set_region(target, 0, shift_dist, action); /* 填充移位后空出的位 */
    _bit_array_mask_top_word(target, num_bits); /* 确保最高位被正确屏蔽 */
}

//
// 比较
//

// 按存储的值比较两个 bit array，索引 0 为最低有效位 (LSB)。数组必须具有相同的长度。
// 返回值：
// >0 iff bitarr1 > bitarr2
//  0 iff bitarr1 == bitarr2
// <0 iff bitarr1 < bitarr2
static inline int bit_array_cmp(const bit_array_t *bitarr1, const bit_array_t *bitarr2, int num_bits)
{
    // 从最高有效字开始比较，因为 memcmp 是按字节比较
    int result = memcmp(bitarr1, bitarr2, BIT_ARRAY_BITMAP_SIZE(num_bits) * sizeof(bit_array_val_t));
    // 对于小端系统，memcmp 的结果与数值比较结果一致。
    // 对于大端系统，需要反转比较方向或逐字比较。
    // 假设是小端系统，memcmp 直接可用。
    return result;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _BIT_ARRAY_H_ */
