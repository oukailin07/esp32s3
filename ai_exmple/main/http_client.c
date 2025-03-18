#include "http_client.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_http_client.h"
#include "esp_log.h"
#include "cJSON.h"
#include "esp_tls.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_crt_bundle.h"  // 包含根证书包
#include <stdlib.h>
#include <string.h>
#include "app_url_encode.h"
#include "ds_inmp441.h"
#include "ds_max98357.h"
#include <json_parser.h>

#define TAG "HTTP"
QueueHandle_t http_speech_to_text_event_queue;
QueueHandle_t http_ai_dialogue_event_queue;
QueueHandle_t http_tts_event_queue;
extern  QueueHandle_t read_start;
char *access_token = "24.0c10b25b8cb7f7167f1fe3aab0bd3bd1.2592000.1744720185.282335-118065803";
char *url_formate = "http://vop.baidu.com/server_api?dev_pid=1537&cuid=dPKArKm9yCGIOwPoCSjTDzmIIj4cBsEV&token=%s";
size_t text_url_encode_size = 0;

esp_http_client_handle_t stt_client;
esp_http_client_handle_t token_client;
esp_http_client_handle_t tongyiqianwen_client;
char *tts_url = "https://tsn.baidu.com/text2audio";
char *formate = "tex=%s&tok=%s&cuid=mpBNOBqqTHmz93GbNEZDm5vUnwV0Lnm1&ctp=1&lan=zh&spd=5&pit=5&vol=5&per=4&aue=4"; // PCM 16K
char *client_id = "HKXvSjLc5Co28bjtvVIXkVuE";
char *client_secret = "pzVcTIXM8K2mTHWuPWZVMZceoQqjptMf";
char token[256] = {0};


char *tongyi_url = "https://dashscope.aliyuncs.com/api/v1/services/aigc/text-generation/generation";
char *tongyiqianwen_key = "sk-276b0fb9cb854582be0452b0372854e5";
char *tongyiqianwen_format = "{\"model\": \"qwen-turbo\",\"input\": {\"messages\": [{\"role\": \"system\",\"content\": \"You are a helpful assistant.\"},{\"role\": \"user\",\"content\": \"%s\"}]},\"parameters\": {\"result_format\": \"message\"}}";

extern i2s_chan_handle_t tx_handle; 

esp_err_t app_http_tongyi_event_handler(esp_http_client_event_t *evt)
{
    if (evt->event_id == HTTP_EVENT_ON_DATA) {
        ESP_LOGI(TAG, "%.*s", evt->data_len, (char *)evt->data);
        //i2s_channel_write(tx_handle, (char *)evt->data, evt->data_len, NULL, 100);
    }
    esp_http_client_event_t *evt_t = (esp_http_client_event_t *)malloc(sizeof(esp_http_client_event_t));
        if (evt_t == NULL) {
            ESP_LOGE(TAG, "Memory allocation failed for evt_t");
            return ESP_FAIL;
        }
    memset(evt_t, 0, sizeof(esp_http_client_event_t));  // 避免野指针
    evt_t->data = strdup((char *)evt->data);   // 复制 evt.data，防止指针无效
    if (evt_t->data == NULL) {
        ESP_LOGE(TAG, "Memory allocation failed for evt_t->data");
        free(evt_t);  // 释放 evt_t
        return ESP_FAIL;
    }
    xQueueSend(http_ai_dialogue_event_queue, evt_t, pdMS_TO_TICKS(100));
    return ESP_OK;
}


//ai大模型
void app_ask_tongyi_task(void *pvParameters)
{   
    esp_http_client_event_t *evt = NULL;
    while (1) {
        if (xQueueReceive(http_speech_to_text_event_queue, &evt, portMAX_DELAY) == pdPASS)
        {
            ESP_LOGI(TAG, "app_ask_tongyi_task start----");
            char *data = heap_caps_calloc(1, strlen(tongyiqianwen_format) + strlen(evt->data) + 1, MALLOC_CAP_SPIRAM);
            sprintf(data, tongyiqianwen_format, evt->data);
            //ESP_LOGI(TAG, "%s %d", evt.data, strlen(evt.data));
    
            esp_http_client_config_t config = {
                .method = HTTP_METHOD_POST,
                .event_handler = app_http_tongyi_event_handler,
                .buffer_size = 4 * 1024,
            };
            config.url = tongyi_url;
            tongyiqianwen_client = esp_http_client_init(&config);
            esp_http_client_set_method(tongyiqianwen_client, HTTP_METHOD_POST);
            esp_http_client_set_header(tongyiqianwen_client, "Content-Type", "application/json");
            esp_http_client_set_header(tongyiqianwen_client, "Authorization", tongyiqianwen_key);
            esp_http_client_set_post_field(tongyiqianwen_client, data, strlen(data));
            esp_err_t err = esp_http_client_perform(tongyiqianwen_client);
            if (err == ESP_OK) {
                ESP_LOGI(TAG, "HTTP GET Status = %d, content_length = %d", esp_http_client_get_status_code(tongyiqianwen_client), (int)esp_http_client_get_content_length(tongyiqianwen_client));
            } else {
                ESP_LOGI(TAG, "HTTP GET request failed: %s", esp_err_to_name(err));
            }
            esp_http_client_cleanup(tongyiqianwen_client);
            free(data);
        }
        vTaskDelay(pdMS_TO_TICKS(1000)); // 延时 1 秒
    }
}

