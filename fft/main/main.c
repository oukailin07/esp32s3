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
#include "audio_player.h"
#include "app_ui.h"
#include "esp32_s3_szp.h"
#define TAG "FFT_VIS"
float fft_table[CONFIG_DSP_MAX_FFT_SIZE];
#define FFT_SIZE        1024
QueueHandle_t fft_result_queue;
QueueHandle_t fft_result_queue_mp3;
int16_t *fft_pcm_buffer ;
// 采样缓冲区
int16_t *i2s_read_buff; // 16-bit stereo, 2倍空间
__attribute__((aligned(16)))
float fft_input[2 * FFT_SIZE];       // interleaved real/imag
__attribute__((aligned(16)))
float fft_output[FFT_SIZE/2];          // magnitude

extern esp_codec_dev_handle_t record_dev_handle;
extern i2s_chan_handle_t i2s_rx_chan;

#define NUM_BARS 64
lv_obj_t *bars[NUM_BARS];


static lv_obj_t *canvas;
static lv_color_t *canvas_buf;
#define FFT_BAR_NUM         64
#define FFT_BAR_WIDTH       5
#define FFT_BAR_HEIGHT      100
#define FFT_CANVAS_WIDTH    (FFT_BAR_NUM * FFT_BAR_WIDTH)
#define FFT_CANVAS_HEIGHT   FFT_BAR_HEIGHT

int peak_heights[FFT_BAR_NUM] = {0};  // 每个柱子的顶部白块高度
const int peak_fall_speed = 8;        // 白块每帧下降速度（像素）

#define BLOCK_NUM 20           // 每列分为多少小块
#define BLOCK_SPACING 0        // 块之间的间距
#define BLOCK_RADIUS 0         // 圆角半径
static int peak_y[FFT_BAR_NUM] = {0};  // 存储每一列白色小块的位置
void hsv_to_rgb(float h, float s, float v, uint8_t *r, uint8_t *g, uint8_t *b)
{
    float c = v * s;
    float x = c * (1 - fabsf(fmodf(h / 60.0f, 2) - 1));
    float m = v - c;
    float r_, g_, b_;

    if (h < 60) { r_ = c; g_ = x; b_ = 0; }
    else if (h < 120) { r_ = x; g_ = c; b_ = 0; }
    else if (h < 180) { r_ = 0; g_ = c; b_ = x; }
    else if (h < 240) { r_ = 0; g_ = x; b_ = c; }
    else if (h < 300) { r_ = x; g_ = 0; b_ = c; }
    else { r_ = c; g_ = 0; b_ = x; }

    *r = (r_ + m) * 255;
    *g = (g_ + m) * 255;
    *b = (b_ + m) * 255;
}


