#include "esp_wifi.h"
#include "esp_log.h"
#include "string.h"
#include "freertos/event_groups.h"
#include "freertos/FreeRTOS.h"
#include "wifi_sta_ap.h"
EventGroupHandle_t app_wifi_event_group;
static const char *TAG = "APP_WIFI";
static int s_retry_num = 0; // retry number
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT BIT1

//sta_congfig
#define  DEFAULT_WIFI_SSID "adol-3466"        //需要连接的WIFI名称
#define  DEFAULT_WIFI_PASSWORD "12345678"   	//wifi对应的密码

//ap_congfig
#define CONFIG_ESP_AP_WIFI_SSID "esp32"
#define CONFIG_ESP_AP_WIFI_PASSWORD "12345678"
#define TAG  "app_sta_ap"
esp_netif_t *sta;
esp_netif_t *ap;
QueueHandle_t wifi_event_queue;
void app_wifi_handler(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        if (s_retry_num < 5) {
            esp_wifi_connect();
            s_retry_num++;
            ESP_LOGI(TAG, "retry to connect to the AP");
        } else {
            xEventGroupSetBits(app_wifi_event_group, WIFI_FAIL_BIT);
        }
        ESP_LOGI(TAG, "connect to the AP fail");
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0;
        xEventGroupSetBits(app_wifi_event_group, WIFI_CONNECTED_BIT);
    }
    else if (event_id == WIFI_EVENT_AP_STACONNECTED) {
        wifi_event_ap_staconnected_t* event = (wifi_event_ap_staconnected_t*) event_data;

    } else if (event_id == WIFI_EVENT_AP_STADISCONNECTED) {
        wifi_event_ap_stadisconnected_t* event = (wifi_event_ap_stadisconnected_t*) event_data;

    }
}
static void wifi_sta_start(void)
{
    sta = esp_netif_create_default_wifi_sta();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &app_wifi_handler, NULL, NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &app_wifi_handler, NULL, NULL));

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = DEFAULT_WIFI_SSID,
            .password = DEFAULT_WIFI_PASSWORD,
            .threshold.authmode = WIFI_AUTH_WPA_PSK,
        },
    };

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "wifi_init_sta finished.");

    EventBits_t bits = xEventGroupWaitBits(app_wifi_event_group, WIFI_CONNECTED_BIT | WIFI_FAIL_BIT, pdFALSE, pdFALSE, portMAX_DELAY);

    if (bits & WIFI_CONNECTED_BIT) {
        ESP_LOGI(TAG, "connected to ap SSID:%s password:%s", wifi_config.sta.ssid, wifi_config.sta.password);
    } else if (bits & WIFI_FAIL_BIT) {
        ESP_LOGI(TAG, "Failed to connect to SSID:%s, password:%s", wifi_config.sta.ssid, wifi_config.sta.password);
    } else {
        ESP_LOGI(TAG, "UNEXPECTED EVENT");
    }
}

static void wifi_ap_start(void)
{
    ap = esp_netif_create_default_wifi_ap();    
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &app_wifi_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &app_wifi_handler, NULL));
    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_AP) );
    wifi_config_t ap_config = {
        .ap = {
            .ssid = CONFIG_ESP_AP_WIFI_SSID,
            .ssid_len = strlen(CONFIG_ESP_AP_WIFI_SSID),
            .channel = 11,
            .password = CONFIG_ESP_AP_WIFI_PASSWORD,
            .max_connection = 5,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &ap_config));
    ESP_ERROR_CHECK(esp_wifi_start());
}

static void wifi_ap_sta_start(void)
{

}

static void wifi_ap_update_info(char *ssid, char *pswd)
{
    wifi_config_t ap_config = {
        .ap = {
            .ssid = CONFIG_ESP_AP_WIFI_SSID,
            .ssid_len = strlen(CONFIG_ESP_AP_WIFI_SSID),
            .channel = 11,
            .password = CONFIG_ESP_AP_WIFI_PASSWORD,
            .max_connection = 5,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK
        },
    };
    strncpy((char *)&ap_config.ap.ssid, ssid, strlen(ssid));
    strncpy((char *)&ap_config.ap.password, pswd, strlen(pswd));
    ap_config.ap.ssid_len = strlen(ssid);
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &ap_config));
}

static void wifi_sta_update_info(char *ssid, char *pswd)
{
    wifi_config_t sta_config = {
        .sta = {
            .ssid = DEFAULT_WIFI_SSID,
            .password = DEFAULT_WIFI_PASSWORD,
            .bssid_set = false,
            .threshold.authmode = WIFI_AUTH_WPA2_PSK,
        }
    };
    memcpy((char *)&sta_config.sta.ssid,ssid,strlen(ssid));
    memcpy((char *)&sta_config.sta.password,pswd,strlen(pswd));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &sta_config));
}
static void wifi_ap_stop(void)
{
    ESP_LOGI(TAG, "ESP_WIFI_MODE_AP STOP");
    ESP_ERROR_CHECK(esp_event_handler_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, app_wifi_handler));
    ESP_ERROR_CHECK(esp_event_handler_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, app_wifi_handler));
    esp_netif_destroy_default_wifi(ap);
    ESP_ERROR_CHECK(esp_wifi_stop() );
    ESP_ERROR_CHECK(esp_wifi_deinit() );
}
static void wifi_sta_stop(void)
{
    ESP_LOGI(TAG, "ESP_WIFI_MODE_STA STOP");
    ESP_ERROR_CHECK(esp_event_handler_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, app_wifi_handler));
    ESP_ERROR_CHECK(esp_event_handler_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, app_wifi_handler));
    esp_netif_destroy_default_wifi(sta);
    ESP_ERROR_CHECK(esp_wifi_stop() );
    ESP_ERROR_CHECK(esp_wifi_deinit() );
}

static void wifi_net_task(void* arg)
{

    for(;;) {
        vTaskDelay(pdMS_TO_TICKS(100));  // 每 100ms 释放 CPU
        WIFI_SET_EVENT_E evt;
        xQueueReceive(wifi_event_queue, &evt, portMAX_DELAY);
        printf("get wifi set event %d\n",evt);
        switch (evt)
        {
        case STA_START:
            wifi_sta_start();
            break;
        case STA_UPDATE:
            //wifi_sta_update_info();
            break;
        case STA_STOP:
            wifi_sta_stop();
            break;
        case AP_START:
            wifi_ap_start();
            break;
        case AP_UPDATE:
            //wifi_ap_update_info();
            break;
        case AP_STOP:
            wifi_ap_stop();
            break;
        default:
            break;
        }
    }
}

void ds_wifi_send_event(WIFI_SET_EVENT_E event){
	WIFI_SET_EVENT_E evt;
    evt = event;
	xQueueSend(wifi_event_queue, &evt, 0);
}

void ds_wifi_ap_sta_init(){
    wifi_event_queue = xQueueCreate(10, sizeof(WIFI_SET_EVENT_E));
    xTaskCreatePinnedToCore(wifi_net_task, "wifi_net_task", 4096, NULL, 10, NULL,1);    
}
