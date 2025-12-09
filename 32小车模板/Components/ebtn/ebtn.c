#include <string.h>
#include "ebtn.h"

#define EBTN_FLAG_ONPRESS_SENT ((uint8_t)0x01) /*!< 标志位：表示已发送按下事件 */
#define EBTN_FLAG_IN_PROCESS ((uint8_t)0x02)   /*!< 标志位：表示按钮正在处理中 */

// 定义按钮类型标志 (用于 prv_process_btn)
#define EBTN_TYPE_NORMAL 0
#define EBTN_TYPE_COMBO  1

/* 默认按钮组实例 */
static ebtn_t ebtn_default;

/**
 * \brief           根据 key_id 获取组合按钮实例指针
 * \param[in]       key_id: 组合按钮的 key_id
 * \return          组合按钮实例指针，如果未找到则返回 NULL
 */
static ebtn_btn_combo_t *prv_get_combo_btn_by_key_id(uint16_t key_id)
{
    ebtn_t *ebtobj = &ebtn_default;
    int i;
    ebtn_btn_combo_dyn_t *target_combo;

    /* 搜索静态组合按钮 */
    for (i = 0; i < ebtobj->btns_combo_cnt; ++i)
    {
        if (ebtobj->btns_combo[i].btn.key_id == key_id)
        {
            return &ebtobj->btns_combo[i];
        }
    }

    /* 搜索动态组合按钮 */
    for (target_combo = ebtobj->btn_combo_dyn_head; target_combo; target_combo = target_combo->next)
    {
        if (target_combo->btn.btn.key_id == key_id)
        {
            return &target_combo->btn;
        }
    }
    return NULL; /* 未找到 */
}

/**
 * \brief           处理按钮信息和状态
 *
 * \param[in]       btn: 要处理的按钮实例
 * \param[in]       old_state: 旧状态
 * \param[in]       new_state: 新状态
 * \param[in]       mstime: 当前毫秒系统时间
 * \param[in]       btn_idx: 按钮在全局（静态+动态）数组中的索引（仅对普通按钮有效）
 * \param[in]       btn_type: 按钮类型 (EBTN_TYPE_NORMAL 或 EBTN_TYPE_COMBO)
 */
