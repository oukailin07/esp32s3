
#include "driver/i2s_std.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "ds_inmp441.h"



i2s_chan_handle_t rx_handle = NULL; // 接收通道句柄

void i2s_mic_init() {
    i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_MIC_NUM, I2S_ROLE_MASTER);
    i2s_new_channel(&chan_cfg, NULL, &rx_handle); // 只初始化接收通道

    i2s_std_config_t std_cfg = {
        .clk_cfg = {
            .sample_rate_hz = I2S_MIC_SAMPLE_RATE, // 采样率 44.1kHz
            .clk_src = I2S_CLK_SRC_DEFAULT, // 默认时钟源
            .mclk_multiple = I2S_MCLK_MULTIPLE_384, // MCLK 倍频系数
        },
        .slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_24BIT, I2S_SLOT_MODE_MONO),
        .gpio_cfg = {
            .mclk = I2S_GPIO_UNUSED,
            .bclk = 10,
            .ws = 9,
            .dout = I2S_GPIO_UNUSED,
            .din = 11,
            .invert_flags = {
                .mclk_inv = false,
                .bclk_inv = false,
                .ws_inv = false,
            },
        },
    };

    std_cfg.slot_cfg.slot_mask = I2S_STD_SLOT_LEFT; // 修改为左声道
    std_cfg.slot_cfg.bit_order_lsb = true;          // 低位先行,这边我不确定,但采集的数据确实受环境声音的改变而改变,高位先行却没有
 
    i2s_channel_init_std_mode(rx_handle, &std_cfg);
    i2s_channel_enable(rx_handle);
}
