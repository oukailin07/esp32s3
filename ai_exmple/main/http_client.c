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
extern QueueHandle_t read_start;

/*
{
    "corpus_no":"6433214037620997779",
    "err_msg":"success.",
    "err_no":0,
    "result":["北京科技馆，"],
    "sn":"371191073711497849365"
}
*/

static char *cjson_stt_info(char *text) {
    // 解析 JSON 字符串
    cJSON *root = cJSON_Parse(text);
    if (root == NULL) {
        ESP_LOGE(TAG, "Error parsing JSON: %s", cJSON_GetErrorPtr());
        return NULL;
    }

    // 获取 "result" 字段
    cJSON *result_array = cJSON_GetObjectItem(root, "result");
    if (result_array == NULL || !cJSON_IsArray(result_array)) {
        ESP_LOGE(TAG, "Error: 'result' field not found or is not an array");
        cJSON_Delete(root);
        return NULL;
    }

    // 获取数组中的第一个元素
    cJSON *first_result = cJSON_GetArrayItem(result_array, 0);
    if (first_result == NULL || !cJSON_IsString(first_result)) {
        ESP_LOGE(TAG, "Error: 'result' array is empty or first element is not a string");
        cJSON_Delete(root);
        return NULL;
    }

    // 提取字符串内容
    char *result_str = strdup(first_result->valuestring); // 复制字符串
    if (result_str == NULL) {
        ESP_LOGE(TAG, "Error: Failed to allocate memory for result string");
        cJSON_Delete(root);
        return NULL;
    }

    // 释放 cJSON 对象
    cJSON_Delete(root);

    return result_str; // 返回解析出的字符串
}


/*
{
    "output":
    {
        "choices":
        [
            {
                "finish_reason":"stop","message":
                {
                    "role":"assistant","content":"没关系，你可以告诉我你遇到了什么问题或者困惑，我会尽力帮助你！ \uD83D\uDE0A"
                }
            }
        ]
    },
                "usage":
                {
                    "total_tokens":40,"output_tokens":18,"input_tokens":22
                },
                    "request_id":"68d1a4c6-fbcf-9a44-8b64-21b87eb763d1"
}
*/
    static char *cjson_tongyi_info(char *text) {
        // 解析 JSON 字符串
        cJSON *root = cJSON_Parse(text);
        if (root == NULL) {
            printf("Error parsing JSON: %s\n", cJSON_GetErrorPtr());
            return NULL;
        }
    
        // 获取 "output" 对象
        cJSON *output_object = cJSON_GetObjectItem(root, "output");
        if (output_object == NULL || !cJSON_IsObject(output_object)) {
            printf("Error: 'output' field not found or is not an object\n");
            cJSON_Delete(root);
            return NULL;
        }
    
        // 获取 "choices" 数组
        cJSON *choices_array = cJSON_GetObjectItem(output_object, "choices");
        if (choices_array == NULL || !cJSON_IsArray(choices_array)) {
            printf("Error: 'choices' field not found or is not an array\n");
            cJSON_Delete(root);
            return NULL;
        }
    
        // 获取数组中的第一个元素
        cJSON *first_choice = cJSON_GetArrayItem(choices_array, 0);
        if (first_choice == NULL) {
            printf("Error: 'choices' array is empty\n");
            cJSON_Delete(root);
            return NULL;
        }
    
        // 获取 "message" 对象
        cJSON *message_object = cJSON_GetObjectItem(first_choice, "message");
        if (message_object == NULL || !cJSON_IsObject(message_object)) {
            printf("Error: 'message' field not found or is not an object\n");
            cJSON_Delete(root);
            return NULL;
        }
    
        // 获取 "content" 字段
        cJSON *content_item = cJSON_GetObjectItem(message_object, "content");
        if (content_item == NULL || !cJSON_IsString(content_item)) {
            printf("Error: 'content' field not found or is not a string\n");
            cJSON_Delete(root);
            return NULL;
        }
    
        // 提取字符串内容
        char *content_str = strdup(content_item->valuestring); // 复制字符串
        if (content_str == NULL) {
            printf("Error: Failed to allocate memory for content string\n");
            cJSON_Delete(root);
            return NULL;
        }
    
        // 释放 cJSON 对象
        cJSON_Delete(root);
    
        return content_str; // 返回解析出的字符串
    }