static void prv_process_btn(ebtn_btn_t *btn, uint8_t old_state, uint8_t new_state, ebtn_time_t mstime, int btn_idx, uint8_t btn_type)
{
    ebtn_t *ebtobj = &ebtn_default;

    /* 检查参数是否设置 */
    if (btn->param == NULL)
    {
        return;
    }

    /* 按钮状态刚刚改变 */
    if (new_state != old_state)
    {
        btn->time_state_change = mstime;

        if (new_state)
        {
            btn->flags |= EBTN_FLAG_IN_PROCESS; /* 标记按钮进入处理中状态 */
        }
    }
    /* 按钮仍然被按下 */
    if (new_state)
    {
        /*
         * 处理消抖并发送按下事件
         *
         * 这是我们检测到有效按下的地方
         */
        if (!(btn->flags & EBTN_FLAG_ONPRESS_SENT))
        {
            /*
             * 满足以下条件时执行 if 语句：
             *
             * - 运行时模式已启用 -> 用户为其去抖设置了自定义配置
             * - 按下的配置去抖时间大于 `0`
             */
            if (ebtn_timer_sub(mstime, btn->time_state_change) >= btn->param->time_debounce)
            {
                /*
                 * 检查是否达到多击限制。
                 */
                if ((btn->click_cnt > 0) && (ebtn_timer_sub(mstime, btn->click_last_time) >= btn->param->time_click_multi_max))
                {
                    // << 修改点: 增加抑制检查 >>
                    uint8_t suppress_click = 0;
                    if (btn_type == EBTN_TYPE_NORMAL && ebtobj->combo_suppress_threshold > 0 &&
                        ebtn_timer_sub(mstime, ebtobj->last_combo_event_time) < ebtobj->combo_suppress_threshold)
                    {
                        ebtn_btn_combo_t* last_combo = prv_get_combo_btn_by_key_id(ebtobj->last_combo_event_id);
                        if (last_combo != NULL && btn_idx >= 0 && bit_array_get(last_combo->comb_key, btn_idx))
                        {
                            suppress_click = 1;
                        }
                    }

                    if (!suppress_click && (btn->event_mask & EBTN_EVT_MASK_ONCLICK))
                    {
                        // << 修改点: 如果是组合键单击，记录时间和ID >>
                        if (btn_type == EBTN_TYPE_COMBO) {
                            ebtobj->last_combo_event_id = btn->key_id;
                            ebtobj->last_combo_event_time = mstime;
                        }
                        ebtobj->evt_fn(btn, EBTN_EVT_ONCLICK); /* 发送单击事件 */
                    }
                    // << 修改点: 如果抑制，也要重置计数 >>
                    btn->click_cnt = 0; /* 重置点击计数 */
                }

                /* 设置保持活动时间 */
                btn->keepalive_last_time = mstime;
                btn->keepalive_cnt = 0;

                /* 开始新的按下处理 */
                btn->flags |= EBTN_FLAG_ONPRESS_SENT; /* 标记已发送按下事件 */
                if (btn->event_mask & EBTN_EVT_MASK_ONPRESS)
                {
                    ebtobj->evt_fn(btn, EBTN_EVT_ONPRESS); /* 发送按下事件 */
                }

                btn->time_change = mstime; /* 按钮状态现在已改变 */
            }
        }
        /*
         * 处理保持活动事件，但仅在按下事件已发送后
         *
         * 当检测到有效按下时发送保持活动事件
         */
        else
        {
            /* 处理保持活动事件循环 */
            while ((btn->param->time_keepalive_period > 0) && (ebtn_timer_sub(mstime, btn->keepalive_last_time) >= btn->param->time_keepalive_period))
            {
                btn->keepalive_last_time += btn->param->time_keepalive_period;
                ++btn->keepalive_cnt;
                if (btn->event_mask & EBTN_EVT_MASK_KEEPALIVE)
                {
                    ebtobj->evt_fn(btn, EBTN_EVT_KEEPALIVE); /* 发送保持活动事件 */
                }
            }

            // 场景1：多次点击以长按结束，需要发送单击事件。
            if ((btn->click_cnt > 0) && (ebtn_timer_sub(mstime, btn->time_change) > btn->param->time_click_pressed_max))
            {
                // << 修改点: 增加抑制检查 >>
                uint8_t suppress_click = 0;
                if (btn_type == EBTN_TYPE_NORMAL && ebtobj->combo_suppress_threshold > 0 &&
                    ebtn_timer_sub(mstime, ebtobj->last_combo_event_time) < ebtobj->combo_suppress_threshold)
                {
                    ebtn_btn_combo_t* last_combo = prv_get_combo_btn_by_key_id(ebtobj->last_combo_event_id);
                    if (last_combo != NULL && btn_idx >= 0 && bit_array_get(last_combo->comb_key, btn_idx))
                    {
                        suppress_click = 1;
                    }
                }

                if (!suppress_click && (btn->event_mask & EBTN_EVT_MASK_ONCLICK))
                {
                    // << 修改点: 如果是组合键单击，记录时间和ID >>
                    if (btn_type == EBTN_TYPE_COMBO) {
                        ebtobj->last_combo_event_id = btn->key_id;
                        ebtobj->last_combo_event_time = mstime;
                    }
                    ebtobj->evt_fn(btn, EBTN_EVT_ONCLICK); /* 发送单击事件 */
                }
                // << 修改点: 如果抑制，也要重置计数 >>
                btn->click_cnt = 0; /* 重置点击计数 */
            }
        }
    }
    /* 按钮仍然被释放 */
    else
    {
        /*
         * 我们只需要在按下事件已经开始的情况下做出反应。
         *
         * 如果不是这种情况，则不执行任何操作
         */
        if (btn->flags & EBTN_FLAG_ONPRESS_SENT)
        {
            /*
             * 满足以下条件时执行 if 语句：
             *
             * - 运行时模式已启用 -> 用户为其去抖设置了自定义配置
             * - 释放的配置去抖时间大于 `0`
             */
            if (ebtn_timer_sub(mstime, btn->time_state_change) >= btn->param->time_debounce_release)
            {
                /* 处理释放事件 */
                btn->flags &= ~EBTN_FLAG_ONPRESS_SENT; /* 清除已发送按下事件标志 */
                if (btn->event_mask & EBTN_EVT_MASK_ONRELEASE)
                {
                    ebtobj->evt_fn(btn, EBTN_EVT_ONRELEASE); /* 发送释放事件 */
                }

                /* 检查单击事件的时间有效性 */
                if (ebtn_timer_sub(mstime, btn->time_change) >= btn->param->time_click_pressed_min &&
                    ebtn_timer_sub(mstime, btn->time_change) <= btn->param->time_click_pressed_max)
                {
                    ++btn->click_cnt;              /* 增加点击计数 */
                    btn->click_last_time = mstime; /* 更新最后点击时间 */
                }
                else
                {
                    // 场景2：如果最后一次按下太短，并且之前的点击序列是有效的，则向用户发送事件。
                    if ((btn->click_cnt > 0) && (ebtn_timer_sub(mstime, btn->time_change) < btn->param->time_click_pressed_min))
                    {
                        // << 修改点: 增加抑制检查 >>
                        uint8_t suppress_click = 0;
                        if (btn_type == EBTN_TYPE_NORMAL && ebtobj->combo_suppress_threshold > 0 &&
                            ebtn_timer_sub(mstime, ebtobj->last_combo_event_time) < ebtobj->combo_suppress_threshold)
                        {
                            ebtn_btn_combo_t* last_combo = prv_get_combo_btn_by_key_id(ebtobj->last_combo_event_id);
                            if (last_combo != NULL && btn_idx >= 0 && bit_array_get(last_combo->comb_key, btn_idx))
                            {
                                suppress_click = 1;
                            }
                        }

                        if (!suppress_click && (btn->event_mask & EBTN_EVT_MASK_ONCLICK))
                        {
                            // << 修改点: 如果是组合键单击，记录时间和ID >>
                            if (btn_type == EBTN_TYPE_COMBO) {
                                ebtobj->last_combo_event_id = btn->key_id;
                                ebtobj->last_combo_event_time = mstime;
                            }
                            ebtobj->evt_fn(btn, EBTN_EVT_ONCLICK); /* 发送单击事件 */
                        }
                        // << 修改点: 如果抑制，也要重置计数 >>
                        // 这种情况不需要重置，因为是在无效点击后发送之前的有效点击
                    }
                    /*
                     * 有一个释放事件，但单击事件检测的时间超出了允许的窗口。
                     *
                     * 重置点击计数器 -> 单击事件的无效序列。
                     */
                    btn->click_cnt = 0;
                }

                // 场景3：如果已达到最大连续点击次数，这部分将在释放事件后立即发送单击事件。
                if ((btn->click_cnt > 0) && (btn->click_cnt == btn->param->max_consecutive))
                {
                    // << 修改点: 增加抑制检查 >>
                    uint8_t suppress_click = 0;
                    if (btn_type == EBTN_TYPE_NORMAL && ebtobj->combo_suppress_threshold > 0 &&
                        ebtn_timer_sub(mstime, ebtobj->last_combo_event_time) < ebtobj->combo_suppress_threshold)
                    {
                        ebtn_btn_combo_t* last_combo = prv_get_combo_btn_by_key_id(ebtobj->last_combo_event_id);
                        if (last_combo != NULL && btn_idx >= 0 && bit_array_get(last_combo->comb_key, btn_idx))
                        {
                            suppress_click = 1;
                        }
                    }

                    if (!suppress_click && (btn->event_mask & EBTN_EVT_MASK_ONCLICK))
                    {
                        // << 修改点: 如果是组合键单击，记录时间和ID >>
                        if (btn_type == EBTN_TYPE_COMBO) {
                            ebtobj->last_combo_event_id = btn->key_id;
                            ebtobj->last_combo_event_time = mstime;
                        }
                        ebtobj->evt_fn(btn, EBTN_EVT_ONCLICK); /* 发送单击事件 */
                    }
                    // << 修改点: 如果抑制，也要重置计数 >>
                    btn->click_cnt = 0; /* 重置点击计数 */
                }

                btn->time_change = mstime; /* 按钮状态现在已改变 */
            }
        }
        else
        {
            /*
             * 根据配置，代码的这部分将在特定超时后发送单击事件。
             *
             * 如果用户偏好在最后一次点击事件发生后才报告多击功能（包括用户点击的次数），则此功能很有用
             */
            if (btn->click_cnt > 0)
            {
                /* 如果距离上次点击时间超过了多击最大间隔 */
                if (ebtn_timer_sub(mstime, btn->click_last_time) >= btn->param->time_click_multi_max)
                {
                    // << 修改点: 增加抑制检查 >>
                    uint8_t suppress_click = 0;
                    if (btn_type == EBTN_TYPE_NORMAL && ebtobj->combo_suppress_threshold > 0 &&
                        ebtn_timer_sub(mstime, ebtobj->last_combo_event_time) < ebtobj->combo_suppress_threshold)
                    {
                        ebtn_btn_combo_t* last_combo = prv_get_combo_btn_by_key_id(ebtobj->last_combo_event_id);
                        if (last_combo != NULL && btn_idx >= 0 && bit_array_get(last_combo->comb_key, btn_idx))
                        {
                            suppress_click = 1;
                        }
                    }

                    if (!suppress_click && (btn->event_mask & EBTN_EVT_MASK_ONCLICK))
                    {
                        // << 修改点: 如果是组合键单击，记录时间和ID >>
                        if (btn_type == EBTN_TYPE_COMBO) {
                            ebtobj->last_combo_event_id = btn->key_id;
                            ebtobj->last_combo_event_time = mstime;
                        }
                        ebtobj->evt_fn(btn, EBTN_EVT_ONCLICK); /* 发送单击事件 */
                    }
                    // << 修改点: 如果抑制，也要重置计数 >>
                    btn->click_cnt = 0; /* 重置点击计数 */
                }
            }
            else
            {
                /* 检查按钮是否在处理中 */
                if (btn->flags & EBTN_FLAG_IN_PROCESS)
                {
                    btn->flags &= ~EBTN_FLAG_IN_PROCESS; /* 清除处理中标志 */
                }
            }
        }
    }
}

