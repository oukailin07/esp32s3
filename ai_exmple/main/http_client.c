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
#include "ds_inmp441.h"
#include "ds_max98357.h"
#define TAG "HTTP"
#include "mbedtls/base64.h"  // 需要启用 mbedtls
#include "esp_log.h"
#include "esp_http_client.h"
#include "cJSON.h"
#include <stdlib.h>
#include <string.h>

// 获取百度智能云的Access Token
char* get_baidu_access_token(void) {
    char url[512];
    snprintf(url, sizeof(url),
             "https://aip.baidubce.com/oauth/2.0/token?grant_type=client_credentials&client_id=%s&client_secret=%s",
             BAIDU_API_KEY, BAIDU_SECRET_KEY);

    esp_http_client_config_t config = {
        .url = url,
        .method = HTTP_METHOD_POST,
        .crt_bundle_attach = esp_crt_bundle_attach,
        .timeout_ms = 5000,
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);
    if (!client) {
        ESP_LOGE(TAG, "Failed to init HTTP client");
        return NULL;
    }

    esp_err_t err = esp_http_client_perform(client);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "HTTP request failed: %s", esp_err_to_name(err));
        esp_http_client_cleanup(client);
        return NULL;
    }

    int content_length = esp_http_client_get_content_length(client);
    if (content_length <= 0) {
        ESP_LOGE(TAG, "Invalid content length: %d", content_length);
        esp_http_client_cleanup(client);
        return NULL;
    }

    char* response_body = malloc(content_length + 1);
    if (!response_body) {
        ESP_LOGE(TAG, "Memory allocation failed");
        esp_http_client_cleanup(client);
        return NULL;
    }
    esp_http_client_read_response(client, response_body, content_length);
    response_body[content_length] = '\0';
    ESP_LOGI(TAG, "HTTP Response: %s", response_body);

    cJSON* response = cJSON_Parse(response_body);
    free(response_body);
    esp_http_client_cleanup(client);
    
    if (!response) {
        ESP_LOGE(TAG, "Failed to parse JSON");
        return NULL;
    }

    cJSON* access_token_item = cJSON_GetObjectItem(response, "access_token");
    if (!cJSON_IsString(access_token_item)) {
        ESP_LOGE(TAG, "JSON does not contain valid access_token");
        cJSON_Delete(response);
        return NULL;
    }

    char* access_token = strdup(access_token_item->valuestring);
    cJSON_Delete(response);
    ESP_LOGI(TAG, "Access Token: %s", access_token);
    return access_token;
}



#define TAG "HTTP"

char* base64_encode(const uint8_t* input, size_t input_len, size_t* output_len) {
    if (!input || input_len == 0) {
        ESP_LOGE(TAG, "Invalid input data for Base64 encoding");
        return NULL;
    }

    size_t encoded_len = ((input_len + 2) / 3) * 4 + 1;  // 计算 Base64 编码长度
    char* encoded_data = (char*)malloc(encoded_len);
    if (!encoded_data) {
        ESP_LOGE(TAG, "Memory allocation failed for Base64 buffer");
        return NULL;
    }

    int ret = mbedtls_base64_encode((unsigned char*)encoded_data, encoded_len, output_len, input, input_len);
    if (ret == MBEDTLS_ERR_BASE64_BUFFER_TOO_SMALL) {
        ESP_LOGE(TAG, "Base64 buffer too small! Required: %d, Allocated: %d", (int)(*output_len), (int)encoded_len);
        free(encoded_data);
        return NULL;
    } else if (ret != 0) {
        ESP_LOGE(TAG, "Base64 encoding failed, error code: %d", ret);
        free(encoded_data);
        return NULL;
    }

    // **替换 `+` 和 `/` 为 URL-safe Base64**
    for (size_t i = 0; i < *output_len; i++) {
        if (encoded_data[i] == '+') encoded_data[i] = '-';
        else if (encoded_data[i] == '/') encoded_data[i] = '_';
    }

    encoded_data[*output_len] = '\0';
    return encoded_data;
}



