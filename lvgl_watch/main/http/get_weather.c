#include "get_weather.h"
#include <stdio.h>
#include "esp_wifi.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "esp_log.h"
#include "esp_http_client.h"
#include "cJSON.h"

#define WEATHER_URL "http://api.seniverse.com/v3/weather/daily.json?key=S-Yt1LQ8hD3SNR3RA&location=jinhua&language=en&unit=c&start=0&days=3"//未来三天
#define WEATHER_URL_24H "http://api.seniverse.com/v3/weather/hourly.json?key=S-Yt1LQ8hD3SNR3RA&location=jinhua&language=zh-Hans&unit=c&start=0&hours=24"//24小时免费用户无权限
#define WEATHER_URL_NOW_H "http://api.seniverse.com/v3/weather/now.json?key=S-Yt1LQ8hD3SNR3RA&location=jinhua&language=zh-Hans&unit=c"//实况
static const char *TAG = "weather";

#define WEATHER_CODE_DAY  \
    {"1": "Sunny", "2": "Partly cloudy", "3": "Cloudy", "4": "Overcast", \
     "5": "Light rain", "6": "Moderate rain", "7": "Heavy rain", "8": "Thunderstorm", \
     "9": "Foggy", "13": "Light snow", "14": "Heavy snow", "15": "Windy"}

WeatherNow now;
Weather24H forecast;

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
    memset(local_response_buffer, 0, sizeof(local_response_buffer));
    esp_http_client_config_t config = {
        .url = WEATHER_URL,
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
        ESP_LOGI(TAG, "Parsed weather data successfully");
        print_weather_details(&weather_data);
        free_seniverse_config(&weather_data);
    } else {
        ESP_LOGE(TAG, "Failed to parse weather data");
    }
    
    esp_http_client_cleanup(client);
}





/*
{
#define WEATHER_URL "http://api.seniverse.com/v3/weather/daily.json?key=S-Yt1LQ8hD3SNR3RA&location=jinhua&language=en&unit=c&start=0&days=3"
  "results": [{
    "location": {
      "id": "WX4FBXXFKE4F",
      "name": "北京",
      "country": "CN",
      "path": "北京,北京,中国",
      "timezone": "Asia/Shanghai",
      "timezone_offset": "+08:00"
    },
    "daily": [{                          //返回指定days天数的结果
      "date": "2015-09-20",              //日期
      "text_day": "多云",                //白天天气现象文字
      "code_day": "4",                  //白天天气现象代码
      "text_night": "晴",               //晚间天气现象文字
      "code_night": "0",                //晚间天气现象代码
      "high": "26",                     //当天最高温度
      "low": "17",                      //当天最低温度
      "precip": "0",                    //降水概率，范围0~100，单位百分比（目前仅支持国外城市）
      "wind_direction": "",             //风向文字
      "wind_direction_degree": "255",   //风向角度，范围0~360
      "wind_speed": "9.66",             //风速，单位km/h（当unit=c时）、mph（当unit=f时）
      "wind_scale": "",                 //风力等级
      "rainfall": "0.0",                //降水量，单位mm
      "humidity": "76"                  //相对湿度，0~100，单位为百分比
    }, {
      "date": "2015-09-21",
      "text_day": "晴",
      "code_day": "0",
      "text_night": "晴",
      "code_night": "0",
      "high": "27",
      "low": "17",
      "precip": "0",
      "wind_direction": "",
      "wind_direction_degree": "157",
      "wind_speed": "17.7",
      "wind_scale": "3",
      "rainfall": "0.0",
      "humidity": "76"
    }, {
      ...                               //更多返回结果
    }],
    "last_update": "2015-09-20T18:00:00+08:00" //数据更新时间（该城市的本地时间）
  }]
}
*/

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