/**
 * \brief           初始化 ebtn 库
 *
 * \param[in]       btns: 静态按钮数组
 * \param[in]       btns_cnt: 静态按钮数量
 * \param[in]       btns_combo: 静态组合按钮数组
 * \param[in]       btns_combo_cnt: 静态组合按钮数量
 * \param[in]       get_state_fn: 获取按钮状态的回调函数
 * \param[in]       evt_fn: 按钮事件回调函数
 * \return          成功返回 1，失败返回 0
 */
int ebtn_init(ebtn_btn_t *btns, uint16_t btns_cnt, ebtn_btn_combo_t *btns_combo, uint16_t btns_combo_cnt, ebtn_get_state_fn get_state_fn, ebtn_evt_fn evt_fn)
{
    ebtn_t *ebtobj = &ebtn_default;

    /* 检查必要的回调函数是否提供 */
    if (evt_fn == NULL || get_state_fn == NULL /* 在仅回调模式下，参数是必需的 */
    )
    {
        return 0; /* 初始化失败 */
    }

    memset(ebtobj, 0x00, sizeof(*ebtobj)); /* 清零默认按钮组实例 */
    ebtobj->btns = btns;
    ebtobj->btns_cnt = btns_cnt;
    ebtobj->btns_combo = btns_combo;
    ebtobj->btns_combo_cnt = btns_combo_cnt;
    ebtobj->evt_fn = evt_fn;
    ebtobj->get_state_fn = get_state_fn;
    ebtobj->config = 0; /* 默认不启用任何特殊配置 */

    // << 新增: 初始化组合键抑制相关变量 >>
    ebtobj->last_combo_event_id = 0xFFFF; // 使用无效ID
    ebtobj->last_combo_event_time = 0;
    ebtobj->combo_suppress_threshold = EBTN_DEFAULT_COMBO_SUPPRESS_THRESHOLD; // 设置默认阈值

    return 1; /* 初始化成功 */
}