char* http_speech_to_text(const uint8_t* audio_data, size_t audio_length) {
    char url[512];
    snprintf(url, sizeof(url),
             "https://aip.baidubce.com/rest/2.0/speech/v2/asr?access_token=24.0c10b25b8cb7f7167f1fe3aab0bd3bd1.2592000.1744720185.282335-118065803");

    // **进行 Base64 编码**
    size_t base64_length;
    char* base64_audio = base64_encode(audio_data, audio_length, &base64_length);
    if (!base64_audio) {
        ESP_LOGE(TAG, "Failed to encode audio to Base64");
        return NULL;
    }

    // **构建 JSON 请求体**
    char* json_body = (char*)malloc(512 + base64_length);  // 预留足够空间
    if (!json_body) {
        ESP_LOGE(TAG, "Memory allocation failed for JSON body");
        free(base64_audio);
        return NULL;
    }

    snprintf(json_body, 512 + base64_length,
             "{\"format\":\"pcm\",\"rate\":16000,\"channel\":1,\"cuid\":\"esp32s3\","
             "\"token\":\"24.0c10b25b8cb7f7167f1fe3aab0bd3bd1.2592000.1744720185.282335-118065803\","
             "\"len\":%d,\"speech\":\"%s\"}",
             (int)audio_length, base64_audio);

    free(base64_audio);  // 释放 Base64 缓存

    //ESP_LOGI(TAG, "请求体: %s", json_body);

    // **初始化 HTTP 客户端**
    esp_http_client_config_t config = {
        .url = url,
        .method = HTTP_METHOD_POST,
        .timeout_ms = 10000,
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);
    if (!client) {
        ESP_LOGE(TAG, "Failed to initialize HTTP client");
        free(json_body);
        return NULL;
    }

    esp_http_client_set_header(client, "Content-Type", "application/json");
    esp_http_client_set_header(client, "Accept", "application/json");
    esp_http_client_set_post_field(client, json_body, strlen(json_body));

    // **发送 HTTP 请求**
    esp_err_t err = esp_http_client_perform(client);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "HTTP request failed: %s", esp_err_to_name(err));
        free(json_body);
        esp_http_client_cleanup(client);
        return NULL;
    }

    // **读取响应**
    int content_length = esp_http_client_get_content_length(client);
    if (content_length <= 0) {
        ESP_LOGE(TAG, "Invalid content length");
        free(json_body);
        esp_http_client_cleanup(client);
        return NULL;
    }

    char* response_body = malloc(content_length + 1);
    if (!response_body) {
        ESP_LOGE(TAG, "Memory allocation failed");
        free(json_body);
        esp_http_client_cleanup(client);
        return NULL;
    }

    int read_len = esp_http_client_read(client, response_body, content_length);
    //if (read_len > 0) {

   // }
    response_body[content_length] = '\0';
    printf("HTTP Response: %s\n", response_body);
    //response_body[content_length] = '\0';  // 添加 Null 终止符
    
    free(json_body);
    esp_http_client_cleanup(client);

    // **解析 JSON 响应**
    cJSON* response = cJSON_Parse(response_body);
    free(response_body);
    if (!response) {
        ESP_LOGE(TAG, "Failed to parse JSON");
        return NULL;
    }

    cJSON* result_item = cJSON_GetObjectItem(response, "result");
    if (!cJSON_IsArray(result_item) || cJSON_GetArraySize(result_item) == 0) {
        ESP_LOGE(TAG, "Invalid JSON response format");
        cJSON_Delete(response);
        return NULL;
    }

    cJSON* first_result = cJSON_GetArrayItem(result_item, 0);
    char* recognized_text = strdup(first_result->valuestring);
    cJSON_Delete(response);

    return recognized_text;
}



// AI对话（MiniMax）
char* http_ai_dialogue(const char* text)
{
    cJSON* request = cJSON_CreateObject();
    cJSON_AddStringToObject(request, "prompt", text); // 输入文本
    cJSON_AddNumberToObject(request, "max_tokens", 50); // 最大生成token数

    char* request_body = cJSON_Print(request);
    cJSON_Delete(request);

    esp_http_client_config_t config = {
        .url = MINIMAX_API_URL,
        .method = HTTP_METHOD_POST,
        //.event_handler = _http_ai_dialogue,//注册时间回调
        .skip_cert_common_name_check = true,
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);
    if (client == NULL) {
        ESP_LOGE(TAG, "Failed to initialize HTTP client");
        return NULL;
    }

    esp_http_client_set_header(client, "Content-Type", "application/json");
    esp_http_client_set_header(client, "Authorization", MINIMAX_API_KEY);

    esp_http_client_set_post_field(client, request_body, strlen(request_body));

    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        int status_code = esp_http_client_get_status_code(client);
        if (status_code == 200) {
            int content_length = esp_http_client_get_content_length(client);
            if (content_length <= 0) {
                ESP_LOGE(TAG, "Invalid content length: %d", content_length);
                esp_http_client_cleanup(client);
                return NULL;
            }

            char* response_body = malloc(content_length + 1); // 分配内存
            if (response_body == NULL) {
                ESP_LOGE(TAG, "Failed to allocate memory for response body");
                esp_http_client_cleanup(client);
                return NULL;
            }

            esp_http_client_read(client, response_body, content_length);
            response_body[content_length] = '\0'; // 添加字符串结束符

            cJSON* response = cJSON_Parse(response_body);
            if (response == NULL) {
                ESP_LOGE(TAG, "Failed to parse JSON response");
                free(response_body);
                esp_http_client_cleanup(client);
                return NULL;
            }

            cJSON* ai_response_json = cJSON_GetObjectItem(response, "text");
            if (ai_response_json == NULL) {
                ESP_LOGE(TAG, "Failed to get AI response from JSON response");
                cJSON_Delete(response);
                free(response_body);
                esp_http_client_cleanup(client);
                return NULL;
            }

            char* ai_response = strdup(ai_response_json->valuestring); // 复制AI响应
            cJSON_Delete(response);
            free(response_body);
            esp_http_client_cleanup(client);
            return ai_response;
        } else {
            ESP_LOGE(TAG, "HTTP request failed, status code: %d", status_code);
        }
    } else {
        ESP_LOGE(TAG, "HTTP request failed, error: %s", esp_err_to_name(err));
    }

    esp_http_client_cleanup(client);
    return NULL;
}
