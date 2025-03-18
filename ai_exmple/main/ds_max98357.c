#include "driver/i2s_std.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


i2s_chan_handle_t tx_handle = NULL; // 发送通道句柄

void i2s_speaker_init() {
    i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_1, I2S_ROLE_MASTER);
    i2s_new_channel(&chan_cfg, &tx_handle, NULL); // 只初始化发送通道

    i2s_std_config_t std_cfg = {
        .clk_cfg = {
            .sample_rate_hz = 16000, 
            .clk_src = I2S_CLK_SRC_DEFAULT, // 默认时钟源
            .mclk_multiple = I2S_MCLK_MULTIPLE_384, // MCLK 倍频系数
        },
        .slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_MONO),
        .gpio_cfg = {
            .mclk = I2S_GPIO_UNUSED,
            .bclk = 14,
            .ws = 21,
            .dout = 13,
            .din = I2S_GPIO_UNUSED,
            .invert_flags = {
                .mclk_inv = false,
                .bclk_inv = false,
                .ws_inv = false,
            },
        },
    };
    std_cfg.slot_cfg.slot_mask = I2S_STD_SLOT_LEFT; // 修改为左声道
    std_cfg.slot_cfg.bit_order_lsb = true;          // 低位先行,这边我不确定,但采集的数据确实受环境声音的改变而改变,高位先行却没有
 
    i2s_channel_init_std_mode(tx_handle, &std_cfg);
    i2s_channel_enable(tx_handle);
}



/*
void app_main() {
    i2s_mic_init();
    i2s_speaker_init();
    uint8_t* buffer = (uint8_t*)malloc(sizeof(uint8_t) * 3 * 64);
    size_t len = 0;

    while (1) {
        // 从麦克风读取数据
        i2s_channel_read(rx_handle, buffer, 64*3, &len, portMAX_DELAY);

        // 将数据发送到扬声器
        i2s_channel_write(tx_handle, buffer, len, &len, portMAX_DELAY);
    }
}
*/