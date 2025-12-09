#ifndef _EBTN_H
#define _EBTN_H

#include <stdint.h>
#include <string.h>

#include "bit_array.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

// #define EBTN_CONFIG_TIMER_16

// 如果你想减少 RAM 大小，这里可以改为 uint16_t
#ifdef EBTN_CONFIG_TIMER_16
typedef uint16_t ebtn_time_t;
typedef int16_t ebtn_time_sign_t;
#else
typedef uint32_t ebtn_time_t;
typedef int32_t ebtn_time_sign_t;
#endif

/* 前向声明 */
struct ebtn_btn;
struct ebtn;

#define EBTN_MAX_KEYNUM (64) /* 最大支持的按键数量 (包括独立按键和组合按键) */

#define EBTN_DEFAULT_COMBO_SUPPRESS_THRESHOLD 500 /* 默认组合键抑制单击阈值 (ms) */

/**
 * \brief           按钮事件列表
 *
 */
typedef enum
{
    EBTN_EVT_ONPRESS = 0x00, /*!< 按下事件 - 检测到有效按下时发送 */
    EBTN_EVT_ONRELEASE,      /*!< 释放事件 - 检测到有效释放事件时发送 (从活动到非活动) */
    EBTN_EVT_ONCLICK,        /*!< 单击事件 - 发生有效的按下和释放事件序列时发送 */
    EBTN_EVT_KEEPALIVE,      /*!< 保持活动事件 - 按钮处于活动状态时定期发送 */
} ebtn_evt_t;

/* 事件掩码 */
#define EBTN_EVT_MASK_ONPRESS   (1 << EBTN_EVT_ONPRESS)   /* 按下事件掩码 */
#define EBTN_EVT_MASK_ONRELEASE (1 << EBTN_EVT_ONRELEASE) /* 释放事件掩码 */
#define EBTN_EVT_MASK_ONCLICK   (1 << EBTN_EVT_ONCLICK)   /* 单击事件掩码 */
#define EBTN_EVT_MASK_KEEPALIVE (1 << EBTN_EVT_KEEPALIVE) /* 保持活动事件掩码 */

#define EBTN_EVT_MASK_ALL (EBTN_EVT_MASK_ONPRESS | EBTN_EVT_MASK_ONRELEASE | EBTN_EVT_MASK_ONCLICK | EBTN_EVT_MASK_KEEPALIVE) /* 所有事件掩码 */

// 定义组合键优先处理的标志
#define EBTN_CFG_COMBO_PRIORITY (1 << 0) /* 组合键优先处理模式 */

/**
 * @brief  返回两个绝对时间之间的差值: time1-time2。
 * @param[in]  time1: 以内部时间单位表示的绝对时间。
 * @param[in]  time2: 以内部时间单位表示的绝对时间。
 * @return 以内部时间单位表示的产生的带符号相对时间。
 */
static inline ebtn_time_sign_t ebtn_timer_sub(ebtn_time_t time1, ebtn_time_t time2)
{
    return time1 - time2;
}

// 测试时间溢出错误
// #define ebtn_timer_sub(time1, time2) (time1 - time2)

/**
 * \brief           按钮事件回调函数原型
 * \param[in]       btn: 发生事件的按钮实例（来自数组）
 * \param[in]       evt: 事件类型
 */
typedef void (*ebtn_evt_fn)(struct ebtn_btn *btn, ebtn_evt_t evt);

/**
 * \brief           获取按钮/输入状态的回调函数
 *
 * \param[in]       btn: 要读取状态的按钮实例（来自数组）
 * \return          当按钮被视为 `活动` 时返回 `1`，否则返回 `0`
 */
typedef uint8_t (*ebtn_get_state_fn)(struct ebtn_btn *btn);

/**
 * \brief           按钮参数结构体
 */