void http_get_weather_24h(void)
{
    memset(local_response_buffer, 0, sizeof(local_response_buffer));

    esp_http_client_config_t config = {
        .url = WEATHER_URL_24H,
        .method = HTTP_METHOD_GET,
        .event_handler = _http_event_handler,
        .user_data = local_response_buffer,
        .disable_auto_redirect = true,
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "24H Weather: Status = %d, content_length = %lld",
                 esp_http_client_get_status_code(client),
                 esp_http_client_get_content_length(client));
        ESP_LOGI(TAG, "Response: %s", local_response_buffer);

        // 你可以添加自己的解析函数 parse_weather_24h(local_response_buffer);
        if (parse_seniverse_weather_now(local_response_buffer, &now)) {
            ESP_LOGI(TAG, "当前城市: %s, 温度: %s°C, 天气: %s", now.city, now.temperature, now.text);
        }
    } else {
        ESP_LOGE(TAG, "24H Weather request failed: %s", esp_err_to_name(err));
    }
    esp_http_client_cleanup(client);
}

void http_get_weather_now(void)
{
    memset(local_response_buffer, 0, sizeof(local_response_buffer));

    esp_http_client_config_t config = {
        .url = WEATHER_URL_NOW_H,
        .method = HTTP_METHOD_GET,
        .event_handler = _http_event_handler,
        .user_data = local_response_buffer,
        .disable_auto_redirect = true,
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "Now Weather: Status = %d, content_length = %lld",
                 esp_http_client_get_status_code(client),
                 esp_http_client_get_content_length(client));
        ESP_LOGI(TAG, "Response: %s", local_response_buffer);
        if (parse_seniverse_weather_24h(local_response_buffer, &forecast)) {
            for (int i = 0; i < forecast.count; i++) {
                ESP_LOGI(TAG, "第%d小时: %s %s°C %s", i,
                         forecast.hourly[i].time,
                         forecast.hourly[i].temperature,
                         forecast.hourly[i].text);
            }
        }
        // 你可以添加自己的解析函数 parse_weather_now(local_response_buffer);
    } else {
        ESP_LOGE(TAG, "Now Weather request failed: %s", esp_err_to_name(err));
    }
    esp_http_client_cleanup(client);
}
/*
{
#define WEATHER_URL_NOW_H "https://api.seniverse.com/v3/weather/now.json?key=S-Yt1LQ8hD3SNR3RA&location=beijing&language=zh-Hans&unit=c"
  "results": [
    {
      "location": {
        "id": "C23NB62W20TF",
        "name": "西雅图",
        "country": "US",
        "path": "西雅图,华盛顿州,美国",
        "timezone": "America/Los_Angeles",
        "timezone_offset": "-07:00"
      },
      "now": {
        "text": "多云", //天气现象文字
        "code": "4", //天气现象代码
        "temperature": "14", //温度，单位为c摄氏度或f华氏度
        "feels_like": "14", //体感温度，单位为c摄氏度或f华氏度
        "pressure": "1018", //气压，单位为mb百帕或in英寸
        "humidity": "76", //相对湿度，0~100，单位为百分比
        "visibility": "16.09", //能见度，单位为km公里或mi英里
        "wind_direction": "西北", //风向文字
        "wind_direction_degree": "340", //风向角度，范围0~360，0为正北，90为正东，180为正南，270为正西
        "wind_speed": "8.05", //风速，单位为km/h公里每小时或mph英里每小时
        "wind_scale": "2", //风力等级，请参考：http://baike.baidu.com/view/465076.htm
        "clouds": "90", //云量，单位%，范围0~100，天空被云覆盖的百分比 #目前不支持中国城市#
        "dew_point": "-12" //露点温度，请参考：http://baike.baidu.com/view/118348.htm #目前不支持中国城市#
      },
      "last_update": "2015-09-25T22:45:00-07:00" //数据更新时间（该城市的本地时间）
    }
  ]
}
*/
bool parse_seniverse_weather_now(const char *json, WeatherNow *weather)
{
    if (!json || !weather) return false;

    cJSON *root = cJSON_Parse(json);
    if (!root) return false;

    cJSON *results = cJSON_GetObjectItem(root, "results");
    if (!cJSON_IsArray(results)) {
        cJSON_Delete(root);
        return false;
    }

    cJSON *first_result = cJSON_GetArrayItem(results, 0);
    cJSON *location = cJSON_GetObjectItem(first_result, "location");
    cJSON *now = cJSON_GetObjectItem(first_result, "now");
    cJSON *last_update = cJSON_GetObjectItem(first_result, "last_update");

    if (location && now && last_update) {
        snprintf(weather->city, sizeof(weather->city), "%s", cJSON_GetObjectItem(location, "name")->valuestring);
        snprintf(weather->text, sizeof(weather->text), "%s", cJSON_GetObjectItem(now, "text")->valuestring);
        snprintf(weather->temperature, sizeof(weather->temperature), "%s", cJSON_GetObjectItem(now, "temperature")->valuestring);
        snprintf(weather->last_update, sizeof(weather->last_update), "%s", last_update->valuestring);
        cJSON_Delete(root);
        return true;
    }

    cJSON_Delete(root);
    return false;
}

