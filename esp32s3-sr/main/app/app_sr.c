#include <stdio.h>
#include <stdlib.h>
#include "stdbool.h"
#include "app_sr.h"
#include "esp_mn_speech_commands.h"
#include "esp_log.h"
#include "esp_wn_iface.h"
#include "esp_wn_models.h"
#include "esp_afe_sr_models.h"
#include "esp_mn_iface.h"
#include "esp_mn_models.h"
#include "model_path.h"
#include "esp_process_sdkconfig.h"
#include "string.h"
#include "hal_i2s.h"
#include "string.h"
#include "app_led.h"

// 定义日志标签
static const char *TAG = "APP_SR";

// 定义全局变量
static esp_afe_sr_iface_t *afe_handle = NULL;  // AFE（音频前端）处理接口
static esp_afe_sr_data_t *afe_data = NULL;     // AFE 数据
static srmodel_list_t *models = NULL;          // 语音识别模型列表
static uint8_t detect_flag = 0;                // 检测标志位，用于标识是否检测到唤醒词
extern app_led_config_t app_led_config;        // 外部定义的 LED 配置结构体

// 初始化语音识别模块
esp_err_t app_sr_init(char *model_partition_label)
{
    // 初始化语音识别模型
    models = esp_srmodel_init(model_partition_label);
    afe_config_t *afe_config = afe_config_init("MMNR", models, AFE_TYPE_SR, AFE_MODE_HIGH_PERF);
    char *wn_name = NULL;  // 唤醒词模型名称

    // 检查模型是否初始化成功
    if (models != NULL) {
        // 遍历模型列表，查找唤醒词模型
        for (int i = 0; i < models->num; i++) {
            if (strstr(models->model_name[i], ESP_WN_PREFIX) != NULL) {
                if (wn_name == NULL) {
                    wn_name = models->model_name[i];  // 获取唤醒词模型名称
                    ESP_LOGI(TAG, "The wakenet model: %s", wn_name);  // 打印唤醒词模型名称
                }
            }
        }
    } else {
        // 如果模型初始化失败，提示用户启用唤醒词模型
        ESP_LOGI(TAG, "Please enable wakenet model and select wake word by menuconfig!");
        return ESP_FAIL;
    }

    // 获取 AFE 处理接口
    //afe_handle = (esp_afe_sr_iface_t *)&ESP_AFE_SR_HANDLE;
    afe_handle = esp_afe_handle_from_config(afe_config);
    // 配置 AFE 参数
    //afe_config_t afe_config = AFE_CONFIG_DEFAULT();
    afe_config->memory_alloc_mode = AFE_MEMORY_ALLOC_MORE_PSRAM;  // 使用更多的 PSRAM
    afe_config->wakenet_init = true;  // 初始化唤醒词检测
    afe_config->wakenet_model_name = wn_name;  // 设置唤醒词模型名称
    //afe_config->voice_communication_init = false;  // 不初始化语音通信
    afe_config->aec_init = false;  // 不初始化回声消除
    afe_config->pcm_config.total_ch_num = 1;  // 总通道数
    afe_config->pcm_config.mic_num = 1;  // 麦克风数量
    afe_config->pcm_config.ref_num = 0;  // 参考通道数量

    // 根据配置创建 AFE 数据
    afe_data = afe_handle->create_from_config(afe_config);
    return ESP_OK;  // 返回成功状态
}

// 音频数据输入任务
void app_sr_feed_task(void *arg)
{
    esp_afe_sr_data_t *afe_data = arg;  // 获取 AFE 数据
    int audio_chunksize = afe_handle->get_feed_chunksize(afe_data);  // 获取每次输入的音频数据块大小
    int nch = afe_handle->get_channel_num(afe_data);  // 获取通道数
    int feed_channel = 1;  // 输入通道数
    assert(nch <= feed_channel);  // 确保通道数小于输入通道数

    // 分配 I2S 缓冲区
    int16_t *i2s_buff = malloc(audio_chunksize * sizeof(int16_t) * feed_channel);
    assert(i2s_buff);

    while (1) {
        // 从 I2S 获取音频数据
        size_t buffer_len = audio_chunksize * sizeof(int16_t) * feed_channel;
        hal_i2s_get_data(i2s_buff, buffer_len);

        // 将音频数据输入到 AFE
        afe_handle->feed(afe_data, i2s_buff);
    }
}