typedef struct ebtn_btn_param
{
    /**
     * \brief           按下事件的最小去抖时间，单位毫秒
     *
     *                  这是输入必须具有稳定活动电平以检测有效 *按下* 事件的时间。
     *
     *                  当值设置为 `> 0` 时，输入必须处于活动状态至少
     *                  最小毫秒时间，才能检测到有效的 *按下* 事件。
     *
     * \note            如果值设置为 `0`，则不使用去抖，*按下* 事件将在输入状态变为 *非活动* 状态时立即触发。
     *                  为安全起见，不使用此功能时，外部逻辑必须确保输入电平的稳定转换。
     *
     */
    uint16_t time_debounce; /*!< 按下去抖时间，单位毫秒 */

    /**
     * \brief           释放事件的最小去抖时间，单位毫秒
     *
     *                  这是输入必须具有最小稳定释放电平以检测有效 *释放* 事件的时间。
     *
     *                  如果应用程序想要防止输入被视为"活动"时线路上出现不必要的毛刺，则此设置可能很有用。
     *
     *                  当值设置为 `> 0` 时，输入必须处于非活动低电平至少
     *                  最小毫秒时间，才能检测到有效的 *释放* 事件。
     *
     * \note            如果值设置为 `0`，则不使用去抖，*释放* 事件将在输入状态变为 *非活动* 状态时立即触发。
     *
     */
    uint16_t time_debounce_release; /*!< 释放事件的去抖时间，单位毫秒 */

    /**
     * \brief           有效单击事件的最短活动输入时间，单位毫秒
     *
     *                  输入应处于活动状态（去抖后）至少此时间量，才能考虑潜在的有效单击事件。将值设置为 `0` 以禁用此功能。
     *
     */
    uint16_t time_click_pressed_min; /*!< 有效单击事件的最短按下时间 */

    /**
     * \brief           有效单击事件的最大活动输入时间，单位毫秒
     *
     *                  输入应被按下最多此时间量，才能仍然触发有效单击。
     *                  设置为 `-1`（即 0xFFFF）以允许任何时间触发单击事件。
     *
     *                  当输入活动时间超过配置的时间时，不检测单击事件并将其忽略。
     *
     */
    uint16_t time_click_pressed_max; /*!< 有效单击事件的最大按下时间 */

    /**
     * \brief           最后一次释放和下一次有效按下之间允许的最大时间，
     *                  以仍然允许多次单击事件，单位毫秒。
     *
     *                  此值也用作超时长度，以将先前检测到的有效单击事件的 *单击* 事件发送到应用程序。
     *
     *                  如果应用程序依赖于连续多次单击，则这是允许用户触发潜在新单击的最长时间，
     *                  否则结构将被重置（如果在目前为止检测到任何单击，则在发送给用户之前）。
     *
     */
    uint16_t time_click_multi_max; /*!< 被视为连续单击的两次单击之间的最大时间 */

    /**
     * \brief           保持活动事件周期，单位毫秒
     *
     *                  当输入处于活动状态时，将在此时间段内发送保持活动事件。
     *                  第一个保持活动将在输入被视为活动后发送。
     *
     */
    uint16_t time_keepalive_period; /*!< 周期性保持活动事件的时间，单位毫秒 */

    /**
     * \brief           允许的最大连续单击事件数，
     *                  在此之前结构将重置为默认值。
     *
     * \note            当达到连续值时，应用程序将收到单击通知。
     *                  这可以在检测到最后一次单击后立即执行，也可以在标准超时后执行
     *                  （除非已经检测到下一次按下，那么它会在有效的下一次按下事件之前发送给应用程序）。
     *
     */
    uint16_t max_consecutive; /*!< 最大连续点击次数 */
} ebtn_btn_param_t;

/* 参数初始化宏 */
#define EBTN_PARAMS_INIT(_time_debounce, _time_debounce_release, _time_click_pressed_min, _time_click_pressed_max, _time_click_multi_max,                      \
                         _time_keepalive_period, _max_consecutive)                                                                                             \
    {                                                                                                                                                          \
        .time_debounce = _time_debounce, .time_debounce_release = _time_debounce_release, .time_click_pressed_min = _time_click_pressed_min,                   \
        .time_click_pressed_max = _time_click_pressed_max, .time_click_multi_max = _time_click_multi_max, .time_keepalive_period = _time_keepalive_period,     \
        .max_consecutive = _max_consecutive                                                                                                                    \
    }

/* 原始按钮初始化宏（带事件掩码）*/
#define EBTN_BUTTON_INIT_RAW(_key_id, _param, _mask)                                                                                                           \
    {                                                                                                                                                          \
        .key_id = _key_id, .param = _param, .event_mask = _mask,                                                                                               \
    }

/* 标准按钮初始化宏（默认启用所有事件）*/
#define EBTN_BUTTON_INIT(_key_id, _param) EBTN_BUTTON_INIT_RAW(_key_id, _param, EBTN_EVT_MASK_ALL)