/**
 * \brief           使用 get_state_fn 获取所有按钮的状态
 *
 * \param[out]      state_array: 存储按钮状态的位数组
 */
static void ebtn_get_current_state(bit_array_t *state_array)
{
    ebtn_t *ebtobj = &ebtn_default;
    ebtn_btn_dyn_t *target;
    int i;

    /* 处理所有静态按钮 */
    for (i = 0; i < ebtobj->btns_cnt; ++i)
    {
        /* 获取按钮状态 */
        uint8_t new_state = ebtobj->get_state_fn(&ebtobj->btns[i]);
        // 保存状态到状态数组
        bit_array_assign(state_array, i, new_state);
    }

    /* 处理所有动态注册的按钮 */
    for (target = ebtobj->btn_dyn_head, i = ebtobj->btns_cnt; target; target = target->next, i++)
    {
        /* 获取按钮状态 */
        uint8_t new_state = ebtobj->get_state_fn(&target->btn);

        // 保存状态到状态数组
        bit_array_assign(state_array, i, new_state);
    }
}

/**
 * \brief           处理单个按钮的状态
 *
 * \param[in]       btn: 要处理的按钮实例
 * \param[in]       old_state: 所有按钮的旧状态位数组
 * \param[in]       curr_state: 所有按钮的当前状态位数组
 * \param[in]       idx: 按钮的内部索引
 * \param[in]       mstime: 当前毫秒系统时间
 */