esp_err_t app_http_baidu_access_token_event_handler(esp_http_client_event_t *evt)
{
    if (evt->event_id == HTTP_EVENT_ON_DATA) {
        // ESP_LOGI(TAG, "%.*s", evt->data_len, (char *)evt->data);
        jparse_ctx_t jctx;
        int ret = json_parse_start(&jctx, (char *)evt->data, evt->data_len);
        if (ret != OS_SUCCESS) {
            ESP_LOGI(TAG, "Parser failed\n");
            return ESP_FAIL;
        }

        if (json_obj_get_string(&jctx, "access_token", token, sizeof(token)) == OS_SUCCESS) {
            ESP_LOGI(TAG, "access_token: %s\n", token);
        }

        json_parse_end(&jctx);
    }

    return ESP_OK;
}

// 获取百度智能云的Access Token
void get_baidu_access_token(void) {
    char url[512];
    snprintf(url, sizeof(url),
             "https://aip.baidubce.com/oauth/2.0/token?client_id=%s&client_secret=%s&grant_type=client_credentials",
             client_id, client_secret);

    esp_http_client_config_t config = {
        .url = url,
        .method = HTTP_METHOD_POST,
        .event_handler = app_http_baidu_access_token_event_handler,
        .buffer_size = 4 * 1024,
    };

    stt_client = esp_http_client_init(&config);
    if (!stt_client) {
        ESP_LOGE(TAG, "Failed to init HTTP client");
        return ;
    }
    esp_http_client_set_method(stt_client, HTTP_METHOD_POST);
    esp_http_client_set_header(stt_client, "Content-Type", "application/json");
    esp_http_client_set_header(stt_client, "Accept", "application/json");
    esp_err_t err = esp_http_client_perform(stt_client);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "HTTP request failed: %s", esp_err_to_name(err));
        esp_http_client_cleanup(stt_client);
        return ;
    }
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "HTTP GET Status = %d, content_length = %d", esp_http_client_get_status_code(stt_client), (int)esp_http_client_get_content_length(stt_client));
    } else {
        ESP_LOGI(TAG, "HTTP GET request failed: %s", esp_err_to_name(err));
    }
    esp_http_client_cleanup(stt_client);
    //free(url);
}


//stt语音识别
// stt语音识别
esp_err_t app_http_baidu_speech_recognition_event_handler(esp_http_client_event_t *evt)
{
    ESP_LOGI(TAG, "app_http_baidu_speech_recognition_event_handler start----");

    if (evt->event_id == HTTP_EVENT_ON_DATA) {
        if (evt->data == NULL) {
            ESP_LOGE(TAG, "evt->data is NULL, skipping processing");
            return ESP_FAIL;
        }

        ESP_LOGI(TAG, "%.*s", evt->data_len, (char *)evt->data);

        // 使用 malloc 动态分配内存
        esp_http_client_event_t *evt_t = (esp_http_client_event_t *)malloc(sizeof(esp_http_client_event_t));
        if (evt_t == NULL) {
            ESP_LOGE(TAG, "Memory allocation failed for evt_t");
            return ESP_FAIL;
        }

        memset(evt_t, 0, sizeof(esp_http_client_event_t));  // 避免野指针
        evt_t->data = strdup((char *)evt->data);  // 复制数据
        if (evt_t->data == NULL) {
            ESP_LOGE(TAG, "Memory allocation failed for evt_t->data");
            free(evt_t);  // 释放 evt_t
            return ESP_FAIL;
        }

        // 发送事件到队列
        if (xQueueSend(http_speech_to_text_event_queue, evt_t, pdMS_TO_TICKS(100)) != pdPASS) {
            ESP_LOGE(TAG, "Failed to send event to queue");
            free(evt_t->data);  // 释放 evt_t->data
            free(evt_t);  // 释放 evt_t
            return ESP_FAIL;
        }
    }

    ESP_LOGI(TAG, "app_http_baidu_speech_recognition_event_handler end----");
    return ESP_OK;
}



