#include "esp_dsp.h"
#include "lvgl.h"
#include <math.h>
#include "driver/i2s_std.h"
#include "dsps_fft2r.h"
#include "dsps_math.h"

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2s_std.h"
#include "esp_log.h"

#include "esp32_s3_szp.h"
#define TAG "FFT_VIS"
#define SAMPLE_RATE     16000
#define I2S_PORT        I2S_NUM_0
#define FFT_SIZE        1024
QueueHandle_t fft_result_queue;

// 采样缓冲区
int16_t *i2s_read_buff; // 16-bit stereo, 2倍空间
float *fft_input;       // interleaved real/imag
float *fft_output;          // magnitude

// LVGL 对象（柱状图）
lv_obj_t *bars[64]; // 假设 64 个柱状条
extern esp_codec_dev_handle_t record_dev_handle;
extern i2s_chan_handle_t i2s_rx_chan;



// 初始化 LVGL 柱状图
void lvgl_fft_ui_init()
{
    lv_obj_t *cont = lv_obj_create(lv_scr_act());
    lv_obj_set_size(cont, 320, 240);
    lv_obj_center(cont);

    for (int i = 0; i < 64; i++) {
        bars[i] = lv_bar_create(cont);
        lv_obj_set_size(bars[i], 4, 100);
        lv_bar_set_range(bars[i], 0, 100);
        lv_obj_align(bars[i], LV_ALIGN_BOTTOM_LEFT, i * 5, 0);
        //lv_obj_set_style_anim_time(bars[i], 100, 0); // 动画时间为 100 ms，可调
    }
}

// FFT 读取麦克风数据
void fft_task(void *arg)
{
    size_t bytes_read = 0;

    // 检查输入输出缓存是否为空
    if (!i2s_read_buff || !fft_input || !fft_output) {
        ESP_LOGE(TAG, "FFT buffers not initialized!");
        vTaskDelete(NULL);
        return;
    }

    while (1) {
        // 读取 I2S 音频数据
        int buffer_len = sizeof(int16_t) * 2 * FFT_SIZE;
        esp_codec_dev_read(record_dev_handle, (void *)i2s_read_buff, buffer_len);

        // 转换为 float 格式，填充复数数组（左声道）
        for (int i = 0; i < FFT_SIZE; i++) {
            int16_t sample = i2s_read_buff[i * 2];  // 假设左声道采样在前
            fft_input[2 * i] = (float)sample;       // 实部
            fft_input[2 * i + 1] = 0.0f;            // 虚部
        }

        // 执行 FFT（复数快速傅里叶变换）
        dsps_fft2r_fc32(fft_input, FFT_SIZE);
        dsps_bit_rev_fc32(fft_input, FFT_SIZE);

        // 计算复数幅度谱
        for (int i = 0; i < FFT_SIZE / 2; i++) {
            float real = fft_input[2 * i];
            float imag = fft_input[2 * i + 1];
            fft_output[i] = sqrtf(real * real + imag * imag);
        }

        // 发送数据到队列
        float fft_result[FFT_SIZE / 2];
        memcpy(fft_result, fft_output, sizeof(fft_result));
        // if (xQueueSend(fft_result_queue, fft_result, 0) != pdPASS) {
        //     ESP_LOGW(TAG, "Queue full, dropping FFT data");
        // }
        xQueueOverwrite(fft_result_queue, fft_result);
        // 降低延迟
        vTaskDelay(10); // 通过减小延迟时间来提升实时性
    }
}


#define UPDATE_THRESHOLD 40.0f      // 只有当变化大于这个值时才认为“有更新”
#define UPDATE_DELAY 5             // 每帧间隔时间（ms）
#define SMOOTH_STEP_UP 5.0f        // 上升步进值（越小越平滑）
#define SMOOTH_STEP_DOWN 2.0f      // 下降步进值（快速下降）

float last_fft[FFT_SIZE / 2] = {0};      // 存储实际频谱值
float displayed_level[64] = {0};         // 当前显示在柱状图上的值



void lvgl_update_task(void *arg)
{
    float recv_fft[FFT_SIZE / 2];

    while (1) {
        if (xQueueReceive(fft_result_queue, recv_fft, portMAX_DELAY)) {
            // 每次更新频谱时，只更新有显著变化的部分
            for (int i = 0; i < 64; i++) {
                int bin = i * (FFT_SIZE / 2) / 64;
                float value = recv_fft[bin];
                
                if (value > 100.0f) value = 100.0f;  // 限制最大值
                if (value < 0.0f) value = 0.0f;
                // 计算与上次值的差异
                float diff = fabs(value - last_fft[bin]);

                // 只有当变化大于阈值时才更新
                if (diff > UPDATE_THRESHOLD) {
                    int level = (int)(value);  // 0~100 之间
                    if (bars[i] == NULL) {
                        ESP_LOGE(TAG,"Warning: bars[%d] is NULL\n", i);
                        continue;
                    }
                    lv_bar_set_value(bars[i], level, LV_ANIM_OFF);
                    // 更新上次的频谱数据
                    last_fft[bin] = value;
                }
            }
        }

        // 调整更新频率，以平衡 CPU 占用与更新频率
        vTaskDelay(5);  // 控制 UI 更新的频率
    }
}



void app_main(void)
{
    bsp_spiffs_mount(); // SPIFFS文件系统初始化
    ESP_ERROR_CHECK(bsp_i2c_init());  // I2C初始化
    pca9557_init();  // IO扩展芯片初始化
    bsp_lvgl_start(); // 初始化液晶屏lvgl接口
    bsp_codec_init();
    dsps_fft2r_init_fc32(NULL, CONFIG_DSP_MAX_FFT_SIZE);
    lvgl_fft_ui_init();
    fft_input = (float *)heap_caps_malloc(sizeof(float) * 2 * FFT_SIZE, MALLOC_CAP_32BIT | MALLOC_CAP_INTERNAL);
    fft_output = (float *)heap_caps_malloc(sizeof(float) * FFT_SIZE, MALLOC_CAP_32BIT | MALLOC_CAP_INTERNAL);  // 如果用来放幅度
    i2s_read_buff = (int16_t *)heap_caps_malloc(sizeof(int16_t) * 2 * FFT_SIZE, MALLOC_CAP_32BIT | MALLOC_CAP_INTERNAL);
    fft_result_queue = xQueueCreate(1, sizeof(float) * FFT_SIZE / 2);
    assert(fft_result_queue != NULL);
    assert(fft_input != NULL);
    assert(fft_output != NULL);
    assert(i2s_read_buff != NULL);
    xTaskCreatePinnedToCore(lvgl_update_task, "lvgl_update_task", 4096 * 2, NULL, 1, NULL, 1);
    xTaskCreatePinnedToCore(fft_task, "fft_lvgl", 4096 * 2, NULL, 5, NULL,0);
    

}