/*
#define WEATHER_URL_24H "https://api.seniverse.com/v3/weather/hourly.json?key=S-Yt1LQ8hD3SNR3RA&location=jinhua&language=zh-Hans&unit=c&start=0&hours=24"
{
  "results": [{
    "location": {
      "id": "WX4FBXXFKE4F",
      "name": "北京",
      "country": "CN",
      "path": "北京,北京,中国",
      "timezone": "Asia/Shanghai",
      "timezone_offset": "+08:00"
    },
    "hourly": [{           //逐小时天气预报数组，数量可由start和hours参数控制，最多24个对象。
      "time": "2016-02-19T14:00:00+08:00",  //时间
      "text": "多云",      //天气现象文字
      "code": "4",        //天气现象代码
      "temperature": "7",     //温度，单位为c摄氏度或f华氏度
      "humidity": "35",       //相对湿度，0~100，单位为百分比
      "wind_direction": "北", //风向
      "wind_speed": "18.0"    //风速，单位为km/h公里每小时或mph英里每小时
    }, {                  //下一个小时预报
      "time": "2016-02-19T15:00:00+08:00",
      "text": "多云",
      "code": "4",
      "temperature": "6",
      "humidity": "37",
      "wind_direction": "西北",
      "wind_speed": "12.0"
    }, {
      ...                 //后面的就省略了
    }]
  }]
}
*/
bool parse_seniverse_weather_24h(const char *json, Weather24H *weather)
{
    if (!json || !weather) return false;

    cJSON *root = cJSON_Parse(json);
    if (!root) return false;

    cJSON *results = cJSON_GetObjectItem(root, "results");
    if (!cJSON_IsArray(results)) {
        cJSON_Delete(root);
        return false;
    }

    cJSON *first_result = cJSON_GetArrayItem(results, 0);
    cJSON *hourly = cJSON_GetObjectItem(first_result, "hourly");

    if (!cJSON_IsArray(hourly)) {
        cJSON_Delete(root);
        return false;
    }

    int count = cJSON_GetArraySize(hourly);
    if (count > MAX_HOURLY_FORECAST) count = MAX_HOURLY_FORECAST;

    for (int i = 0; i < count; ++i) {
        cJSON *item = cJSON_GetArrayItem(hourly, i);
        snprintf(weather->hourly[i].time, sizeof(weather->hourly[i].time), "%s", cJSON_GetObjectItem(item, "time")->valuestring);
        snprintf(weather->hourly[i].text, sizeof(weather->hourly[i].text), "%s", cJSON_GetObjectItem(item, "text")->valuestring);
        snprintf(weather->hourly[i].temperature, sizeof(weather->hourly[i].temperature), "%s", cJSON_GetObjectItem(item, "temperature")->valuestring);
    }

    weather->count = count;
    cJSON_Delete(root);
    return true;
}