esp_err_t app_http_tongyi_event_handler(esp_http_client_event_t *evt)
{
    if (evt->event_id == HTTP_EVENT_ON_DATA) {
        ESP_LOGI(TAG, "app_http_tongyi_event_handler start----");
        ESP_LOGI(TAG, "%.*s", evt->data_len, (char *)evt->data);

        char *tongyi_data_json = strdup((char *)evt->data);   // 复制 evt.data，防止指针无效
        if (tongyi_data_json == NULL) {
            ESP_LOGE(TAG, "Memory allocation failed for evt_t->data");
            free(tongyi_data_json);  // 释放 evt_t
            return ESP_FAIL;
        }
        if (xQueueSend(http_ai_dialogue_event_queue, &tongyi_data_json, pdMS_TO_TICKS(100)) != pdPASS) {
            ESP_LOGE(TAG, "Failed to send event to queue");
            free(tongyi_data_json);  // 释放内存
            return ESP_FAIL;
        }
        
        ESP_LOGI(TAG, "app_http_tongyi_event_handler start----");
    }
    
    return ESP_OK;
}



void app_ask_tongyi_task(void *pvParameters) {
    while (1) {
        ESP_LOGI(TAG, "app_ask_tongyi_task start----");
        char *stt_data_cjson = NULL;
        char *stt_data = NULL;

        // 检查堆栈使用情况
        ESP_LOGI(TAG, "app_ask_tongyi_task stack free: %d", uxTaskGetStackHighWaterMark(NULL));

        if (xQueueReceive(http_speech_to_text_event_queue, &stt_data_cjson, portMAX_DELAY) == pdPASS) {
            ESP_LOGI(TAG, "app_ask_tongyi_task start2----");
            if (stt_data_cjson == NULL) {
                ESP_LOGE(TAG, "stt_data_cjson is NULL, skipping...");
                continue;
            }

            // 解析数据
            stt_data = cjson_stt_info(stt_data_cjson);
            if (stt_data == NULL || strlen(stt_data) == 0) {
                ESP_LOGE(TAG, "stt_data is NULL or empty, skipping...");
                int read_start_flag = 1;
                xQueueSend(read_start, &read_start_flag, 0);
                free(stt_data_cjson);
                continue;
            }

            ESP_LOGI(TAG, "app_ask_tongyi_task stt_data: %s", stt_data);

            char *data = heap_caps_calloc(1, strlen(tongyiqianwen_format) + strlen(stt_data) + 1, MALLOC_CAP_SPIRAM);
            if (data == NULL) {
                ESP_LOGE(TAG, "Failed to allocate memory for data");
                free(stt_data_cjson);
                free(stt_data);
                continue;
            }

            sprintf(data, tongyiqianwen_format, stt_data);
            ESP_LOGI(TAG, "app_ask_tongyi_task data: %s", data);
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

        if (stt_data_cjson != NULL) {
            free(stt_data_cjson);
        }
        if (stt_data != NULL) {
            free(stt_data);
        }

        // 检查堆内存完整性
        heap_caps_check_integrity_all(true);
        ESP_LOGI(TAG, "app_ask_tongyi_task end----");
        vTaskDelay(pdMS_TO_TICKS(1000)); // 延时 1 秒
    }
}

esp_err_t app_http_baidu_access_token_event_handler(esp_http_client_event_t *evt)
{
    if (evt->event_id == HTTP_EVENT_ON_DATA) {
        ESP_LOGI(TAG, "app_http_baidu_access_token_event_handler start----");
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
        ESP_LOGI(TAG, "app_http_baidu_access_token_event_handler end----");
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
esp_err_t app_http_baidu_speech_recognition_event_handler(esp_http_client_event_t *evt) {
    if (evt->event_id == HTTP_EVENT_ON_DATA) {
        ESP_LOGI(TAG, "app_http_baidu_speech_recognition_event_handler start----");

        // 检查 evt->data 是否有效
        if (evt->data == NULL) {
            ESP_LOGE(TAG, "evt->data is NULL, skipping processing");
            return ESP_FAIL;
        }

        // 打印接收到的数据
        ESP_LOGI(TAG, "stt语音识别内容 %.*s", evt->data_len, (char *)evt->data);

        // 使用 strdup 复制数据
        char *stt_data_cjson = strdup((char *)evt->data);
        if (stt_data_cjson == NULL) {
            ESP_LOGE(TAG, "Memory allocation failed for stt_data_cjson");
            return ESP_FAIL;
        }

        // 发送事件到队列
        if (xQueueSend(http_speech_to_text_event_queue, &stt_data_cjson, pdMS_TO_TICKS(100)) != pdPASS) {
            ESP_LOGE(TAG, "Failed to send event to queue");
            free(stt_data_cjson);  // 释放内存
            return ESP_FAIL;
        }

        ESP_LOGI(TAG, "app_http_baidu_speech_recognition_event_handler end----");
    }

    return ESP_OK;
}


void http_speech_to_text(char* wav_raw_buffer, size_t wav_file_size) {
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
    if(wav_raw_buffer != NULL)
    {
        free(wav_raw_buffer);
    }

    free(url);
    ESP_LOGI(TAG, "http_speech_to_text end----");
}

esp_err_t app_http_baidu_tts_event_handler(esp_http_client_event_t *evt)
{
    if (evt->event_id == HTTP_EVENT_ON_DATA) {
        ESP_LOGI(TAG, "Received length:%d", evt->data_len);
        //i2s_channel_write(tx_handle, (char *)evt->data, evt->data_len, NULL, 100);
    }

    return ESP_OK;
}

/*错误代码
{"err_no":500,"err_msg":"notsupport.","sn":"abcdefgh","idx":1}
*/



//tts文字转语音
void http_tts_dialogue(void* arg)
{
    while (1)
    {    ESP_LOGI(TAG, "http_tts_dialogue start----");
        char *tongyi_data_json = NULL;
        char *tongyi_data = NULL;
        if (xQueueReceive(http_ai_dialogue_event_queue, &tongyi_data_json, portMAX_DELAY) == pdPASS)
        {
            if(tongyi_data_json == NULL)
            {
                ESP_LOGI(TAG, "http_tts_dialogue:tongyi_data_json == NULL");
                continue;
            }
            else
            {
                tongyi_data = cjson_tongyi_info(tongyi_data_json);
                ESP_LOGI(TAG, "tongyi_data:%s",tongyi_data);
            }
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

            url_encode((unsigned char *)tongyi_data, strlen(tongyi_data), &text_url_encode_size, NULL, 0);

            ESP_LOGI(TAG, "text size after url:%zu", text_url_encode_size);

            char *text_url_encode = heap_caps_calloc(1, text_url_encode_size + 1, MALLOC_CAP_SPIRAM);

            if (text_url_encode == NULL) {
                ESP_LOGI(TAG, "Malloc text url encode failed");
            }

            url_encode((unsigned char *)tongyi_data, strlen(tongyi_data), &text_url_encode_size, (unsigned char *)text_url_encode, text_url_encode_size + 1);

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
        if(tongyi_data_json != NULL)
        {
            free(tongyi_data_json);
        }
        if(tongyi_data != NULL)
        {
            free(tongyi_data);
        }
        ESP_LOGI(TAG, "http_tts_dialogue end----");
        vTaskDelay(pdMS_TO_TICKS(100)); // 延时 1 秒
    }
}






void http_init(){
    http_speech_to_text_event_queue = xQueueCreate(10, sizeof(char *));  
    http_ai_dialogue_event_queue = xQueueCreate(10, sizeof(char *));
    xTaskCreate(app_ask_tongyi_task, "ask tongyi",  4 * 2048, NULL, 5, NULL);
    xTaskCreate(http_tts_dialogue, "http_tts_dialogue", 4* 2048, NULL, 5, NULL);
}