static void ebtn_process_btn(ebtn_btn_t *btn, bit_array_t *old_state, bit_array_t *curr_state, int idx, ebtn_time_t mstime)
{
    /* 调用内部处理函数 - 传递索引和普通按钮类型 */
    prv_process_btn(btn, bit_array_get(old_state, idx), bit_array_get(curr_state, idx), mstime, idx, EBTN_TYPE_NORMAL);
}

/**
 * \brief           处理组合按钮的状态
 *
 * \param[in]       btn: 组合按钮实例
 * \param[in]       old_state: 所有按钮的旧状态位数组
 * \param[in]       curr_state: 所有按钮的当前状态位数组
 * \param[in]       comb_key: 组合键的位数组
 * \param[in]       mstime: 当前毫秒系统时间
 */
static void ebtn_process_btn_combo(ebtn_btn_t *btn, bit_array_t *old_state, bit_array_t *curr_state, bit_array_t *comb_key, ebtn_time_t mstime)
{
    BIT_ARRAY_DEFINE(tmp_data, EBTN_MAX_KEYNUM) = {0}; /* 临时位数组 */

    /* 如果组合键为空，则直接返回 */
    if (bit_array_num_bits_set(comb_key, EBTN_MAX_KEYNUM) == 0)
    {
        return;
    }
    /* 计算当前状态下组合键是否被按下 */
    bit_array_and(tmp_data, curr_state, comb_key, EBTN_MAX_KEYNUM);
    uint8_t curr = bit_array_cmp(tmp_data, comb_key, EBTN_MAX_KEYNUM) == 0; /* 当前状态：1为按下，0为释放 */

    /* 计算旧状态下组合键是否被按下 */
    bit_array_and(tmp_data, old_state, comb_key, EBTN_MAX_KEYNUM);
    uint8_t old = bit_array_cmp(tmp_data, comb_key, EBTN_MAX_KEYNUM) == 0; /* 旧状态：1为按下，0为释放 */

    /* 调用内部处理函数 - 传递-1作为索引(不需要)，并标记为组合按钮类型 */
    prv_process_btn(btn, old, curr, mstime, -1, EBTN_TYPE_COMBO);
}

/**
 * \brief           使用给定的当前状态处理所有按钮
 *
 * \param[in]       curr_state: 所有按钮的当前状态位数组
 * \param[in]       mstime: 当前毫秒系统时间
 */
void ebtn_process_with_curr_state(bit_array_t *curr_state, ebtn_time_t mstime)
{
    ebtn_t *ebtobj = &ebtn_default;
    ebtn_btn_dyn_t *target;
    ebtn_btn_combo_dyn_t *target_combo;
    int i;
    uint8_t combo_priority = ebtobj->config & EBTN_CFG_COMBO_PRIORITY;

    // 清空组合键活动标记
    if (combo_priority)
    {
        bit_array_clear_all(ebtobj->combo_active, EBTN_MAX_KEYNUM);
    }

    // 首先处理所有组合按钮，用于填充组合键活动标记
    if (combo_priority)
    {
        /* 处理所有静态组合按钮 */
        for (i = 0; i < ebtobj->btns_combo_cnt; ++i)
        {
            // 检查组合键是否被按下
            BIT_ARRAY_DEFINE(tmp_data, EBTN_MAX_KEYNUM) = {0};
            bit_array_t *comb_key = ebtobj->btns_combo[i].comb_key;

            bit_array_and(tmp_data, curr_state, comb_key, EBTN_MAX_KEYNUM);
            uint8_t curr = bit_array_cmp(tmp_data, comb_key, EBTN_MAX_KEYNUM) == 0;

            // 如果组合键被按下，标记其成员按键为活动组合键的一部分
            if (curr)
            {
                bit_array_or(ebtobj->combo_active, ebtobj->combo_active, comb_key, EBTN_MAX_KEYNUM);
            }
        }

        /* 处理所有动态组合按钮 */
        for (target_combo = ebtobj->btn_combo_dyn_head; target_combo; target_combo = target_combo->next)
        {
            // 检查组合键是否被按下
            BIT_ARRAY_DEFINE(tmp_data, EBTN_MAX_KEYNUM) = {0};
            bit_array_t *comb_key = target_combo->btn.comb_key;

            bit_array_and(tmp_data, curr_state, comb_key, EBTN_MAX_KEYNUM);
            uint8_t curr = bit_array_cmp(tmp_data, comb_key, EBTN_MAX_KEYNUM) == 0;

            // 如果组合键被按下，标记其成员按键为活动组合键的一部分
            if (curr)
            {
                bit_array_or(ebtobj->combo_active, ebtobj->combo_active, comb_key, EBTN_MAX_KEYNUM);
            }
        }
    }

    /* 处理所有静态按钮 */
    for (i = 0; i < ebtobj->btns_cnt; ++i)
    {
        // 如果启用了组合键优先模式且该按键是当前活动组合键的一部分，则跳过处理
        if (combo_priority && bit_array_get(ebtobj->combo_active, i))
        {
            continue;
        }
        ebtn_process_btn(&ebtobj->btns[i], ebtobj->old_state, curr_state, i, mstime);
    }

    /* 处理所有动态按钮 */
    for (target = ebtobj->btn_dyn_head, i = ebtobj->btns_cnt; target; target = target->next, i++)
    {
        // 如果启用了组合键优先模式且该按键是当前活动组合键的一部分，则跳过处理
        if (combo_priority && bit_array_get(ebtobj->combo_active, i))
        {
            continue;
        }
        ebtn_process_btn(&target->btn, ebtobj->old_state, curr_state, i, mstime);
    }

    /* 处理所有静态组合按钮 */
    for (i = 0; i < ebtobj->btns_combo_cnt; ++i)
    {
        ebtn_process_btn_combo(&ebtobj->btns_combo[i].btn, ebtobj->old_state, curr_state, ebtobj->btns_combo[i].comb_key, mstime);
    }

    /* 处理所有动态组合按钮 */
    for (target_combo = ebtobj->btn_combo_dyn_head; target_combo; target_combo = target_combo->next)
    {
        ebtn_process_btn_combo(&target_combo->btn.btn, ebtobj->old_state, curr_state, target_combo->btn.comb_key, mstime);
    }

    /* 复制当前状态到旧状态，为下一次处理做准备 */
    bit_array_copy_all(ebtobj->old_state, curr_state, EBTN_MAX_KEYNUM);
}