void lvgl_fft_canvas_init()
{
    canvas_buf = heap_caps_malloc(FFT_CANVAS_WIDTH * FFT_CANVAS_HEIGHT * sizeof(lv_color_t), MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    if (!canvas_buf) {
        ESP_LOGE(TAG, "Failed to allocate canvas buffer");
        return;
    }
    assert(canvas_buf != NULL);  // 调试阶段加断言
    canvas = lv_canvas_create(lv_scr_act());
    lv_canvas_set_buffer(canvas, canvas_buf, FFT_CANVAS_WIDTH, FFT_CANVAS_HEIGHT, LV_IMG_CF_TRUE_COLOR);
    lv_obj_align(canvas, LV_ALIGN_BOTTOM_LEFT, 0, 0);

    // 创建样式并设置渐变背景
    static lv_style_t style;
    lv_style_init(&style);

    // 设置渐变背景
    lv_style_set_bg_color(&style, lv_color_make(0, 0, 0));  // 起始颜色为黑色
    lv_style_set_bg_grad_color(&style, lv_color_make(40, 40, 40));  // 渐变到深灰色
    lv_style_set_bg_grad_dir(&style, LV_GRAD_DIR_VER);  // 垂直渐变

    // 应用样式到画布
    lv_obj_add_style(canvas, &style, 0);
}


static uint32_t last_color_switch_time = 0;   // 记录上次颜色切换时间
static int color_cycle_direction = 1;          // 颜色循环方向：1 为正向，-1 为反向
static float hue_offset = 0.0f;                // 颜色循环偏移量

void lvgl_fft_canvas_update(float *fft_avg) {
    lv_canvas_fill_bg(canvas, lv_color_black(), LV_OPA_COVER);  // 清屏

    int bar_width = FFT_BAR_WIDTH - 2;
    int block_h = FFT_CANVAS_HEIGHT / BLOCK_NUM;

    uint32_t current_time = lv_tick_get();
    
    // 每两秒切换一次颜色
    if (current_time - last_color_switch_time >= 50) {
        last_color_switch_time = current_time;
        hue_offset += color_cycle_direction * 3.0f;  // 每次偏移
        
        // 控制偏移范围
        if (hue_offset > 360.0f) {
            hue_offset = 0.0f;
        } else if (hue_offset < 0.0f) {
            hue_offset = 360.0f;
        }
    }

    for (int i = 0; i < FFT_BAR_NUM; i++) {
        //float value = 100.0f;  // 模拟默认值
        float value = fft_avg[i];
        value = fminf(fmaxf(value, 0.0f), 100.0f);

        int bar_h = (int)(value * FFT_CANVAS_HEIGHT / 100.0f);
        int blocks_to_draw = bar_h / block_h;
        int bar_x = i * FFT_BAR_WIDTH;

        // 从左到右的 hue 值（加上动态偏移）
        float hue = ((float)i / FFT_BAR_NUM) * 360.0f + hue_offset;
        if (hue > 360.0f) {
            hue -= 360.0f;
        }

        int denom = blocks_to_draw > 0 ? blocks_to_draw : 1;
        for (int j = 0; j < blocks_to_draw; j++) {
            int block_y = FFT_CANVAS_HEIGHT - (j + 1) * block_h + BLOCK_SPACING / 2;

            float value_brightness = 1.0f - ((float)j / denom) * 0.6f;  // 从底部 1.0 到顶部 0.4
            uint8_t r, g, b;
            hsv_to_rgb(hue, 1.0f, value_brightness, &r, &g, &b);
            lv_color_t block_color = lv_color_make(r, g, b);

            lv_draw_rect_dsc_t block_dsc;
            lv_draw_rect_dsc_init(&block_dsc);
            block_dsc.bg_color = block_color;
            block_dsc.bg_opa = LV_OPA_COVER;
            block_dsc.radius = (j < 2) ? 1 : BLOCK_RADIUS;

            lv_canvas_draw_rect(canvas,
                                bar_x,
                                block_y,
                                bar_width,
                                block_h - BLOCK_SPACING,
                                &block_dsc);
        }

        // 白色峰值块
        int current_peak_y = FFT_CANVAS_HEIGHT - bar_h;

        if (peak_y[i] == 0 || current_peak_y < peak_y[i]) {
            peak_y[i] = current_peak_y;
        } else {
            peak_y[i] += peak_fall_speed;
            if (peak_y[i] > FFT_CANVAS_HEIGHT - block_h) {
                peak_y[i] = FFT_CANVAS_HEIGHT - block_h;
            }
        }

        lv_draw_rect_dsc_t peak_dsc;
        lv_draw_rect_dsc_init(&peak_dsc);

        // === 关键修改点 ===
        if (bar_h < block_h) {
            // 音量太低，柱子高度小于一个 block，使用渐变色代替白色
            float value_brightness = 1.0f;
            uint8_t r, g, b;
            hsv_to_rgb(hue, 1.0f, value_brightness, &r, &g, &b);
            peak_dsc.bg_color = lv_color_make(r, g, b);
        } else {
            // 音量正常，使用白色峰值块
            peak_dsc.bg_color = lv_color_white();
        }
        // ===================
        peak_dsc.bg_opa = LV_OPA_COVER;
        peak_dsc.radius = BLOCK_RADIUS;

        lv_canvas_draw_rect(canvas,
                            bar_x,
                            peak_y[i],
                            bar_width,
                            block_h - BLOCK_SPACING,
                            &peak_dsc);
    }
}

#define SMOOTHING_FACTOR 1.0f
#define MAX_FFT_INPUT_VALUE 32767.0f

// 音频采集与 FFT 分析任务
void fft_task(void *arg)
{
    // 检查 FFT 输入缓冲区是否初始化
    if (!i2s_read_buff) {
        ESP_LOGE(TAG, "FFT buffers not initialized!");
        vTaskDelete(NULL);  // 删除当前任务
        return;
    }

    // 每个频谱柱表示的频段宽度（FFT 输出中前半部分用于分析）
    const int group_size = (FFT_SIZE / 2) / FFT_BAR_NUM;

    // 用于存储最终每个频谱柱的平均幅值
    float fft_result[FFT_BAR_NUM];

    while (1) {
        // 计算读取缓冲区长度（4 通道，每个通道 16 位，FFT_SIZE 个采样点）
        int buffer_len = sizeof(int16_t) * 4 * FFT_SIZE;

        // 从 I2S 接口读取音频数据
        esp_codec_dev_read(record_dev_handle, (void *)i2s_read_buff, buffer_len);

        // 预处理采样数据，并准备 FFT 输入
        for (int i = 0; i < FFT_SIZE; i++) {
            // 选择第4通道和第2通道的音频数据并混合（双声道平均）
            int16_t ch4 = i2s_read_buff[i * 4 + 3];
            int16_t ch2 = i2s_read_buff[i * 4 + 1];
            int16_t mixed = (ch4 + ch2) / 2;

            // 转换为 float 类型，并限制值范围
            float sample = (float)mixed;
            fft_input[2 * i] = fmaxf(fminf(sample, MAX_FFT_INPUT_VALUE), -MAX_FFT_INPUT_VALUE);  // 实部
            fft_input[2 * i + 1] = 0.0f;  // 虚部设为 0
        }

        // 进行 FFT 计算（实数输入，复数输出）
        dsps_fft2r_fc32(fft_input, FFT_SIZE);

        // 位反转（必要的 FFT 后处理步骤）
        dsps_bit_rev_fc32(fft_input, FFT_SIZE);

        // 计算频谱幅值并进行归一化和平滑处理
        for (int i = 0; i < FFT_SIZE / 2; i++) {
            float real = fft_input[2 * i];     // 实部
            float imag = fft_input[2 * i + 1]; // 虚部
            float mag = sqrtf(real * real + imag * imag);  // 幅值计算
            mag = isnan(mag) || mag < 0.0f ? 0.0f : mag;    // 处理异常值

            // 归一化到 0~100 范围
            float norm_mag = fminf(fmaxf(mag / MAX_FFT_INPUT_VALUE * 100.0f, 0.0f), 100.0f);

            // 使用滑动平均滤波实现频谱平滑效果
            fft_output[i] = (1 - SMOOTHING_FACTOR) * fft_output[i] + SMOOTHING_FACTOR * norm_mag;

            if (isnan(fft_output[i])) {
                fft_output[i] = 0.0f;  // 处理 NaN
            }
        }

        // 对每个频谱柱求平均幅值（频段分组）
        for (int i = 0; i < FFT_BAR_NUM; i++) {
            float sum = 0;
            for (int j = 0; j < group_size; j++) {
                sum += fft_output[i * group_size + j];  // 同一频段内累加
            }
            fft_result[i] = sum / group_size;  // 取平均值

            if (!isfinite(fft_result[i])) {
                fft_result[i] = 0.0f;  // 防止出现无穷大
            }

            // 可用于调试查看每个频段的幅值
            // ESP_LOGI(TAG, "fft_result[%d] = %f", i, fft_result[i]);
        }

        // 将频谱结果发送到消息队列中（覆盖旧数据）
        xQueueOverwrite(fft_result_queue, fft_result);

        // 小延时，释放 CPU 给其他任务使用
        vTaskDelay(1);
    }
}
SemaphoreHandle_t fft_data_ready = NULL;

void fft_task_mp3(void *arg)
{
    const int group_size = (FFT_SIZE / 2) / FFT_BAR_NUM;
    float fft_result[FFT_BAR_NUM];

    while (1) {
        if (xSemaphoreTake(fft_data_ready, portMAX_DELAY) == pdTRUE) {
            if (lvgl_port_lock(0)) {
            for (int i = 0; i < FFT_SIZE; i++) {
                float sample = (float)fft_pcm_buffer[i];
                fft_input[2 * i] = sample;
                fft_input[2 * i + 1] = 0.0f;
                //ESP_LOGE(TAG, "sample = %f", sample);
            }
            lvgl_port_unlock();   // 解锁
        }
            bsp_codec_volume_set(1, NULL);
            dsps_fft2r_fc32(fft_input, FFT_SIZE);
            dsps_bit_rev_fc32(fft_input, FFT_SIZE);

            for (int i = 0; i < FFT_SIZE / 2; i++) {
                float real = fft_input[2 * i];
                float imag = fft_input[2 * i + 1];
                float mag = sqrtf(real * real + imag * imag);
                float norm_mag = fminf(fmaxf(mag / MAX_FFT_INPUT_VALUE * 100.0f, 0.0f), 100.0f);
                fft_output[i] = (1 - SMOOTHING_FACTOR) * fft_output[i] + SMOOTHING_FACTOR * norm_mag;
                //ESP_LOGI(TAG, "fft_output[%d] = %f", i, fft_output[i]);
            }

            for (int i = 0; i < FFT_BAR_NUM; i++) {
                float sum = 0;
                for (int j = 0; j < group_size; j++) {
                    sum += fft_output[i * group_size + j];
                }
                fft_result[i] = sum / group_size;
            }

            xQueueOverwrite(fft_result_queue, fft_result);
        }
    }
}

void fft_task_pcm(void *arg)
{
    // 检查 FFT 输入缓冲区是否初始化
    if (!i2s_read_buff) {
        ESP_LOGE(TAG, "FFT buffers not initialized!");
        vTaskDelete(NULL);
        return;
    }

    // 打开 PCM 文件（只打开一次）
    static FILE *pcm_file = NULL;
    if (!pcm_file) {
        pcm_file = fopen("/spiffs/test.pcm", "rb");
        if (!pcm_file) {
            ESP_LOGE(TAG, "Failed to open /spiffs/test.pcm");
            vTaskDelete(NULL);
            return;
        }
    }

    const int group_size = (FFT_SIZE / 2) / FFT_BAR_NUM;
    float fft_result[FFT_BAR_NUM];

    while (1) {
        // 单声道，每个采样点 2 字节，总共 FFT_SIZE 个点
        int buffer_len = sizeof(int16_t) * FFT_SIZE;

        // 从文件中读取音频数据
        int read_bytes = fread(i2s_read_buff, 1, buffer_len, pcm_file);

        // 如果读取的数据小于期望大小，检查是否是文件结尾或发生了错误
        if (read_bytes < buffer_len) {
            if (read_bytes == 0) {
                // 文件已读取完毕，重新设置文件指针到开头
                ESP_LOGW(TAG, "Reached end of PCM file. Restarting from the beginning...");
                fseek(pcm_file, 0, SEEK_SET);  // 循环播放
            } else {
                // 发生读取错误
                ESP_LOGW(TAG, "Read error, retrying...");
                // 可选择延时，避免过快重复读取错误
                vTaskDelay(100 / portTICK_PERIOD_MS);
            }
        } else {
            // 成功读取了期望大小的数据，继续处理
            //ESP_LOGI(TAG, "Read PCM data successfully.");
        }
    
        // 处理数据：转换为 float，准备 FFT 输入（实部+虚部）
        int16_t *samples = (int16_t *)i2s_read_buff;
        for (int i = 0; i < FFT_SIZE; i++) {
            float sample = (float)samples[i];
            fft_input[2 * i] = fmaxf(fminf(sample, MAX_FFT_INPUT_VALUE), -MAX_FFT_INPUT_VALUE); // 实部
            fft_input[2 * i + 1] = 0.0f; // 虚部设为 0
        }

        // 进行 FFT 计算（实数输入，复数输出）
        dsps_fft2r_fc32(fft_input, FFT_SIZE);

        // 位反转（必要的 FFT 后处理步骤）
        dsps_bit_rev_fc32(fft_input, FFT_SIZE);

        // 计算频谱幅值并进行归一化和平滑处理
        for (int i = 0; i < FFT_SIZE / 2; i++) {
            float real = fft_input[2 * i];     // 实部
            float imag = fft_input[2 * i + 1]; // 虚部
            float mag = sqrtf(real * real + imag * imag);  // 幅值计算
            mag = isnan(mag) || mag < 0.0f ? 0.0f : mag;    // 处理异常值

            // 归一化到 0~100 范围
            float norm_mag = fminf(fmaxf(mag / MAX_FFT_INPUT_VALUE * 100.0f, 0.0f), 100.0f);

            // 使用滑动平均滤波实现频谱平滑效果
            fft_output[i] = (1 - SMOOTHING_FACTOR) * fft_output[i] + SMOOTHING_FACTOR * norm_mag;

            if (isnan(fft_output[i])) {
                fft_output[i] = 0.0f;  // 处理 NaN
            }
        }

        // 对每个频谱柱求平均幅值（频段分组）
        for (int i = 0; i < FFT_BAR_NUM; i++) {
            float sum = 0;
            for (int j = 0; j < group_size; j++) {
                sum += fft_output[i * group_size + j];  // 同一频段内累加
            }
            fft_result[i] = sum / group_size;  // 取平均值

            if (!isfinite(fft_result[i])) {
                fft_result[i] = 0.0f;  // 防止出现无穷大
            }

            // 可用于调试查看每个频段的幅值
            // ESP_LOGI(TAG, "fft_result[%d] = %f", i, fft_result[i]);
        }

        // 将频谱结果发送到消息队列中（覆盖旧数据）
        xQueueOverwrite(fft_result_queue, fft_result);

        // 小延时，释放 CPU 给其他任务使用
        vTaskDelay(10);
    }
}


void lvgl_update_task(void *arg)
{

    float recv_fft[FFT_BAR_NUM];

    while (1) {
        if (xQueueReceive(fft_result_queue, recv_fft, portMAX_DELAY)) {
            if (lvgl_port_lock(0)) {  // 加锁
                lvgl_fft_canvas_update(recv_fft);
                lvgl_port_unlock();   // 解锁
            } else {
                ESP_LOGW(TAG, "Failed to receive FFT result from queue.");
            }
        }
        vTaskDelay(1);  // 控制刷新速率
    }
}

void app_main(void)
{
    bsp_spiffs_mount(); // SPIFFS文件系统初始化
    ESP_ERROR_CHECK(bsp_i2c_init());  // I2C初始化
    pca9557_init();  // IO扩展芯片初始化
    bsp_lvgl_start(); // 初始化液晶屏lvgl接口
    bsp_codec_init();
    dsps_fft2r_init_fc32(fft_table, CONFIG_DSP_MAX_FFT_SIZE);
    fft_pcm_buffer = heap_caps_malloc(sizeof(int16_t) * FFT_SIZE, MALLOC_CAP_8BIT |MALLOC_CAP_INTERNAL);

    fft_data_ready = xSemaphoreCreateBinary();
    i2s_read_buff = (int16_t *)heap_caps_malloc(sizeof(int16_t) * 4 * FFT_SIZE, MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);
    fft_result_queue = xQueueCreate(1, sizeof(float) * FFT_BAR_NUM);
    fft_result_queue_mp3 = xQueueCreate(1, sizeof(float) * FFT_SIZE);
    assert(fft_result_queue != NULL);
    assert(fft_input != NULL);
    assert(fft_output != NULL);
    assert(i2s_read_buff != NULL);
    lvgl_fft_canvas_init();
    //mp3_player_init();

    xTaskCreatePinnedToCore(lvgl_update_task, "lvgl_update_task", 4096 * 2, NULL, 1, NULL, 1);
    //xTaskCreatePinnedToCore(fft_task, "fft_lvgl", 4096 * 2, NULL, 5, NULL,0);
    xTaskCreatePinnedToCore(fft_task_pcm, "fft_lvgl_pcm", 4096 * 2, NULL, 5, NULL,0);
    //xTaskCreatePinnedToCore(fft_task_mp3, "fft_task_mp3", 4096 * 2, NULL, 5, NULL,0);
    
}