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

#define WEATHER_CODE_DAY  \
    {"1": "Sunny", "2": "Partly cloudy", "3": "Cloudy", "4": "Overcast", \
     "5": "Light rain", "6": "Moderate rain", "7": "Heavy rain", "8": "Thunderstorm", \
     "9": "Foggy", "13": "Light snow", "14": "Heavy snow", "15": "Windy"}

char local_response_buffer[MAX_BUFFER_SIZE] = {0}; 
// HTTP 客户端事件处理函数
esp_err_t _http_event_handler(esp_http_client_event_t *evt) {
    switch(evt->event_id) {
        case HTTP_EVENT_ON_DATA:
            if (evt->user_data) {
                char *buffer = (char *)evt->user_data;
                size_t current_len = strnlen(buffer, MAX_BUFFER_SIZE);
                size_t remaining_space = MAX_BUFFER_SIZE - current_len - 1;

                if (remaining_space > 0) {
                    size_t copy_len = (evt->data_len < remaining_space) ? evt->data_len : remaining_space;
                    memcpy(buffer + current_len, evt->data, copy_len);
                    buffer[current_len + copy_len] = '\0';
                } else {
                    ESP_LOGE(TAG, "Buffer full!");
                }
            }
            break;
        default:
            break;
    }
    return ESP_OK;
}


 
void http_rest_with_url(void)
{
    esp_http_client_config_t config = {
        .url = "http://api.seniverse.com/v3/weather/daily.json?key=S-Yt1LQ8hD3SNR3RA&location=jinhua&language=en&unit=c&start=0&days=3",
        .method = HTTP_METHOD_GET,
        .event_handler = _http_event_handler,
        .user_data = local_response_buffer,        // Pass address of local buffer to get response
        .disable_auto_redirect = true,
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);

    // GET
    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "HTTP GET Status = %d, content_length = %lld",
                esp_http_client_get_status_code(client),
                esp_http_client_get_content_length(client));
    } else {
        ESP_LOGE(TAG, "HTTP GET request failed: %s", esp_err_to_name(err));
    }
    //ESP_LOG_BUFFER_HEX(TAG, local_response_buffer, strlen(local_response_buffer));

	ESP_LOGI(TAG, "recv data ---2 = %d %s\r\n", strlen(local_response_buffer), local_response_buffer); /*打印心知天气json原始数据*/
    if (strstr(local_response_buffer, "}") == NULL) {
        ESP_LOGE(TAG, "Incomplete JSON data");
        return ;
    }
    user_seniverse_config_t weather_data;
    if (parse_seniverse_weather(local_response_buffer, &weather_data)) {
        ESP_LOGI(TAG, "==== Weather Info ====");
        ESP_LOGI(TAG, "Location: %s, %s", weather_data.name, weather_data.country);
        ESP_LOGI(TAG, "Timezone: %s (offset %s)", weather_data.timezone, weather_data.timezone_offset);
        ESP_LOGI(TAG, "Last Update: %s", weather_data.last_update);
    
        for (int i = 0; i < 3; ++i) {
            user_seniverse_day_config_t *day = &weather_data.day_config[i];
            if (day->date == NULL) continue;
    
            ESP_LOGI(TAG, "--- Day %d ---", i + 1);
            ESP_LOGI(TAG, "Date: %s", day->date);
            ESP_LOGI(TAG, "Day Weather: %s (Code: %s)", day->text_day, day->code_day);
            ESP_LOGI(TAG, "Night Weather: %s (Code: %s)", day->text_night, day->code_night);
            ESP_LOGI(TAG, "High: %s°C, Low: %s°C", day->high, day->low);
            ESP_LOGI(TAG, "Humidity: %s%%", day->humidity);
            ESP_LOGI(TAG, "Rainfall: %s mm, Precip: %s%%", day->rainfall, day->precip);
            ESP_LOGI(TAG, "Wind: %s (%s°), %s km/h, Scale: %s", 
                day->wind_direction,
                day->wind_direction_degree,
                day->wind_speed,
                day->wind_scale);
        }
        print_weather_details(&weather_data);
        free_seniverse_config(&weather_data);
    } else {
        ESP_LOGE(TAG, "Failed to parse weather data");
    }
    
    esp_http_client_cleanup(client);
}


