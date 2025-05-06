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
#define FFT_SIZE        1024
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
const int peak_fall_speed = 2;        // 白块每帧下降速度（像素）

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
    int color_group_size = 8;  // 每种基准色调的柱子数量

    // 基准色调：红、橙、黄、绿、青、蓝、紫、粉（HSV色相角度大致）
    uint16_t base_hues[] = {0, 60, 120, 180, 210, 270, 330, 0}; // 红色、黄色、绿色、青色、蓝色、紫色、粉色等

    for (int i = 0; i < FFT_BAR_NUM; i++) {
        int bin = i * (FFT_SIZE / 2) / FFT_BAR_NUM;
        float value = fft_data[bin];

        // 限幅
        if (value > 100.0f) value = 100.0f;
        if (value < 0.0f) value = 0.0f;

        // 计算柱子高度与块数
        int bar_h = (int)(value * FFT_CANVAS_HEIGHT / 100.0f);
        int blocks_to_draw = bar_h / block_h;

        // 获取该柱子的色调索引（每 color_group_size 个柱子为一个色调）
        int color_index = i / color_group_size;
        if (color_index >= sizeof(base_hues)/sizeof(base_hues[0])) {
            color_index = sizeof(base_hues)/sizeof(base_hues[0]) - 1;
        }

        int base_hue = base_hues[color_index];
        int bar_x = i * FFT_BAR_WIDTH;

        for (int j = 0; j < blocks_to_draw; j++) {
            int block_y = FFT_CANVAS_HEIGHT - (j + 1) * block_h + BLOCK_SPACING / 2;

            // 渐变：上方块颜色更深，下方更亮
            uint8_t sat = 200;  // 饱和度固定为 200，确保颜色不变灰
            uint8_t val = 255 - (j * 155 / BLOCK_NUM);  // 渐变值：255 ~ 100，表示上方块颜色更深，下方更亮
            if (val < 100) val = 100;

            // 保持色相为基准色调，饱和度为固定值，亮度根据块的高度变化
            lv_color_t block_color = hsv_to_rgb(base_hue, sat, val);

            // 主色块
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

            // 发光边缘（可选）
            lv_draw_rect_dsc_t glow_dsc;
            lv_draw_rect_dsc_init(&glow_dsc);
            glow_dsc.bg_color = lv_color_white();
            glow_dsc.bg_opa = LV_OPA_20;
            glow_dsc.radius = BLOCK_RADIUS;

            lv_canvas_draw_rect(canvas,
                                bar_x - 1,
                                block_y - 1,
                                bar_width + 2,
                                block_h - BLOCK_SPACING + 2,
                                &glow_dsc);
        }

        // 峰值白块逻辑
        int current_peak_y = FFT_CANVAS_HEIGHT - bar_h;

        if (peak_y[i] == 0 || current_peak_y < peak_y[i]) {
            peak_y[i] = current_peak_y;
        } else {
            peak_y[i] += 2;
            if (peak_y[i] > FFT_CANVAS_HEIGHT - block_h) {
                peak_y[i] = FFT_CANVAS_HEIGHT - block_h;
            }
        }

        // 绘制顶部白色小块
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





// FFT 读取麦克风数据
void fft_task(void *arg)
{

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
    fft_input = (float *)heap_caps_malloc(sizeof(float) * 2 * FFT_SIZE, MALLOC_CAP_32BIT | MALLOC_CAP_INTERNAL);
    fft_output = (float *)heap_caps_malloc(sizeof(float) * FFT_SIZE, MALLOC_CAP_32BIT | MALLOC_CAP_INTERNAL);  // 如果用来放幅度
    i2s_read_buff = (int16_t *)heap_caps_malloc(sizeof(int16_t) * 2 * FFT_SIZE, MALLOC_CAP_32BIT | MALLOC_CAP_INTERNAL);
    fft_result_queue = xQueueCreate(1, sizeof(float) * FFT_SIZE / 2);
    assert(fft_result_queue != NULL);
    assert(fft_input != NULL);
    assert(fft_output != NULL);
    assert(i2s_read_buff != NULL);
    lvgl_fft_canvas_init();
    xTaskCreatePinnedToCore(lvgl_update_task, "lvgl_update_task", 4096 * 2, NULL, 1, NULL, 1);
    xTaskCreatePinnedToCore(fft_task, "fft_lvgl", 4096 * 2, NULL, 5, NULL,0);
    

}