#include "hal_i2s.h"
#include "freertos/FreeRTOS.h"

i2s_chan_handle_t rx_handle = NULL; // I2S rx channel handler

/**
 * @brief 初始化I2S通信通道（仅接收模式）
 * 
 * 配置I2S控制器为Philips标准模式，单声道输入，支持主时钟（BCLK）和字选择（WS）信号，
 * 适用于连接外部音频ADC或数字麦克风。
 * 
 * @param config I2S配置参数（包含引脚、采样率、位宽等）
 * @return esp_err_t 
 *   - ESP_OK: 初始化成功
 *   - 其他错误码: 初始化失败（错误码按位或累积）
 * 
 * @note 
 * - 默认使用左声道数据（slot_mask = I2S_STD_SLOT_LEFT）
 * - 未启用主时钟（MCLK，设为GPIO_NUM_NC）
 * - 输入数据为单声道（I2S_SLOT_MODE_MONO）
 * - 错误处理使用 `|=` 操作符累积错误码（需调用者进一步判断具体错误源）
 */
 esp_err_t hal_i2s_init(i2s_config_t config)
 {
     esp_err_t ret_val = ESP_OK;
 
     /* 步骤1：创建I2S通道（仅接收） */
     i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(config.i2s_num, I2S_ROLE_MASTER);
     // 参数说明：
     // - config.i2s_num: I2S控制器编号（如I2S_NUM_0）
     // - I2S_ROLE_MASTER: 配置为主模式（生成BCLK和WS时钟）
 
     // 创建接收通道（tx_handle设为NULL表示不启用发送）
     ret_val |= i2s_new_channel(&chan_cfg, NULL, &rx_handle);
 
     /* 步骤2：配置I2S标准模式参数 */
     i2s_std_config_t std_cfg = {
         .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(config.sample_rate), // 设置采样率
         .slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(
             config.bits_per_sample,    // 数据位宽（如16/24/32位）
             I2S_SLOT_MODE_MONO        // 单声道模式
         ),
         .gpio_cfg = {
             .mclk = GPIO_NUM_NC,      // 禁用主时钟（MCLK）
             .bclk = config.bclk_pin,  // 位时钟引脚（BCLK）
             .ws = config.ws_pin,      // 字选择引脚（WS/LRCLK）
             .dout = GPIO_NUM_NC,      // 禁用发送数据线
             .din = config.din_pin,    // 接收数据引脚（DIN）
             .invert_flags = {
                 .mclk_inv = false,    // 不反转MCLK（未使用）
                 .bclk_inv = false,    // BCLK不反转
                 .ws_inv = false,      // WS不反转
             },
         },
     };
 
     /* 强制使用左声道数据（单声道场景下常见配置） */
     std_cfg.slot_cfg.slot_mask = I2S_STD_SLOT_LEFT;
 
     /* 步骤3：初始化I2S通道并启用 */
     ret_val |= i2s_channel_init_std_mode(rx_handle, &std_cfg); // 应用配置
     ret_val |= i2s_channel_enable(rx_handle);                  // 启用通道
 
     return ret_val; // 返回累积错误码（需调用者判断ESP_OK或其他）
 }

/**
 * @brief 从I2S通道读取音频数据并进行位处理
 * 
 * 该函数从I2S接收通道读取32位原始音频数据，通过右移操作提取有效位，转换为16位音频数据，
 * 以适应AFE（音频前端）的输入要求。处理后的数据将直接覆盖原始缓冲区。
 * 
 * @param buffer 指向存储音频数据的缓冲区的指针（处理后为int16_t类型数据）
 * @param buffer_len 缓冲区总长度（字节数）
 * @return esp_err_t 返回I2S操作结果状态码
 * 
 * @note 数据转换说明：
 * - I2S原始数据为32位有符号整型，其中[31:8]为有效位（24位），[7:0]为低位补零
 * - 右移14位操作等效于取原始数据的[29:14]位（16位），实现对音频信号的2^14倍数放大
 * - 强制类型转换直接覆盖缓冲区，原始32位数据被转换为16位数据（可能造成数据截断，此为设计预期）
 */
 esp_err_t hal_i2s_get_data(int16_t *buffer, int buffer_len)
 {
     esp_err_t ret = ESP_FAIL;
     size_t bytes_read;
     
     /* 计算32位数据块的个数（buffer总字节数 / int32_t类型大小） */
     int audio_chunksize = buffer_len / sizeof(int32_t);
     
     /* 从I2S通道读取原始数据（阻塞模式，portMAX_DELAY表示无限等待） */
     ret = i2s_channel_read(rx_handle, buffer, buffer_len, &bytes_read, portMAX_DELAY);
 
     /* 将缓冲区强制转换为32位指针进行位操作 */
     int32_t *tmp_buff = (int32_t *)buffer;
     
     /* 遍历所有32位数据块 */
     for (int i = 0; i < audio_chunksize; i++) {
         /* 右移14位操作：
          * 1. 丢弃低14位补零数据（8位有效低零 + 6位扩展）
          * 2. 取[29:14]位作为有效16位数据（符号位保留）
          * 3. 等效于将原始24位有效数据放大16384倍（2^14） */
         tmp_buff[i] = tmp_buff[i] >> 14;
     }
 
     return ret;
 }