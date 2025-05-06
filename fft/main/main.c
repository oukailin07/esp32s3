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
#define FFT_SIZE        128
QueueHandle_t fft_result_queue;

// 采样缓冲区
int16_t *i2s_read_buff; // 16-bit stereo, 2倍空间
float *fft_input;       // interleaved real/imag
float *fft_output;          // magnitude

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
lv_color_t hsv_to_rgb(uint16_t h, uint8_t s, uint8_t v)
{
    float r, g, b;
    float hf = h / 60.0f; // 0-6
    int i = (int)hf;
    float f = hf - i;
    float p = v * (1.0f - s / 255.0f);
    float q = v * (1.0f - f * s / 255.0f);
    float t = v * (1.0f - (1.0f - f) * s / 255.0f);

    switch (i % 6) {
        case 0: r = v; g = t; b = p; break;
        case 1: r = q; g = v; b = p; break;
        case 2: r = p; g = v; b = t; break;
        case 3: r = p; g = q; b = v; break;
        case 4: r = t; g = p; b = v; break;
        case 5: r = v; g = p; b = q; break;
        default: r = g = b = 0; break;
    }

    return lv_color_make((uint8_t)r, (uint8_t)g, (uint8_t)b);
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


void lvgl_fft_canvas_update(float *fft_data)
{
    lv_canvas_fill_bg(canvas, lv_color_black(), LV_OPA_COVER);  // 清屏

    int bar_width = FFT_BAR_WIDTH - 2;
    int block_h = FFT_CANVAS_HEIGHT / BLOCK_NUM;
    int color_group_size = 8;  // 每种颜色基准的柱子数量

    // 八种颜色基准，每组从浅到深
    struct {
        uint8_t r_start, g_start, b_start;  // 浅色（底部）
        uint8_t r_end, g_end, b_end;        // 深色（顶部）
    } color_gradients[8] = {
        {255,150,150, 255,0,0},     // 红
        {255,177,0, 255,240,200},   // 橙
        {255,255,0, 255,255,200},   // 黄
        {200,255,200, 0,255,0},     // 绿
        {100,255,255, 0,255,255},   // 青
        {96,145,255, 0,78,255},     // 蓝
        {255,100,255, 255,0,255},   // 紫
        {255,200,255, 255,105,180}  // 粉
    };

    for (int i = 0; i < FFT_BAR_NUM; i++) {
        int bin = i * (FFT_SIZE / 2) / FFT_BAR_NUM;
        float value = fft_data[bin];

        value = fminf(fmaxf(value, 0.0f), 100.0f);  // 限幅到 0~100

        int bar_h = (int)(value * FFT_CANVAS_HEIGHT / 100.0f);
        int blocks_to_draw = bar_h / block_h;
        int bar_x = i * FFT_BAR_WIDTH;

        int color_index = i / color_group_size;
        if (color_index >= 8) color_index = 7;

        uint8_t r_start = color_gradients[color_index].r_start;
        uint8_t g_start = color_gradients[color_index].g_start;
        uint8_t b_start = color_gradients[color_index].b_start;
        uint8_t r_end = color_gradients[color_index].r_end;
        uint8_t g_end = color_gradients[color_index].g_end;
        uint8_t b_end = color_gradients[color_index].b_end;

        for (int j = 0; j < blocks_to_draw; j++) {
            int block_y = FFT_CANVAS_HEIGHT - (j + 1) * block_h + BLOCK_SPACING / 2;

            // 从底部（浅）到顶部（深）渐变
            uint8_t r = r_end - (r_end - r_start) * j / blocks_to_draw;
            uint8_t g = g_end - (g_end - g_start) * j / blocks_to_draw;
            uint8_t b = b_end - (b_end - b_start) * j / blocks_to_draw;

            lv_color_t block_color = lv_color_make(r, g, b);

            lv_draw_rect_dsc_t block_dsc;
            lv_draw_rect_dsc_init(&block_dsc);
            block_dsc.bg_color = block_color;
            block_dsc.bg_opa = LV_OPA_COVER;
            block_dsc.radius = BLOCK_RADIUS;

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
        peak_dsc.bg_color = lv_color_white();
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





#define SMOOTHING_FACTOR 0.02f
#define MAX_FFT_INPUT_VALUE 10000.0f  // 根据需求调整最大输入值


void fft_task(void *arg)
{
    if (!i2s_read_buff || !fft_input || !fft_output) {
        ESP_LOGE(TAG, "FFT buffers not initialized!");
        vTaskDelete(NULL);
        return;
    }

    while (1) {
        // 每个样本包含 4 通道，每通道 16bit（int16_t）
        int buffer_len = sizeof(int16_t) * 4 * FFT_SIZE;
        esp_codec_dev_read(record_dev_handle, (void *)i2s_read_buff, buffer_len);

        // 合并 4 个通道音频为一个通道进行 FFT
        for (int i = 0; i < FFT_SIZE; i++) {
            int16_t ch0 = i2s_read_buff[i * 4 + 0];
            int16_t ch1 = i2s_read_buff[i * 4 + 1];

            // 简单平均混音
            int16_t mixed = (ch0 + ch1) / 2;

            // 对 FFT 输入数据进行限制，防止异常值
            fft_input[2 * i] = fmaxf(fminf((float)mixed, MAX_FFT_INPUT_VALUE), -MAX_FFT_INPUT_VALUE);  // 实部
            fft_input[2 * i + 1] = 0.0f; // 虚部初始化为 0
        }

        // 执行 FFT
        dsps_fft2r_fc32(fft_input, FFT_SIZE);
        dsps_bit_rev_fc32(fft_input, FFT_SIZE);

        // 计算幅值并进行平滑处理
        for (int i = 0; i < FFT_SIZE / 2; i++) {
            float real = fft_input[2 * i];
            float imag = fft_input[2 * i + 1];
            float magnitude = sqrtf(real * real + imag * imag);

            // 如果幅值为 NaN 或负数，跳过这个值
            if (isnan(magnitude) || magnitude < 0.0f) {
                magnitude = 0.0f;
            }

            // 平滑处理
            fft_output[i] = (1 - SMOOTHING_FACTOR) * fft_output[i] + SMOOTHING_FACTOR * magnitude - 5.0f;

            // 检查输出是否有效，如果是 NaN 则设为默认值 0
            if (isnan(fft_output[i])) {
                ESP_LOGW(TAG, "Warning: FFT output[%d] is NaN", i);
                fft_output[i] = 0.0f; // 将 NaN 设置为 0 或其他默认值
            }

            //ESP_LOGI(TAG, "FFT output[%d]: %f", i, fft_output[i]);
        }

        // 拷贝结果并通过队列传递
        float fft_result[FFT_SIZE / 2];
        memcpy(fft_result, fft_output, sizeof(fft_result));
        xQueueOverwrite(fft_result_queue, fft_result);

        vTaskDelay(1); // 可调节刷新频率
    }
}





void lvgl_update_task(void *arg)
{

    float recv_fft[FFT_SIZE / 2];

    while (1) {
        if (xQueueReceive(fft_result_queue, recv_fft, portMAX_DELAY)) {
            if (lvgl_port_lock(0)) {  // 加锁
                lvgl_fft_canvas_update(recv_fft);
                lvgl_port_unlock();   // 解锁
            }
        }
        vTaskDelay(5);  // 控制刷新速率
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
    //lvgl_fft_ui_init();
    // fft_input = (float *)heap_caps_malloc(sizeof(float) * 2 * FFT_SIZE, MALLOC_CAP_32BIT | MALLOC_CAP_INTERNAL);
    // fft_output = (float *)heap_caps_malloc(sizeof(float) * FFT_SIZE / 2, MALLOC_CAP_32BIT | MALLOC_CAP_INTERNAL);  // 如果用来放幅度
    fft_input = (float *)heap_caps_aligned_alloc(16, sizeof(float) * 2 * FFT_SIZE, MALLOC_CAP_32BIT | MALLOC_CAP_INTERNAL);
    fft_output = (float *)heap_caps_aligned_alloc(16, sizeof(float) * FFT_SIZE / 2, MALLOC_CAP_32BIT | MALLOC_CAP_INTERNAL);
    ESP_LOGE(TAG, "fft_input addr = %p", fft_input);
    ESP_LOGE(TAG, "fft_output addr = %p", fft_output);
    i2s_read_buff = (int16_t *)heap_caps_malloc(sizeof(int16_t) * 4 * FFT_SIZE, MALLOC_CAP_32BIT | MALLOC_CAP_INTERNAL);
    fft_result_queue = xQueueCreate(1, sizeof(float) * FFT_SIZE / 2);
    assert(fft_result_queue != NULL);
    assert(fft_input != NULL);
    assert(fft_output != NULL);
    assert(i2s_read_buff != NULL);
    lvgl_fft_canvas_init();
    xTaskCreatePinnedToCore(lvgl_update_task, "lvgl_update_task", 4096 * 2, NULL, 1, NULL, 1);
    xTaskCreatePinnedToCore(fft_task, "fft_lvgl", 4096 * 2, NULL, 5, NULL,0);
    

}