/**
 * \brief           处理所有按钮的状态（获取当前状态并处理）
 *
 * \param[in]       mstime: 当前毫秒系统时间
 */
void ebtn_process(ebtn_time_t mstime)
{
    BIT_ARRAY_DEFINE(curr_state, EBTN_MAX_KEYNUM) = {0}; /* 定义当前状态位数组 */

    // 获取当前状态
    ebtn_get_current_state(curr_state);

    // 使用当前状态处理按钮
    ebtn_process_with_curr_state(curr_state, mstime);
}

/**
 * \brief           获取总按钮数量（静态+动态）
 * \return          总按钮数量
 */
int ebtn_get_total_btn_cnt(void)
{
    ebtn_t *ebtobj = &ebtn_default;
    int total_cnt = 0;
    ebtn_btn_dyn_t *curr = ebtobj->btn_dyn_head;

    total_cnt += ebtobj->btns_cnt; /* 加上静态按钮数量 */

    /* 遍历动态按钮链表 */
    while (curr)
    {
        total_cnt++;
        curr = curr->next;
    }
    return total_cnt;
}

/**
 * \brief           根据 key_id 获取按钮的索引
 * \param[in]       key_id: 按钮的 key_id
 * \return          按钮索引，如果未找到则返回 -1
 */
int ebtn_get_btn_index_by_key_id(uint16_t key_id)
{
    ebtn_t *ebtobj = &ebtn_default;
    int i = 0;
    ebtn_btn_dyn_t *target;

    /* 搜索静态按钮 */
    for (i = 0; i < ebtobj->btns_cnt; ++i)
    {
        if (ebtobj->btns[i].key_id == key_id)
        {
            return i;
        }
    }

    /* 搜索动态按钮 */
    for (target = ebtobj->btn_dyn_head, i = ebtobj->btns_cnt; target; target = target->next, i++)
    {
        if (target->btn.key_id == key_id)
        {
            return i;
        }
    }

    return -1; /* 未找到 */
}

/**
 * \brief           根据 key_id 获取按钮实例指针
 * \param[in]       key_id: 按钮的 key_id
 * \return          按钮实例指针，如果未找到则返回 NULL
 */
ebtn_btn_t *ebtn_get_btn_by_key_id(uint16_t key_id)
{
    ebtn_t *ebtobj = &ebtn_default;
    int i = 0;
    ebtn_btn_dyn_t *target;

    /* 搜索静态按钮 */
    for (i = 0; i < ebtobj->btns_cnt; ++i)
    {
        if (ebtobj->btns[i].key_id == key_id)
        {
            return &ebtobj->btns[i];
        }
    }

    /* 搜索动态按钮 */
    for (target = ebtobj->btn_dyn_head, i = ebtobj->btns_cnt; target; target = target->next, i++)
    {
        if (target->btn.key_id == key_id)
        {
            return &target->btn;
        }
    }

    return NULL; /* 未找到 */
}