/* 动态按钮初始化宏 */
#define EBTN_BUTTON_DYN_INIT(_key_id, _param)                                                                                                                  \
    {                                                                                                                                                          \
        .next = NULL, .btn = EBTN_BUTTON_INIT(_key_id, _param),                                                                                                \
    }

/* 原始组合按钮初始化宏（带事件掩码）*/
#define EBTN_BUTTON_COMBO_INIT_RAW(_key_id, _param, _mask)                                                                                                     \
    {                                                                                                                                                          \
        .comb_key = {0}, .btn = EBTN_BUTTON_INIT_RAW(_key_id, _param, _mask),                                                                                  \
    }

/* 标准组合按钮初始化宏（默认启用所有事件）*/
#define EBTN_BUTTON_COMBO_INIT(_key_id, _param)                                                                                                                \
    {                                                                                                                                                          \
        .comb_key = {0}, .btn = EBTN_BUTTON_INIT(_key_id, _param),                                                                                             \
    }

/* 动态组合按钮初始化宏 */
#define EBTN_BUTTON_COMBO_DYN_INIT(_key_id, _param)                                                                                                            \
    {                                                                                                                                                          \
        .next = NULL, .btn = EBTN_BUTTON_COMBO_INIT(_key_id, _param),                                                                                          \
    }

/* 获取数组大小的宏 */
#define EBTN_ARRAY_SIZE(_arr) sizeof(_arr) / sizeof((_arr)[0])

/**
 * \brief           按钮结构体
 */
typedef struct ebtn_btn
{
    uint16_t key_id;    /*!< 用户定义的回调函数自定义参数 */
    uint8_t flags;      /*!< 私有按钮标志管理 */
    uint8_t event_mask; /*!< 私有按钮事件掩码管理 */

    ebtn_time_t time_change;       /*!< 按钮状态上次在有效去抖后改变的时间，单位毫秒 */
    ebtn_time_t time_state_change; /*!< 按钮状态上次改变的时间，单位毫秒 */

    ebtn_time_t keepalive_last_time; /*!< 上次发送保持活动事件的时间，单位毫秒 */
    ebtn_time_t click_last_time;     /*!< 上次成功检测到（未发送！）单击事件的时间，单位毫秒 */

    uint16_t keepalive_cnt; /*!< 成功检测到按下事件后发送的保持活动事件数。释放后重置为 0 */
    uint16_t click_cnt;     /*!< 检测到的连续单击次数，遵循单击之间的最大超时 */

    const ebtn_btn_param_t *param; /*!< 指向按钮参数的指针 */
} ebtn_btn_t;

/**
 * \brief           组合按钮结构体
 */
typedef struct ebtn_btn_combo
{
    BIT_ARRAY_DEFINE(comb_key, EBTN_MAX_KEYNUM); /*!< 选择键索引 - `1` 表示活动，`0` 表示非活动 */

    ebtn_btn_t btn; /*!< 基础按钮结构 */
} ebtn_btn_combo_t;

/**
 * \brief           动态按钮结构体
 */
typedef struct ebtn_btn_dyn
{
    struct ebtn_btn_dyn *next; /*!< 指向下一个按钮 */

    ebtn_btn_t btn; /*!< 基础按钮结构 */
} ebtn_btn_dyn_t;

/**
 * \brief           动态组合按钮结构体
 */
typedef struct ebtn_btn_combo_dyn
{
    struct ebtn_btn_combo_dyn *next; /*!< 指向下一个组合按钮 */

    ebtn_btn_combo_t btn; /*!< 组合按钮结构 */
} ebtn_btn_combo_dyn_t;

/**
 * \brief           按钮库主结构体
 */
typedef struct ebtn
{
    ebtn_btn_t *btns;             /*!< 指向静态按钮数组的指针 */
    uint16_t btns_cnt;            /*!< 静态按钮数组中的按钮数量 */
    ebtn_btn_combo_t *btns_combo; /*!< 指向静态组合按钮数组的指针 */
    uint16_t btns_combo_cnt;      /*!< 静态组合按钮数组中的按钮数量 */

    ebtn_btn_dyn_t *btn_dyn_head;             /*!< 指向动态按钮链表头 */
    ebtn_btn_combo_dyn_t *btn_combo_dyn_head; /*!< 指向动态组合按钮链表头 */

    ebtn_evt_fn evt_fn;             /*!< 指向事件回调函数的指针 */
    ebtn_get_state_fn get_state_fn; /*!< 指向获取状态回调函数的指针 */

    BIT_ARRAY_DEFINE(old_state, EBTN_MAX_KEYNUM); /*!< 旧按钮状态位数组 - `1` 表示活动，`0` 表示非活动 */
    BIT_ARRAY_DEFINE(combo_active, EBTN_MAX_KEYNUM); /*!< 活动组合键标记 - 用于防止单个按键事件 */
    
    uint8_t config; /*!< 配置标志位 */

    /* 新增：组合键冲突抑制相关 */
    uint16_t last_combo_event_id;       /*!< 最后触发单击事件的组合键ID */
    ebtn_time_t last_combo_event_time;    /*!< 最后触发组合键单击事件的时间戳 */
    ebtn_time_t combo_suppress_threshold; /*!< 组合键抑制普通按键单击的阈值 (ms) */
} ebtn_t;

