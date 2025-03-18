#include <stdio.h>
#include "ds_wifi_ap_sta.h"
#include "driver/i2s_std.h"
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_psram.h"
#include <string.h>
#include "http_client.h"
#include "freertos/task.h"
#include "esp_http_client.h"
#include "esp_heap_caps.h"



#include "app_spiffs.h"
#include "ds_inmp441.h"
#include "ds_max98357.h"

#define TAG "AI_EXAMPLE"  // 定义 TAG 用于日志输出


i2s_microphone_config_t i2s_microphone_config = {
    .bclk_pin = 10,
    .ws_pin = 9,
    .din_pin = 11,
    .i2s_num = I2S_NUM_0,
    .sample_rate = 16 * 1000,
    .bits_per_sample = I2S_DATA_BIT_WIDTH_16BIT,
};
FILE *wav_file;
size_t wav_file_size = 0;
char *wav_raw_buffer = NULL;

extern i2s_chan_handle_t tx_handle; 
extern  i2s_chan_handle_t rx_handle;


static void read_audio_task(void* arg)
{
    while(1)
    {
        hal_i2s_record("/spiffs/record.wav", 5);
        wav_file = fopen("/spiffs/record.wav", "r");
        if (wav_file == NULL) {
            ESP_LOGI(TAG, "Read audio file failed");
        }
        fseek(wav_file, 0, SEEK_END);
        wav_file_size = ftell(wav_file);
        fseek(wav_file, 0, SEEK_SET);
        ESP_LOGI(TAG, "WAV File size:%zu", wav_file_size);
        wav_raw_buffer = heap_caps_malloc(wav_file_size + 1, MALLOC_CAP_DMA);
        if (wav_raw_buffer == NULL) {
            ESP_LOGI(TAG, "Malloc wav raw buffer fail");
            return;
        }
        fread(wav_raw_buffer, 1, wav_file_size, wav_file);
        fclose(wav_file);
        // 语音识别
        http_speech_to_text(wav_raw_buffer, wav_file_size);
        vTaskDelay(pdMS_TO_TICKS(1000)); // 延时 1 秒
    }

}

void app_main(void)
{
    // Init NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // Connect WIFI
    app_wifi_init("adol-3466", "123456");
    // Init spiffs
    ESP_ERROR_CHECK(app_spiffs_init("/spiffs"));
    // Init i2s microphone
    ESP_ERROR_CHECK(hal_i2s_microphone_init(i2s_microphone_config));
    http_init();
    i2s_speaker_init();
    xTaskCreatePinnedToCore(read_audio_task, "read_audio_task", 4096, NULL, 10, NULL,1);   

    while(1)
    {
        vTaskDelay(pdMS_TO_TICKS(1000)); // 延时 1 秒
    }
}