/**
 * \brief           根据按钮实例指针获取按钮索引
 * \param[in]       btn: 按钮实例指针
 * \return          按钮索引，如果未找到则返回 -1
 */
int ebtn_get_btn_index_by_btn(ebtn_btn_t *btn)
{
    return ebtn_get_btn_index_by_key_id(btn->key_id);
}

/**
 * \brief           根据动态按钮实例指针获取按钮索引
 * \param[in]       btn: 动态按钮实例指针
 * \return          按钮索引，如果未找到则返回 -1
 */
int ebtn_get_btn_index_by_btn_dyn(ebtn_btn_dyn_t *btn)
{
    return ebtn_get_btn_index_by_key_id(btn->btn.key_id);
}

/**
 * \brief           通过索引向组合按钮添加一个按钮
 * \param[in,out]   btn: 组合按钮实例
 * \param[in]       idx: 要添加按钮的索引
 */
void ebtn_combo_btn_add_btn_by_idx(ebtn_btn_combo_t *btn, int idx)
{
    bit_array_set(btn->comb_key, idx); /* 在组合键位数组中设置对应位 */
}

/**
 * \brief           通过索引从组合按钮中移除一个按钮
 * \param[in,out]   btn: 组合按钮实例
 * \param[in]       idx: 要移除按钮的索引
 */
void ebtn_combo_btn_remove_btn_by_idx(ebtn_btn_combo_t *btn, int idx)
{
    bit_array_clear(btn->comb_key, idx); /* 在组合键位数组中清除对应位 */
}

/**
 * \brief           通过 key_id 向组合按钮添加一个按钮
 * \param[in,out]   btn: 组合按钮实例
 * \param[in]       key_id: 要添加按钮的 key_id
 */
void ebtn_combo_btn_add_btn(ebtn_btn_combo_t *btn, uint16_t key_id)
{
    int idx = ebtn_get_btn_index_by_key_id(key_id); /* 获取按钮索引 */
    if (idx < 0)
    {
        return; /* 按钮未找到 */
    }
    ebtn_combo_btn_add_btn_by_idx(btn, idx); /* 通过索引添加 */
}

/**
 * \brief           通过 key_id 从组合按钮中移除一个按钮
 * \param[in,out]   btn: 组合按钮实例
 * \param[in]       key_id: 要移除按钮的 key_id
 */
void ebtn_combo_btn_remove_btn(ebtn_btn_combo_t *btn, uint16_t key_id)
{
    int idx = ebtn_get_btn_index_by_key_id(key_id); /* 获取按钮索引 */
    if (idx < 0)
    {
        return; /* 按钮未找到 */
    }
    ebtn_combo_btn_remove_btn_by_idx(btn, idx); /* 通过索引移除 */
}

/**
 * \brief           检查按钮是否处于活动状态（已发送按下事件）
 * \param[in]       btn: 按钮实例指针
 * \return          如果按钮有效且处于活动状态，返回 1，否则返回 0
 */
int ebtn_is_btn_active(const ebtn_btn_t *btn)
{
    return btn != NULL && (btn->flags & EBTN_FLAG_ONPRESS_SENT);
}

/**
 * \brief           检查按钮是否正在处理中
 * \param[in]       btn: 按钮实例指针
 * \return          如果按钮有效且正在处理中，返回 1，否则返回 0
 */
int ebtn_is_btn_in_process(const ebtn_btn_t *btn)
{
    return btn != NULL && (btn->flags & EBTN_FLAG_IN_PROCESS);
}

/**
 * \brief           检查是否有任何按钮正在处理中
 * \return          如果有任何按钮正在处理中，返回 1，否则返回 0
 */
int ebtn_is_in_process(void)
{
    ebtn_t *ebtobj = &ebtn_default;
    ebtn_btn_dyn_t *target;
    ebtn_btn_combo_dyn_t *target_combo;
    int i;

    /* 检查所有静态按钮 */
    for (i = 0; i < ebtobj->btns_cnt; ++i)
    {
        if (ebtn_is_btn_in_process(&ebtobj->btns[i]))
        {
            return 1; /* 发现有按钮在处理中 */
        }
    }

    /* 检查所有动态按钮 */
    for (target = ebtobj->btn_dyn_head, i = ebtobj->btns_cnt; target; target = target->next, i++)
    {
        if (ebtn_is_btn_in_process(&target->btn))
        {
            return 1; /* 发现有按钮在处理中 */
        }
    }

    /* 检查所有静态组合按钮 */
    for (i = 0; i < ebtobj->btns_combo_cnt; ++i)
    {
        if (ebtn_is_btn_in_process(&ebtobj->btns_combo[i].btn))
        {
            return 1; /* 发现有按钮在处理中 */
        }
    }

    /* 检查所有动态组合按钮 */
    for (target_combo = ebtobj->btn_combo_dyn_head; target_combo; target_combo = target_combo->next)
    {
        if (ebtn_is_btn_in_process(&target_combo->btn.btn))
        {
            return 1; /* 发现有按钮在处理中 */
        }
    }

    return 0; /* 没有按钮在处理中 */
}