/**
 * \brief           按钮处理函数，读取输入并相应地执行操作。
 *
 *
 * \param[in]       mstime: 当前系统时间，单位毫秒
 */
void ebtn_process(ebtn_time_t mstime);

/**
 * \brief           使用所有按钮的输入状态进行按钮处理的函数。
 *
 * \param[in]       curr_state: 当前所有按钮的输入状态位数组
 * \param[in]       mstime: 当前系统时间，单位毫秒
 */
void ebtn_process_with_curr_state(bit_array_t *curr_state, ebtn_time_t mstime);

/**
 * \brief           检查按钮是否处于活动状态。
 *                  活动状态被认为是在初始去抖期通过后。
 *                  这是按下事件和释放事件之间的时间段。
 *
 * \param[in]       btn: 要检查的按钮句柄
 * \return          如果处于活动状态则返回 `1`，否则返回 `0`
 */
int ebtn_is_btn_active(const ebtn_btn_t *btn);

/**
 * \brief           检查按钮是否正在处理中。
 *                  用于低功耗处理，指示按钮暂时空闲，嵌入式系统可以考虑进入深度睡眠。
 *
 * \param[in]       btn: 要检查的按钮句柄
 * \return          如果正在处理中则返回 `1`，否则返回 `0`
 */
int ebtn_is_btn_in_process(const ebtn_btn_t *btn);

/**
 * \brief           检查是否有任何按钮正在处理中。
 *                  用于低功耗处理，指示按钮暂时空闲，嵌入式系统可以考虑进入深度睡眠。
 *
 * \return          如果正在处理中则返回 `1`，否则返回 `0`
 */
int ebtn_is_in_process(void);

/**
 * \brief           初始化按钮管理器
 * \param[in]       btns: 要处理的静态按钮数组
 * \param[in]       btns_cnt: 要处理的静态按钮数量
 * \param[in]       btns_combo: 要处理的静态组合按钮数组
 * \param[in]       btns_combo_cnt: 要处理的静态组合按钮数量
 * \param[in]       get_state_fn: 指向按需提供按钮状态的函数的指针。
 * \param[in]       evt_fn: 按钮事件回调函数
 *
 * \return          成功返回 `1`，否则返回 `0`
 */
int ebtn_init(ebtn_btn_t *btns, uint16_t btns_cnt, ebtn_btn_combo_t *btns_combo, uint16_t btns_combo_cnt, ebtn_get_state_fn get_state_fn, ebtn_evt_fn evt_fn);

/**
 * @brief 注册一个动态按钮
 *
 * @param button: 动态按钮结构实例
 * \return          成功返回 `1`，否则返回 `0`
 */
int ebtn_register(ebtn_btn_dyn_t *button);

/**
 * \brief           注册一个动态组合按钮
 * \param[in]       button: 动态组合按钮结构实例
 *
 * \return          成功返回 `1`，否则返回 `0`
 */
int ebtn_combo_register(ebtn_btn_combo_dyn_t *button);

/**
 * \brief           获取当前总按钮数（静态+动态）
 *
 * \return          按钮总数。
 */
int ebtn_get_total_btn_cnt(void);

/**
 * \brief           根据 key_id 获取内部 key_idx
 * \param[in]       key_id: 按键 ID
 *
 * \return          错误返回 '-1'，否则返回 key_idx
 */
int ebtn_get_btn_index_by_key_id(uint16_t key_id);

/**
 * \brief           根据 key_id 获取按钮实例指针，包括静态和动态注册的按钮。
 *
 * \param[in]       key_id: 按键 ID
 *
 * \return          错误返回 'NULL'，否则返回按钮实例指针
 */
ebtn_btn_t *ebtn_get_btn_by_key_id(uint16_t key_id);