// 语音检测任务
void app_sr_detect_task(void *arg)
{
    esp_afe_sr_data_t *afe_data = arg;  // 获取 AFE 数据
    int afe_chunksize = afe_handle->get_fetch_chunksize(afe_data);  // 获取每次处理的音频数据块大小

    // 过滤出中文语音识别模型
    char *mn_name = esp_srmodel_filter(models, ESP_MN_PREFIX, ESP_MN_CHINESE);
    ESP_LOGI(TAG, "multinet:%s\n", mn_name);  // 打印模型名称

    // 获取语音识别模型接口
    esp_mn_iface_t *multinet = esp_mn_handle_from_name(mn_name);

    // 创建语音识别模型数据
    model_iface_data_t *model_data = multinet->create(mn_name, 3000);

    // 从 SDK 配置中更新语音命令
    esp_mn_commands_update_from_sdkconfig(multinet, model_data);

    // 获取语音识别模型的采样块大小
    int mu_chunksize = multinet->get_samp_chunksize(model_data);
    assert(mu_chunksize == afe_chunksize);  // 确保采样块大小与 AFE 的块大小一致

    // 添加自定义语音命令
    esp_mn_commands_add(1000, "guan deng");      // 关灯
    esp_mn_commands_add(1001, "kai deng");       // 开灯
    esp_mn_commands_add(1002, "cai hong deng");  // 彩虹灯
    esp_mn_commands_add(1003, "huan yan se");    // 换颜色
    esp_mn_commands_add(1004, "you dian liang"); // 有点亮
    esp_mn_commands_add(1005, "you dian an");    // 有点暗
    esp_mn_commands_add(1006, "yi jia yi");      // 一加一
    esp_mn_commands_add(1007, "zao shang hao");  // 早上好

    // 更新语音命令
    esp_mn_commands_update();

    while (1) {
        // 从 AFE 获取处理结果
        afe_fetch_result_t *res = afe_handle->fetch(afe_data);
        if (!res || res->ret_value == ESP_FAIL) {
            ESP_LOGI(TAG, "fetch error!");  // 如果获取失败，打印错误信息
            break;
        }

        // 检查唤醒词状态
        if (res->wakeup_state == WAKENET_DETECTED) {
            ESP_LOGI(TAG, "WAKEWORD DETECTED");  // 检测到唤醒词
            multinet->clean(model_data);  // 清除语音识别模型的状态
        } else if (res->wakeup_state == WAKENET_CHANNEL_VERIFIED) {
            detect_flag = 1;  // 设置检测标志位
            ESP_LOGI(TAG, "AFE_FETCH_CHANNEL_VERIFIED, channel index: %d", res->trigger_channel_id);  // 打印通道索引
        }

        // 如果检测到唤醒词
        if (detect_flag == 1) {
            // 使用语音识别模型检测语音
            esp_mn_state_t mn_state = multinet->detect(model_data, res->data);

            if (mn_state == ESP_MN_STATE_DETECTING) {
                continue;  // 如果正在检测中，继续循环
            }

            if (mn_state == ESP_MN_STATE_DETECTED) {
                // 获取语音识别结果
                esp_mn_results_t *mn_result = multinet->get_results(model_data);
                for (int i = 0; i < mn_result->num; i++) {
                    // 打印识别结果
                    ESP_LOGI(TAG, "TOP %d, command_id: %d, phrase_id: %d, string:%s prob: %f", i + 1, mn_result->command_id[i], mn_result->phrase_id[i], mn_result->string, mn_result->prob[i]);
                    if (mn_result->command_id[i] >= 1000 && mn_result->command_id[i] <= 1007) {
                        // 如果是自定义命令，更新 LED 配置
                        app_led_config.command = mn_result->command_id[i] - 1000;
                        continue;
                    }
                }

                ESP_LOGI(TAG, "listening ........");  // 打印正在监听
            }

            if (mn_state == ESP_MN_STATE_TIMEOUT) {
                // 如果超时，获取结果并重新启用唤醒词检测
                esp_mn_results_t *mn_result = multinet->get_results(model_data);
                ESP_LOGI(TAG, "timeout, string:%s", mn_result->string);
                afe_handle->enable_wakenet(afe_data);
                detect_flag = 0;  // 重置检测标志位
                ESP_LOGI(TAG, "awaits to be waken up ........");  // 打印等待唤醒
                continue;
            }
        }
    }
}

// 启动语音识别任务
void app_sr_task_start()
{
    // 创建音频数据输入任务
    xTaskCreatePinnedToCore(&app_sr_feed_task, "feed_task", 8 * 1024, (void *)afe_data, 5, NULL, 0);
    // 创建语音检测任务
    xTaskCreatePinnedToCore(&app_sr_detect_task, "detect_task", 8 * 1024, (void *)afe_data, 4, NULL, 1);
}