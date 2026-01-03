#include "key_app.h"
// 按钮参数配置
static const ebtn_btn_param_t btn_param = EBTN_PARAMS_INIT(
    20,    // time_debounce: 按下去抖50ms
    20,    // time_debounce_release: 释放去抖30ms
    50,    // time_click_pressed_min: 最短按下50ms（有效单击）
    500,  // time_click_pressed_max: 最长按下3秒（超过则不算单击）
    500,   // time_click_multi_max: 连击间隔800ms
    500,   // time_keepalive_period: 长按保持事件周期500ms
    3      // max_consecutive: 最大连续单击3次
);

// 定义普通按钮数组
static ebtn_btn_t buttons[] = {
    EBTN_BUTTON_INIT(BTN_MODE1_ID, &btn_param),  // MODE1按键
    EBTN_BUTTON_INIT(BTN_MODE2_ID, &btn_param),  // MODE2按键
    EBTN_BUTTON_INIT(BTN_MODE3_ID, &btn_param),  // MODE2按键
};

// 定义组合按钮数组
static ebtn_btn_combo_t combo_buttons[] = {
    EBTN_BUTTON_COMBO_INIT(BTN_COMBO_ID, &btn_param),  // MODE1+MODE2组合键
};

/**
 * @brief 获取按钮硬件状态
 * @param btn 按钮实例指针
 * @return 1-按下，0-释放
 */
uint8_t get_button_state(ebtn_btn_t *btn) {
    // 根据按钮ID读取硬件状态
    switch(btn->key_id) {
        case BTN_MODE1_ID: 
            return HAL_GPIO_ReadPin(MODE1_GPIO_Port, MODE1_Pin) == GPIO_PIN_RESET;
        case BTN_MODE2_ID: 
            return HAL_GPIO_ReadPin(MODE2_GPIO_Port, MODE2_Pin) == GPIO_PIN_RESET;
        case BTN_MODE3_ID:
            return HAL_GPIO_ReadPin(MODE3_GPIO_Port, MODE3_Pin) == GPIO_PIN_RESET;
        default: 
            return 0;
    }
}

/**
 * @brief 处理MODE1按键事件
 * @param evt 事件类型
 */
void handle_mode1_button(ebtn_evt_t evt) {
    switch(evt) {
            
        case EBTN_EVT_ONCLICK: {
            ebtn_btn_t *btn = ebtn_get_btn_by_key_id(BTN_MODE1_ID);
            uint16_t click_count = ebtn_click_get_count(btn);
            
            // 根据连击次数执行不同操作
            switch(click_count) {
                case 1:
                    pid_control_mode++;
                if(pid_control_mode>=MAX_PID_MODE)
                {
                    pid_control_mode=0;
                }
                    // 单击操作
                    break;
                case 2:
                    // 双击操作
                
                    break;
                case 3:
                    // 三击操作
                
                    break;
                default:
                    break;
            }
            break;
        }
        
        case EBTN_EVT_KEEPALIVE: {
            pid_running^=1;
            // 长按保持操作（每500ms触发一次）
            break;
        }
        
        default:
            break;
    }
}

/**
 * @brief 处理MODE2按键事件
 * @param evt 事件类型
 */
void handle_mode2_button(ebtn_evt_t evt) {
    switch(evt) {
        case EBTN_EVT_ONPRESS:
            // 在这里添加按下时的处理逻辑
            break;
            
        case EBTN_EVT_ONRELEASE:
            // 在这里添加释放时的处理逻辑
            break;
            
        case EBTN_EVT_ONCLICK: {
            ebtn_btn_t *btn = ebtn_get_btn_by_key_id(BTN_MODE2_ID);
            uint16_t click_count = ebtn_click_get_count(btn);
            
            // 根据连击次数执行不同操作
            switch(click_count) {
                case 1:
                   
                    // 单击操作
                    break;
                case 2:
                    
                    // 双击操作
                    break;
                default:
                    break;
            }
            break;
        }
        
        case EBTN_EVT_KEEPALIVE: {
           
            // 长按保持操作
            break;
        }
        
        default:
            break;
    }
}

/**
 * @brief 处理MODE2按键事件
 * @param evt 事件类型
 */
void handle_mode3_button(ebtn_evt_t evt) {
    switch(evt) {
        case EBTN_EVT_ONPRESS:
            // 在这里添加按下时的处理逻辑
            break;
            
        case EBTN_EVT_ONRELEASE:
            // 在这里添加释放时的处理逻辑
            break;
            
        case EBTN_EVT_ONCLICK: {
            ebtn_btn_t *btn = ebtn_get_btn_by_key_id(BTN_MODE3_ID);
            uint16_t click_count = ebtn_click_get_count(btn);
            
            // 根据连击次数执行不同操作
            switch(click_count) {
                case 1:
                   
                    // 单击操作
                    break;
                case 2:
                    
                    // 双击操作
                    break;
                default:
                    break;
            }
            break;
        }
        
        case EBTN_EVT_KEEPALIVE: {
           
            // 长按保持操作
            break;
        }
        
        default:
            break;
    }
}

/**
 * @brief 处理组合按键事件（MODE1+MODE2同时按下）
 * @param evt 事件类型
 */
void handle_combo_button(ebtn_evt_t evt) {
    switch(evt) {
            
        case EBTN_EVT_ONCLICK: {
            
            // 组合键单击操作
            // 例如：进入设置模式、重置系统等
            break;
        }        
        default:
            break;
    }
}

/**
 * @brief 按钮事件处理回调函数
 * @param btn 触发事件的按钮
 * @param evt 事件类型
 */
void button_event_handler(ebtn_btn_t *btn, ebtn_evt_t evt) {
    switch(btn->key_id) {
        case BTN_MODE1_ID:
            handle_mode1_button(evt);
            break;
        case BTN_MODE2_ID:
            handle_mode2_button(evt);
            break;
        case BTN_COMBO_ID:
            handle_combo_button(evt);
            break;
        default:
            break;
    }
}



/**
 * @brief 初始化按钮系统
 */
void button_system_init(void) {
    // 初始化按钮库
    if (!ebtn_init(buttons, EBTN_ARRAY_SIZE(buttons),
                   combo_buttons, EBTN_ARRAY_SIZE(combo_buttons),
                   get_button_state, button_event_handler)) {
        return;
    }
    
    // 配置组合按钮：MODE1+MODE2
    ebtn_combo_btn_add_btn(&combo_buttons[0], BTN_MODE1_ID);
    ebtn_combo_btn_add_btn(&combo_buttons[0], BTN_MODE2_ID);
    
    // 启用组合键优先处理模式
    // 当组合键激活时，会抑制单个按键的事件
    ebtn_set_config(EBTN_CFG_COMBO_PRIORITY);
    
    // 设置组合键抑制阈值为300ms
    // 组合键单击事件触发后300ms内，成员按键的单击事件会被抑制
    ebtn_set_combo_suppress_threshold(300);
}

/**
 * @brief 按钮处理任务（在主循环或定时器中调用）
 * 建议调用频率: 10-20ms
 */

void Key_Task(void) {
    ebtn_process(uwTick);
}