bool parse_seniverse_weather(const char *json_str, user_seniverse_config_t *out_config) {
    if (!json_str || !out_config) {
        ESP_LOGE(TAG, "Invalid arguments to parser");
        return false;
    }

    cJSON *root = cJSON_Parse(json_str);
    if (!root) {
        ESP_LOGE(TAG, "JSON parse failed");
        return false;
    }

    // 错误状态检查（如城市未找到）
    cJSON *status_code = cJSON_GetObjectItem(root, "status_code");
    if (status_code && cJSON_IsString(status_code)) {
        ESP_LOGE(TAG, "API error: %s", status_code->valuestring);
        cJSON_Delete(root);
        return false;
    }

    cJSON *results = cJSON_GetObjectItem(root, "results");
    if (!results || !cJSON_IsArray(results)) {
        ESP_LOGE(TAG, "Missing 'results'");
        cJSON_Delete(root);
        return false;
    }

    cJSON *result = cJSON_GetArrayItem(results, 0);
    if (!result) {
        ESP_LOGE(TAG, "No result[0] object");
        cJSON_Delete(root);
        return false;
    }

    // 读取 location
    cJSON *location = cJSON_GetObjectItem(result, "location");
    if (location) {
        #define COPY_STRING_FIELD(json_obj, field) do { \
            cJSON *item = cJSON_GetObjectItem(json_obj, #field); \
            out_config->field = (item && cJSON_IsString(item)) ? strdup(item->valuestring) : NULL; \
        } while (0)

        COPY_STRING_FIELD(location, id);
        COPY_STRING_FIELD(location, name);
        COPY_STRING_FIELD(location, country);
        COPY_STRING_FIELD(location, path);
        COPY_STRING_FIELD(location, timezone);
        COPY_STRING_FIELD(location, timezone_offset);
    }

    // 读取 last_update
    cJSON *last_update = cJSON_GetObjectItem(result, "last_update");
    out_config->last_update = (last_update && cJSON_IsString(last_update)) ? strdup(last_update->valuestring) : NULL;

    // 读取 daily 数组
    cJSON *daily = cJSON_GetObjectItem(result, "daily");
    if (!daily || !cJSON_IsArray(daily)) {
        ESP_LOGE(TAG, "Missing 'daily'");
        cJSON_Delete(root);
        return false;
    }

    int i;
    for (i = 0; i < cJSON_GetArraySize(daily) && i < 3; i++) {
        cJSON *day = cJSON_GetArrayItem(daily, i);
        user_seniverse_day_config_t *cfg = &out_config->day_config[i];

        #define COPY_DAY_FIELD(field) do { \
            cJSON *item = cJSON_GetObjectItem(day, #field); \
            cfg->field = (item && cJSON_IsString(item)) ? strdup(item->valuestring) : NULL; \
        } while (0)

        COPY_DAY_FIELD(date);
        COPY_DAY_FIELD(text_day);
        COPY_DAY_FIELD(code_day);
        COPY_DAY_FIELD(text_night);
        COPY_DAY_FIELD(code_night);
        COPY_DAY_FIELD(high);
        COPY_DAY_FIELD(low);
        COPY_DAY_FIELD(rainfall);
        COPY_DAY_FIELD(precip);
        COPY_DAY_FIELD(wind_direction);
        COPY_DAY_FIELD(wind_direction_degree);
        COPY_DAY_FIELD(wind_speed);
        COPY_DAY_FIELD(wind_scale);
        COPY_DAY_FIELD(humidity);

        ESP_LOGI(TAG, "[Day %d] %s | Day: %s | High: %s | Low: %s | Wind: %s",
                 i,
                 cfg->date ? cfg->date : "N/A",
                 cfg->text_day ? cfg->text_day : "N/A",
                 cfg->high ? cfg->high : "N/A",
                 cfg->low ? cfg->low : "N/A",
                 cfg->wind_direction ? cfg->wind_direction : "N/A");
    }

    cJSON_Delete(root);
    return true;
}

void free_seniverse_config(user_seniverse_config_t *cfg) {
    if (!cfg) return;

    // 释放 location 相关字段
    free(cfg->id);
    free(cfg->name);
    free(cfg->country);
    free(cfg->path);
    free(cfg->timezone);
    free(cfg->timezone_offset);

    // 释放 day_config 数组中的字段
    for (int i = 0; i < 3; i++) {
        free(cfg->day_config[i].date);
        free(cfg->day_config[i].text_day);
        free(cfg->day_config[i].code_day);
        free(cfg->day_config[i].text_night);
        free(cfg->day_config[i].code_night);
        free(cfg->day_config[i].high);
        free(cfg->day_config[i].low);
        free(cfg->day_config[i].rainfall);
        free(cfg->day_config[i].precip);
        free(cfg->day_config[i].wind_direction);
        free(cfg->day_config[i].wind_direction_degree);
        free(cfg->day_config[i].wind_speed);
        free(cfg->day_config[i].wind_scale);
        free(cfg->day_config[i].humidity);
    }

    // 释放 last_update
    free(cfg->last_update);
}




void print_weather_details(user_seniverse_config_t *weather_data) {
    ESP_LOGI(TAG, "==== Weather Info ====");
    ESP_LOGI(TAG, "Location: %s, %s", weather_data->name, weather_data->country);
    ESP_LOGI(TAG, "Timezone: %s (offset %s)", weather_data->timezone, weather_data->timezone_offset);
    ESP_LOGI(TAG, "Last Update: %s", weather_data->last_update);

    for (int i = 0; i < 3; i++) {
        ESP_LOGI(TAG, "--- Day %d ---", i);
        ESP_LOGI(TAG, "Date: %s", weather_data->day_config[i].date);
        ESP_LOGI(TAG, "Day Weather: %s (Code: %s)",
                 weather_data->day_config[i].text_day,
                 weather_data->day_config[i].code_day);
        ESP_LOGI(TAG, "Night Weather: %s (Code: %s)",
                 weather_data->day_config[i].text_night,
                 weather_data->day_config[i].code_night);
        ESP_LOGI(TAG, "High: %s°C, Low: %s°C", weather_data->day_config[i].high, weather_data->day_config[i].low);
        ESP_LOGI(TAG, "Humidity: %s%%", weather_data->day_config[i].humidity);
        ESP_LOGI(TAG, "Rainfall: %s mm, Precip: %s%%", weather_data->day_config[i].rainfall, weather_data->day_config[i].precip);
        ESP_LOGI(TAG, "Wind: %s (%s°), %s km/h, Scale: %s", 
                 weather_data->day_config[i].wind_direction,
                 weather_data->day_config[i].wind_direction_degree,
                 weather_data->day_config[i].wind_speed,
                 weather_data->day_config[i].wind_scale);
    }
}