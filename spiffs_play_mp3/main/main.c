#include <stdio.h>
#include "driver/i2s_common.h"
#include "driver/i2s_std.h"
#include "hal_i2s.h"
#include "app_spiffs.h"

hal_i2s_pin_t hal_i2s_pin = {
    .bclk_pin = 14,
    .dout_pin = 13,
    .ws_pin = 21,
};

void app_main(void)
{
    ESP_ERROR_CHECK(app_spiffs_init());
    ESP_ERROR_CHECK(audio_app_player_init(I2S_NUM_0, hal_i2s_pin, 16 * 1000));
    ESP_ERROR_CHECK(audio_app_player_music("/spiffs/wash_end_zh_1ch.mp3"));
}
