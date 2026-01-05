# 维特智能 IMU 接入与“动中打靶”V2.0 升级手册

> **定位**：这份文档是为您刚插入的 `imu_wit.c/h` 写的保姆级指南。它将指导您如何把 IMU 数据用起来，实现真正的“全姿态反馈”控制。

---

## 一、 第一步：硬件与底层配置 (CubeMX)

1.  **接线**：
    *   将维特智能 IMU 的 `TX` 接到 STM32 的某个空闲串口 `RX`（例如 `UART4` 或 `UART6`）。
    *   **供电**：VCC 接 3.3V/5V，GND 共地。
2.  **CubeMX 设置**：
    *   **Baud Rate**：通常默认为 `115200`（如果您的 IMU 设置过 921600，请匹配）。
    *   **DMA**：建议开启该串口的 `RX DMA` (Circular 模式)，以保证高频数据不丢包。

---

## 二、 第二步：代码集成 (三板斧)

### 1. 初始化
在 `main.c` 的初始化区域，调用：
```c
/* USER CODE BEGIN 2 */
IMU_Init(); // 初始化状态，准备归零
/* USER CODE END 2 */
```

### 2. 数据塞入 (串口接收处)
在您的串口接收处理函数中（例如 `uart_motor.c` 的 `Uart_Proc` 或者中断回调里），把收到的数据塞给解析器：
```c
// 假设是在 UART4 收到 IMU 数据
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == UART4) {
        // 调用我们刚刚加入的解析函数
        IMU_ParsePacket(uart4_rx_buffer, 11); // 11是维特单包长度
    }
}
```

### 3. 数据使用 (PID 控制处)
现在您可以在 `pid_app.c` 中直接调用 `IMU_GetYaw()` 或访问 `g_imu_data.gyro.z`。

---

## 三、 第三步：V2.0 终极方案逻辑 (如何吊打对手)

既然您现在既有 **编码器前馈**，又有 **IMU 反馈**，您可以组合出最强的控制流：

### 核心公式 (在 pid_app.c 中修改)：
```c
// 1. 获取视觉偏差输出 (外环)
X_Output = pid_calculate_positional(&X_pid, g_circle.center_x);

// 2. 减去底盘编码器前馈 (预测抵消)
X_Output -= (Chassis_Feedforward_Speed * Gimbal_FF_K);

// 3. 【新增：IMU 阻尼】
// 如果云台被路面颠簸惊动了，IMU 会产生角速度，我们直接把它也减掉
X_Output -= (g_imu_data.gyro.z * Gyro_Damping_K); 

// 4. 最后给电机
Motor_Position_Control(&X_huart, X_Output);
```

---

## 四、 常见问题 (避坑指南)

1.  **极性问题**：
    *   如果发现加上 IMU 数据后，云台疯狂乱抖，大概率是 `g_imu_data.gyro.z` 前面的符号反了。请尝试把 `-=` 改成 `+=`。
2.  **频率问题**：
    *   IMU 建议设置在 `100Hz` 或 `200Hz` 输出。如果太低，前馈的优势发挥不出来。
3.  **零点漂移**：
    *   `imu_wit.c` 已经带了上电自动归零功能。启动时请保持云台静止 1 秒。

---

**下一步建议**：
先完成硬件接线，然后在 `main.c` 里打印一下 `IMU_GetYaw()`，看看能不能正常看到角度随云台转动而变化。如果能看到，我们再进行 PID 融合！
