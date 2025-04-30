#ifndef GET_WEATHER_H
#define GET_WEATHER_H
#include <stdio.h>
#include <stdbool.h>
#define MAX_BUFFER_SIZE 2048  // 扩大缓冲区

/**
 * @brief 心知天气（seniverse） ，单日天气各项数据结构体
 */
typedef struct{
	char *date;
	char *text_day;
	char *code_day;
	char *text_night;
	char *code_night;
	char *high;
	char *low;
	char *rainfall;
	char *precip;
	char *wind_direction;
	char *wind_direction_degree;
	char *wind_speed;
	char *wind_scale;
	char *humidity;

} user_seniverse_day_config_t;

/**
 * @brief 心知天气（seniverse） ，完整数据结构体
 */
typedef struct {
	char *id;
	char *name;
	char *country;
	char *path;
	char *timezone;
	char *timezone_offset;
    user_seniverse_day_config_t day_config[3];/*这里的3是指心知天气URL中的 days=3*/
	char *last_update;

} user_seniverse_config_t;


typedef struct {
    char city[32];
    char text[32];
    char temperature[8];
    char last_update[32];
} WeatherNow;

#define MAX_HOURLY_FORECAST 24

typedef struct {
    char time[32];
    char text[32];
    char temperature[8];
} WeatherHourly;

typedef struct {
    WeatherHourly hourly[MAX_HOURLY_FORECAST];
    int count;
} Weather24H;

void http_rest_with_url(void);
bool parse_seniverse_weather(const char *json_str, user_seniverse_config_t *out_config);
void free_seniverse_config(user_seniverse_config_t *cfg);
void print_weather_details(user_seniverse_config_t *weather_data);
void http_get_weather_24h(void);
void http_get_weather_now(void);
bool parse_seniverse_weather_now(const char *json, WeatherNow *weather);
bool parse_seniverse_weather_24h(const char *json, Weather24H *weather);
#endif