/**
 * \brief           注册一个动态按钮
 * \param[in]       button: 要注册的动态按钮实例指针
 * \return          成功返回 1，失败返回 0
 */
int ebtn_register(ebtn_btn_dyn_t *button)
{
    ebtn_t *ebtobj = &ebtn_default;

    ebtn_btn_dyn_t *curr = ebtobj->btn_dyn_head;
    ebtn_btn_dyn_t *last = NULL;

    /* 检查输入参数是否有效 */
    if (!button)
    {
        return 0; /* 无效指针 */
    }

    /* 检查是否达到最大按钮数量限制 */
    if (ebtn_get_total_btn_cnt() >= EBTN_MAX_KEYNUM)
    {
        return 0; /* 达到最大数量限制 */
    }

    /* 如果链表为空，直接设置为头节点 */
    if (curr == NULL)
    {
        ebtobj->btn_dyn_head = button;
        button->next = NULL; /* 确保新节点的 next 指针为 NULL */
        return 1;            /* 注册成功 */
    }

    /* 遍历链表查找是否已存在或找到链表末尾 */
    while (curr)
    {
        if (curr == button)
        {
            return 0; /* 按钮已存在 */
        }
        last = curr;
        curr = curr->next;
    }

    /* 将新按钮添加到链表末尾 */
    last->next = button;
    button->next = NULL; /* 确保新节点的 next 指针为 NULL */

    return 1; /* 注册成功 */
}

/**
 * \brief           注册一个动态组合按钮
 * \param[in]       button: 要注册的动态组合按钮实例指针
 * \return          成功返回 1，失败返回 0
 */
int ebtn_combo_register(ebtn_btn_combo_dyn_t *button)
{
    ebtn_t *ebtobj = &ebtn_default;

    ebtn_btn_combo_dyn_t *curr = ebtobj->btn_combo_dyn_head;
    ebtn_btn_combo_dyn_t *last = NULL;

    /* 检查输入参数是否有效 */
    if (!button)
    {
        return 0; /* 无效指针 */
    }

    /* 如果链表为空，直接设置为头节点 */
    if (curr == NULL)
    {
        ebtobj->btn_combo_dyn_head = button;
        button->next = NULL; /* 确保新节点的 next 指针为 NULL */
        return 1;            /* 注册成功 */
    }

    /* 遍历链表查找是否已存在或找到链表末尾 */
    while (curr)
    {
        if (curr == button)
        {
            return 0; /* 按钮已存在 */
        }
        last = curr;
        curr = curr->next;
    }

    /* 将新按钮添加到链表末尾 */
    last->next = button;
    button->next = NULL; /* 确保新节点的 next 指针为 NULL */

    return 1; /* 注册成功 */
}

/**
 * \brief           设置按钮库配置
 * \param[in]       cfg_flags: 配置标志位，使用 EBTN_CFG_xxx 宏组合
 */
void ebtn_set_config(uint8_t cfg_flags)
{
    ebtn_t *ebtobj = &ebtn_default;
    ebtobj->config = cfg_flags;
}

/**
 * \brief           获取当前按钮库配置
 * \return          当前配置标志位
 */
uint8_t ebtn_get_config(void)
{
    ebtn_t *ebtobj = &ebtn_default;
    return ebtobj->config;
}

/**
 * \brief           设置组合键冲突抑制阈值
 *
 *                  当一个组合键的单击事件触发后，在此阈值时间内（毫秒），
 *                  属于该组合键成员的普通按键的单击事件将被抑制。
 *                  设置为 0 可禁用此功能。
 *
 * \param[in]       threshold: 抑制时间阈值，单位毫秒。
 */
void ebtn_set_combo_suppress_threshold(ebtn_time_t threshold)
{
    ebtn_t *ebtobj = &ebtn_default;
    ebtobj->combo_suppress_threshold = threshold;
}

