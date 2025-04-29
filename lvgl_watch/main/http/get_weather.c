#include "get_weather.h"
#include <stdio.h>
#include "esp_wifi.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "esp_log.h"
#include "esp_http_client.h"
#include "cJSON.h"


#define WEATHER_URL "https://api.seniverse.com/v3/weather/now.json?key=S-Yt1LQ8hD3SNR3RA&location=浙江&language=zh-Hans&unit=c"
static const char *TAG = "weather";

// HTTP 客户端事件处理函数
esp_err_t _http_event_handler(esp_http_client_event_t *evt) {
    return ESP_OK;
}

// 获取天气数据
void get_weather_from_seniverse() {
    esp_http_client_config_t config = {
        .url = WEATHER_URL,
        .event_handler = _http_event_handler,
        .transport_type = HTTP_TRANSPORT_OVER_SSL,  // 使用 HTTPS
        .cert_pem = NULL,                           // 不验证证书（开发时）
        .skip_cert_common_name_check = true         // 跳过域名验证（开发时）
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_err_t err = esp_http_client_perform(client);  // 执行 HTTP 请求

    if (err == ESP_OK) {
        int content_length = esp_http_client_get_content_length(client);
        char *buffer = malloc(content_length + 1);
        esp_http_client_read(client, buffer, content_length);
        buffer[content_length] = 0;

        // 解析 JSON 响应
        cJSON *root = cJSON_Parse(buffer);
        cJSON *results = cJSON_GetObjectItem(root, "results");
        cJSON *location = cJSON_GetArrayItem(results, 0);
        cJSON *now = cJSON_GetObjectItem(location, "now");

        const char *weather = cJSON_GetObjectItem(now, "text")->valuestring;
        const char *temperature = cJSON_GetObjectItem(now, "temperature")->valuestring;
        const char *humidity = cJSON_GetObjectItem(now, "humidity")->valuestring;

        ESP_LOGI(TAG, "当前天气: %s", weather);
        ESP_LOGI(TAG, "当前温度: %s°C", temperature);
        ESP_LOGI(TAG, "湿度: %s%%", humidity);

        // 清理
        cJSON_Delete(root);
        free(buffer);
    } else {
        ESP_LOGE(TAG, "HTTP request failed");
    }

    esp_http_client_cleanup(client);
}