void http_speech_to_text(const char* wav_raw_buffer, size_t wav_file_size) {
    ESP_LOGI(TAG, "http_speech_to_text start----");
    // **初始化 HTTP 客户端**
    esp_http_client_config_t config = {
        .method = HTTP_METHOD_POST,
        .event_handler = app_http_baidu_speech_recognition_event_handler,
        .buffer_size = 4 * 1024,
    };
    char *url = heap_caps_malloc(strlen(url_formate) + strlen(access_token) + 1, MALLOC_CAP_SPIRAM);
    sprintf(url, url_formate, access_token);
    config.url = url;
    
    stt_client = esp_http_client_init(&config);
    if (!stt_client) {
        ESP_LOGE(TAG, "Failed to initialize HTTP client");
        return ;
    }
    esp_http_client_set_method(stt_client, HTTP_METHOD_POST);
    esp_http_client_set_header(stt_client, "Content-Type", "audio/pcm;rate=16000");
    esp_http_client_set_header(stt_client, "Accept", "application/json");
    esp_http_client_set_post_field(stt_client, wav_raw_buffer, wav_file_size);

    // **发送 HTTP 请求**
    esp_err_t err = esp_http_client_perform(stt_client);
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "HTTP GET Status = %d, content_length = %d", esp_http_client_get_status_code(stt_client), (int)esp_http_client_get_content_length(stt_client));
    } else {
        ESP_LOGI(TAG, "HTTP GET request failed: %s", esp_err_to_name(err));
    }

    esp_http_client_cleanup(stt_client);

    free(url);
    ESP_LOGI(TAG, "http_speech_to_text end----");
}

esp_err_t app_http_baidu_tts_event_handler(esp_http_client_event_t *evt)
{
    if (evt->event_id == HTTP_EVENT_ON_DATA) {
        ESP_LOGI(TAG, "Received length:%d", evt->data_len);
        i2s_channel_write(tx_handle, (char *)evt->data, evt->data_len, NULL, 100);
    }

    return ESP_OK;
}
//tts文字转语音
void http_tts_dialogue(void* arg)
{
    while (1)
    {    
        esp_http_client_event_t *evt;
        if (xQueueReceive(http_ai_dialogue_event_queue, &evt, portMAX_DELAY) == pdPASS)
        {
            // Http
            esp_http_client_config_t config = {
                .method = HTTP_METHOD_POST,
                .event_handler = app_http_baidu_tts_event_handler,
                .buffer_size = 10 * 1024,
            };

            config.url = tts_url;
            stt_client = esp_http_client_init(&config);
            esp_http_client_set_method(stt_client, HTTP_METHOD_POST);
            esp_http_client_set_header(stt_client, "Content-Type", "application/x-www-form-urlencoded");
            esp_http_client_set_header(stt_client, "Accept", "*/*");

            url_encode((unsigned char *)evt->data, strlen(evt->data), &text_url_encode_size, NULL, 0);

            ESP_LOGI(TAG, "text size after url:%zu", text_url_encode_size);

            char *text_url_encode = heap_caps_calloc(1, text_url_encode_size + 1, MALLOC_CAP_SPIRAM);

            if (text_url_encode == NULL) {
                ESP_LOGI(TAG, "Malloc text url encode failed");
            }

            url_encode((unsigned char *)evt->data, strlen(evt->data), &text_url_encode_size, (unsigned char *)text_url_encode, text_url_encode_size + 1);

            char *payload = heap_caps_calloc(1, strlen(formate) + strlen(access_token) + strlen(text_url_encode) + 1, MALLOC_CAP_SPIRAM);

            if (payload == NULL) {
                free(text_url_encode);
                ESP_LOGI(TAG, "Malloc payload failed");
            }

            sprintf(payload, formate, text_url_encode, access_token);
            esp_http_client_set_post_field(stt_client, payload, strlen(payload));

            esp_err_t err = esp_http_client_perform(stt_client);
            if (err == ESP_OK) {
                ESP_LOGI(TAG, "HTTP GET Status = %d, content_length = %d", esp_http_client_get_status_code(stt_client), (int)esp_http_client_get_content_length(stt_client));
            } else {
                ESP_LOGI(TAG, "HTTP GET request failed: %s", esp_err_to_name(err));
            }
            esp_http_client_cleanup(stt_client);

            free(text_url_encode);
            free(payload);
            int value = 1;
            xQueueSend(read_start, &value, 0);
        }
        vTaskDelay(pdMS_TO_TICKS(1000)); // 延时 1 秒
    }
    
 
}






void http_init(){
    http_speech_to_text_event_queue = xQueueCreate(10, 1024);  
    http_ai_dialogue_event_queue = xQueueCreate(10, 1024);  
    http_tts_event_queue = xQueueCreate(10, 1024); 
    xTaskCreate(app_ask_tongyi_task, "ask tongyi",  4 * 2048, NULL, 10, NULL);
    xTaskCreate(http_tts_dialogue, "http_tts_dialogue", 4* 2048, NULL, 10, NULL);
}