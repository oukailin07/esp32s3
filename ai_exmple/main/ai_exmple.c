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
#include <string.h>  // 包含 strlen 函数的声明
#include "ds_inmp441.h"
#include "ds_max98357.h"
#include "ds_wifi_ap_sta.h"
#include "http_client.h"
#define SAMPLE_RATE 44100 // 采样率
#define BUFFER_SIZE (SAMPLE_RATE * 3) // 3秒的音频数据
#define TAG "AI_EXAMPLE"  // 定义 TAG 用于日志输出
extern i2s_chan_handle_t tx_handle; 
extern  i2s_chan_handle_t rx_handle;
void app_main(void)
{
	ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
	ds_wifi_ap_sta_init();
	ds_wifi_send_event(AP_STA_START);
    i2s_mic_init();
    i2s_speaker_init();
    uint8_t* buffer = (uint8_t*)malloc(sizeof(uint8_t) * 3 * 64);
    size_t len = 0;
    if (esp_psram_is_initialized())
        {
            printf("PSRAM is enabled.\n");
            // 获取 PSRAM 大小
            size_t psram_size = esp_psram_get_size();
            printf("PSRAM size: %u bytes\n", psram_size);
        }
        else
        {
            printf("PSRAM is not enabled.\n");
        }
    while (1) {
        // 提示开始读取语音
        ESP_LOGI(TAG, "Start recording audio for 3 seconds...");

        // 分配音频缓冲区
        uint8_t* buffer = (uint8_t*)malloc(BUFFER_SIZE * sizeof(uint8_t));
        if (buffer == NULL) {
            ESP_LOGE(TAG, "Failed to allocate memory for audio buffer");
            vTaskDelay(pdMS_TO_TICKS(1000));
            continue;
        }

        size_t total_len = 0;
        size_t len = 0;

        // 读取3秒的音频数据
        while (total_len < BUFFER_SIZE) {
            i2s_channel_read(rx_handle, buffer + total_len, BUFFER_SIZE - total_len, &len, portMAX_DELAY);
            total_len += len;
        }

        ESP_LOGI(TAG, "Audio recorded, total length: %d bytes", total_len);

        // 将音频发送到语音识别服务（百度智能云）
        char* recognized_text = http_speech_to_text(buffer, total_len);
        if (recognized_text != NULL) {
            ESP_LOGI(TAG, "Recognized Text: %s", recognized_text);

            // 将识别到的文本发送到MiniMax进行对话生成
            char* ai_response = http_ai_dialogue(recognized_text);
            if (ai_response != NULL) {
                ESP_LOGI(TAG, "AI Response: %s", ai_response);

                // 将AI生成的文本通过语音合成服务转换为音频并播放
                i2s_channel_write(tx_handle, ai_response, strlen(ai_response), &len, portMAX_DELAY);  // 播放音频
                free(ai_response);  // 释放AI响应的内存
            }
            free(recognized_text);  // 释放语音识别结果的内存
        }
        free(buffer);  // 释放音频缓冲区的内存

        vTaskDelay(pdMS_TO_TICKS(1000));  // 延时1秒
    }
}
