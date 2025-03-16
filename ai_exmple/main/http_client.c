#include "http_client.h"
#include "esp_http_client.h"
#include "esp_log.h"
#include "cJSON.h"
#include "esp_crt_bundle.h"  // 包含根证书包
#define TAG "HTTP"

// 获取百度智能云的Access Token
char* get_baidu_access_token(void)
{
    char url[256];
    snprintf(url, sizeof(url), "https://aip.baidubce.com/oauth/2.0/token?grant_type=client_credentials&client_id=%s&client_secret=%s",
             BAIDU_API_KEY, BAIDU_SECRET_KEY);

    esp_http_client_config_t config = {
        .url = url,
        .method = HTTP_METHOD_POST,
        .crt_bundle_attach = esp_crt_bundle_attach,  // 启用根证书验证
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);

    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        int status_code = esp_http_client_get_status_code(client);
        if (status_code == 200) {
            char* response_body = malloc(esp_http_client_get_content_length(client));
            esp_http_client_read(client, response_body, esp_http_client_get_content_length(client));

            cJSON* response = cJSON_Parse(response_body);
            char* access_token = strdup(cJSON_GetObjectItem(response, "access_token")->valuestring);
            cJSON_Delete(response);
            free(response_body);
            return access_token;
        }
    }
    esp_http_client_cleanup(client);
    return NULL;
}

// 语音识别（百度智能云）
char* http_speech_to_text(const uint8_t* audio_data, size_t audio_length)
{
    //char* access_token = get_baidu_access_token();
    //if (access_token == NULL) {
    //    ESP_LOGE(TAG, "Failed to get Baidu access token");
    //    return NULL;
    //}
    char url[256];
    snprintf(url, sizeof(url), "https://aip.baidubce.com/oauth/2.0/token?grant_type=client_credentials&client_id=%s&client_secret=%s",
             BAIDU_API_KEY, BAIDU_SECRET_KEY);

    esp_http_client_config_t config = {
        .url = url,
        .method = HTTP_METHOD_POST,
        .crt_bundle_attach = esp_crt_bundle_attach,  // 启用根证书验证
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);
    if (client == NULL) {
        ESP_LOGE(TAG, "Failed to initialize HTTP client");
        return NULL;
    }

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

            cJSON* recognized_text_json = cJSON_GetObjectItem(response, "result");
            if (recognized_text_json == NULL) {
                ESP_LOGE(TAG, "Failed to get recognized text from JSON response");
                cJSON_Delete(response);
                free(response_body);
                esp_http_client_cleanup(client);
                return NULL;
            }

            char* recognized_text = strdup(recognized_text_json->valuestring); // 复制识别结果
            cJSON_Delete(response);
            free(response_body);
            esp_http_client_cleanup(client);
            return recognized_text;
        } else {
            ESP_LOGE(TAG, "HTTP request failed, status code: %d", status_code);
        }
    } else {
        ESP_LOGE(TAG, "HTTP request failed, error: %s", esp_err_to_name(err));
    }

    esp_http_client_cleanup(client);
    return NULL;
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