/**
 * \brief           根据按钮实例指针获取内部 key_idx
 * \param[in]       btn: 按钮实例指针
 *
 * \return          错误返回 '-1'，否则返回 key_idx
 */
int ebtn_get_btn_index_by_btn(ebtn_btn_t *btn);

/**
 * \brief           根据动态按钮实例指针获取内部 key_idx
 * \param[in]       btn: 动态按钮实例指针
 *
 * \return          错误返回 '-1'，否则返回 key_idx
 */
int ebtn_get_btn_index_by_btn_dyn(ebtn_btn_dyn_t *btn);

/**
 * \brief           通过 key_idx 绑定组合按钮键
 * \param[in,out]   btn: 组合按钮实例
 * \param[in]       idx: 按键索引 (key_idx)
 *
 */
void ebtn_combo_btn_add_btn_by_idx(ebtn_btn_combo_t *btn, int idx);

/**
 * \brief           通过 key_idx 移除组合按钮键
 * \param[in,out]   btn: 组合按钮实例
 * \param[in]       idx: 按键索引 (key_idx)
 *
 */
void ebtn_combo_btn_remove_btn_by_idx(ebtn_btn_combo_t *btn, int idx);

/**
 * \brief           通过 key_id 绑定组合按钮键，确保 key_id (按钮) 已注册。
 * \param[in,out]   btn: 组合按钮实例
 * \param[in]       key_id: 按键 ID
 *
 */
void ebtn_combo_btn_add_btn(ebtn_btn_combo_t *btn, uint16_t key_id);

/**
 * \brief           通过 key_id 移除组合按钮键，确保 key_id (按钮) 已注册。
 * \param[in,out]   btn: 组合按钮实例
 * \param[in]       key_id: 按键 ID
 *
 */
void ebtn_combo_btn_remove_btn(ebtn_btn_combo_t *btn, uint16_t key_id);

/**
 * \brief           获取特定按钮的保持活动周期
 * \param[in]       btn: 要获取保持活动周期的按钮实例
 * \return          保持活动周期，单位 `ms`
 */
#define ebtn_keepalive_get_period(btn) ((btn)->time_keepalive_period)

/**
 * \brief           获取自上次按下事件以来的实际保持活动计数。
 *                  如果按钮未按下，则设置为 `0`
 * \param[in]       btn: 要获取保持活动周期的按钮实例
 * \return          自按下事件以来的保持活动事件数
 */
#define ebtn_keepalive_get_count(btn) ((btn)->keepalive_cnt)

/**
 * \brief           获取特定所需时间（毫秒）的保持活动计数。
 *                  它将计算特定按钮应产生的保持活动滴答数，
 *                  直到达到请求的时间。
 *
 *                  函数的结果可与 \ref ebtn_keepalive_get_count 一起使用，后者返回
 *                  自按钮上次按下事件以来的实际保持活动计数。
 *
 * \note            值始终是整数对齐的，粒度为一个保持活动时间周期
 * \note            实现为宏，因为当使用静态保持活动时，编译器可能会对其进行优化
 *
 * \param[in]       btn: 用于检查的按钮
 * \param[in]       ms_time: 要计算保持活动计数的时间，单位毫秒
 * \return          保持活动计数
 */
#define ebtn_keepalive_get_count_for_time(btn, ms_time) ((ms_time) / ebtn_keepalive_get_period(btn))

/**
 * \brief           获取按钮上的连续单击事件数
 * \param[in]       btn: 要获取单击次数的按钮实例
 * \return          按钮上的连续单击次数
 */
#define ebtn_click_get_count(btn) ((btn)->click_cnt)

/**
 * \brief           设置按钮库配置
 * \param[in]       cfg_flags: 配置标志位，使用 EBTN_CFG_xxx 宏组合
 */
void ebtn_set_config(uint8_t cfg_flags);

/**
 * \brief           获取当前按钮库配置
 * \return          当前配置标志位
 */
uint8_t ebtn_get_config(void);

/**
 * \brief           设置组合键冲突抑制阈值
 *
 *                  当一个组合键的单击事件触发后，在此阈值时间内（毫秒），
 *                  属于该组合键成员的普通按键的单击事件将被抑制。
 *                  设置为 0 可禁用此功能。
 *
 * \param[in]       threshold: 抑制时间阈值，单位毫秒。
 */
void ebtn_set_combo_suppress_threshold(ebtn_time_t threshold);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _EBTN_H */
