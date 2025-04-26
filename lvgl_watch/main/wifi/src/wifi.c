#include "wifi.h"
#include "lvgl.h"
#include "custom.h"
static const char *TAG = "APP_WIFI";
EventGroupHandle_t app_wifi_event_group;
static int s_retry_num = 0; // retry number
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT BIT1
bool wifi_flag =false;
extern  lv_ui_top top;

static void app_wifi_handler(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        if (s_retry_num < 5) {
            //esp_wifi_connect();
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
}
void app_wifi_init(char *ssid, char *pswd)
{
    app_wifi_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &app_wifi_handler, NULL, NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &app_wifi_handler, NULL, NULL));

    wifi_config_t wifi_config = {
        .sta = {
            .threshold.authmode = WIFI_AUTH_WPA_PSK,
        },
    };
    strncpy((char *)&wifi_config.sta.ssid, ssid, strlen(ssid));
    strncpy((char *)&wifi_config.sta.password, pswd, strlen(pswd));

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "wifi_init_sta finished.");

    EventBits_t bits = xEventGroupWaitBits(app_wifi_event_group, WIFI_CONNECTED_BIT | WIFI_FAIL_BIT, pdFALSE, pdFALSE, portMAX_DELAY);

    if (bits & WIFI_CONNECTED_BIT) {
        ESP_LOGI(TAG, "connected to ap SSID:%s password:%s", wifi_config.sta.ssid, wifi_config.sta.password);
        if(top.screen_top_wifi)
        {
            lv_imgbtn_set_state(top.screen_top_wifi, LV_IMGBTN_STATE_CHECKED_RELEASED);
        }
    } else if (bits & WIFI_FAIL_BIT) {
        ESP_LOGI(TAG, "Failed to connect to SSID:%s, password:%s", wifi_config.sta.ssid, wifi_config.sta.password);
    } else {
        ESP_LOGI(TAG, "UNEXPECTED EVENT");
    }
}




static void print_auth_mode(int authmode)
{
    switch (authmode) {
    case WIFI_AUTH_OPEN:
        ESP_LOGI(TAG, "Authmode \tWIFI_AUTH_OPEN");
        break;
    case WIFI_AUTH_OWE:
        ESP_LOGI(TAG, "Authmode \tWIFI_AUTH_OWE");
        break;
    case WIFI_AUTH_WEP:
        ESP_LOGI(TAG, "Authmode \tWIFI_AUTH_WEP");
        break;
    case WIFI_AUTH_WPA_PSK:
        ESP_LOGI(TAG, "Authmode \tWIFI_AUTH_WPA_PSK");
        break;
    case WIFI_AUTH_WPA2_PSK:
        ESP_LOGI(TAG, "Authmode \tWIFI_AUTH_WPA2_PSK");
        break;
    case WIFI_AUTH_WPA_WPA2_PSK:
        ESP_LOGI(TAG, "Authmode \tWIFI_AUTH_WPA_WPA2_PSK");
        break;
    case WIFI_AUTH_ENTERPRISE:
        ESP_LOGI(TAG, "Authmode \tWIFI_AUTH_ENTERPRISE");
        break;
    case WIFI_AUTH_WPA3_PSK:
        ESP_LOGI(TAG, "Authmode \tWIFI_AUTH_WPA3_PSK");
        break;
    case WIFI_AUTH_WPA2_WPA3_PSK:
        ESP_LOGI(TAG, "Authmode \tWIFI_AUTH_WPA2_WPA3_PSK");
        break;
    case WIFI_AUTH_WPA3_ENT_192:
        ESP_LOGI(TAG, "Authmode \tWIFI_AUTH_WPA3_ENT_192");
        break;
    case WIFI_AUTH_WPA3_EXT_PSK:
        ESP_LOGI(TAG, "Authmode \tWIFI_AUTH_WPA3_EXT_PSK");
        break;
    case WIFI_AUTH_WPA3_EXT_PSK_MIXED_MODE:
        ESP_LOGI(TAG, "Authmode \tWIFI_AUTH_WPA3_EXT_PSK_MIXED_MODE");
        break;
    default:
        ESP_LOGI(TAG, "Authmode \tWIFI_AUTH_UNKNOWN");
        break;
    }
}

static void print_cipher_type(int pairwise_cipher, int group_cipher)
{
    switch (pairwise_cipher) {
    case WIFI_CIPHER_TYPE_NONE:
        ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_NONE");
        break;
    case WIFI_CIPHER_TYPE_WEP40:
        ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_WEP40");
        break;
    case WIFI_CIPHER_TYPE_WEP104:
        ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_WEP104");
        break;
    case WIFI_CIPHER_TYPE_TKIP:
        ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_TKIP");
        break;
    case WIFI_CIPHER_TYPE_CCMP:
        ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_CCMP");
        break;
    case WIFI_CIPHER_TYPE_TKIP_CCMP:
        ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_TKIP_CCMP");
        break;
    case WIFI_CIPHER_TYPE_AES_CMAC128:
        ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_AES_CMAC128");
        break;
    case WIFI_CIPHER_TYPE_SMS4:
        ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_SMS4");
        break;
    case WIFI_CIPHER_TYPE_GCMP:
        ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_GCMP");
        break;
    case WIFI_CIPHER_TYPE_GCMP256:
        ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_GCMP256");
        break;
    default:
        ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_UNKNOWN");
        break;
    }

    switch (group_cipher) {
    case WIFI_CIPHER_TYPE_NONE:
        ESP_LOGI(TAG, "Group Cipher \tWIFI_CIPHER_TYPE_NONE");
        break;
    case WIFI_CIPHER_TYPE_WEP40:
        ESP_LOGI(TAG, "Group Cipher \tWIFI_CIPHER_TYPE_WEP40");
        break;
    case WIFI_CIPHER_TYPE_WEP104:
        ESP_LOGI(TAG, "Group Cipher \tWIFI_CIPHER_TYPE_WEP104");
        break;
    case WIFI_CIPHER_TYPE_TKIP:
        ESP_LOGI(TAG, "Group Cipher \tWIFI_CIPHER_TYPE_TKIP");
        break;
    case WIFI_CIPHER_TYPE_CCMP:
        ESP_LOGI(TAG, "Group Cipher \tWIFI_CIPHER_TYPE_CCMP");
        break;
    case WIFI_CIPHER_TYPE_TKIP_CCMP:
        ESP_LOGI(TAG, "Group Cipher \tWIFI_CIPHER_TYPE_TKIP_CCMP");
        break;
    case WIFI_CIPHER_TYPE_SMS4:
        ESP_LOGI(TAG, "Group Cipher \tWIFI_CIPHER_TYPE_SMS4");
        break;
    case WIFI_CIPHER_TYPE_GCMP:
        ESP_LOGI(TAG, "Group Cipher \tWIFI_CIPHER_TYPE_GCMP");
        break;
    case WIFI_CIPHER_TYPE_GCMP256:
        ESP_LOGI(TAG, "Group Cipher \tWIFI_CIPHER_TYPE_GCMP256");
        break;
    default:
        ESP_LOGI(TAG, "Group Cipher \tWIFI_CIPHER_TYPE_UNKNOWN");
        break;
    }
}
wifi_ap_record_t g_ap_info[DEFAULT_SCAN_LIST_SIZE];

/* Initialize Wi-Fi as sta and set scan method */
uint16_t wifi_scan(wifi_ap_record_t *ap_info )
{

    uint16_t number = DEFAULT_SCAN_LIST_SIZE;
    uint16_t ap_count = 0;
    memset(ap_info, 0, sizeof(ap_info));

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());
    // wifi_scan_config_t scan_config = {
    //     .ssid = NULL,       // 扫描所有 SSID
    //     .bssid = NULL,      // 扫描所有 BSSID
    //     .channel = 0,        // 扫描所有信道
    //     .show_hidden = true, // 是否扫描隐藏网络
    // };
    esp_wifi_scan_start(NULL, true);
    ESP_LOGI(TAG, "Max AP number ap_info can hold = %u", number);
    ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&number, ap_info));
    ESP_ERROR_CHECK(esp_wifi_scan_get_ap_num(&ap_count));
    ESP_LOGI(TAG, "Total APs scanned = %u, actual AP number ap_info holds = %u", ap_count, number);
    for (int i = 0; i < number; i++) {
        ESP_LOGI(TAG, "SSID \t\t%s", ap_info[i].ssid);
        ESP_LOGI(TAG, "RSSI \t\t%d", ap_info[i].rssi);
        print_auth_mode(ap_info[i].authmode);
        if (ap_info[i].authmode != WIFI_AUTH_WEP) {
            print_cipher_type(ap_info[i].pairwise_cipher, ap_info[i].group_cipher);
        }
        ESP_LOGI(TAG, "Channel \t\t%d", ap_info[i].primary);
    }
    // 5. 停止 WiFi 并清理配置，避免影响后续连接
    ESP_ERROR_CHECK(esp_wifi_stop());
    //ESP_ERROR_CHECK(esp_wifi_deinit());
    return number;
}

esp_err_t print_current_wifi_info(wifi_ap_record_t *ap_info) {
    
    esp_err_t ret = esp_wifi_sta_get_ap_info(ap_info);

    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "Connected to:");
        ESP_LOGI(TAG, "  SSID: %s", ap_info->ssid);
        ESP_LOGI(TAG, "BSSID: %02x:%02x:%02x:%02x:%02x:%02x", 
            ap_info->bssid[0], ap_info->bssid[1], ap_info->bssid[2],
            ap_info->bssid[3], ap_info->bssid[4], ap_info->bssid[5]);
        ESP_LOGI(TAG, "  RSSI: %d dBm", ap_info->rssi);
        ESP_LOGI(TAG, "  Channel: %d", ap_info->primary);
    } else {
        ESP_LOGE(TAG, "Failed to get AP info (error: %s)", esp_err_to_name(ret));
    }
    return ret;
}

bool is_wifi_connected() {
    wifi_ap_record_t ap_info;
    return (esp_wifi_sta_get_ap_info(&ap_info) == ESP_OK);
}
void wifi_init(void)
{
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_t *sta_netif = esp_netif_create_default_wifi_sta();
    assert(sta_netif);

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    wifi_flag = true;

}

void app_wifi_del(void)
{
    esp_event_loop_delete_default();
    esp_netif_deinit();
    esp_netif_t *sta_netif = esp_netif_get_handle_from_ifkey("WIFI_STA_DEF");
    if (sta_netif) {
        esp_netif_destroy(sta_netif);
    }
    esp_err_t err = esp_event_handler_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, &app_wifi_handler);
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "event  IP_EVENT_STA_GOT_IP handler unregister failed: %s", esp_err_to_name(err));
    }
    err = esp_event_handler_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, &app_wifi_handler);
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "event  ESP_EVENT_ANY_ID handler unregister failed: %s", esp_err_to_name(err));
    }
    vEventGroupDelete(app_wifi_event_group);
}

void save_wifi_info(const char* ssid, const char* password) {
    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open("wifi_info", NVS_READWRITE, &nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGE("NVS", "Error (%s) opening NVS handle!", esp_err_to_name(err));
        return;
    }
    err = nvs_set_str(nvs_handle, "ssid", ssid);
    if (err != ESP_OK) {
        ESP_LOGE("NVS", "Error (%s) setting SSID!", esp_err_to_name(err));
    }
    err = nvs_set_str(nvs_handle, "password", password);
    if (err != ESP_OK) {
        ESP_LOGE("NVS", "Error (%s) setting password!", esp_err_to_name(err));
    }
    err = nvs_commit(nvs_handle);
        if (err != ESP_OK) {
        ESP_LOGE("NVS", "Error (%s) committing changes!", esp_err_to_name(err));
    }
    nvs_close(nvs_handle);
}
    
esp_err_t read_wifi_info(char* ssid, size_t ssid_len, char* password, size_t pass_len) {
    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open("wifi_info", NVS_READONLY, &nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGE("NVS", "Error (%s) opening NVS handle!", esp_err_to_name(err));
        return err;
    }
    err = nvs_get_str(nvs_handle, "ssid", ssid, &ssid_len);
    if (err != ESP_OK) {
        ESP_LOGE("NVS", "Error (%s) reading SSID!", esp_err_to_name(err));
        return err;
    }
    err = nvs_get_str(nvs_handle, "password", password, &pass_len);
    if (err != ESP_OK) {
        ESP_LOGE("NVS", "Error (%s) reading password!", esp_err_to_name(err));
        return err;
    }
    nvs_close(nvs_handle);
    return ESP_OK;
}

// void app_main() {
//     esp_err_t ret = nvs_flash_init();
//     if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
//     ESP_ERROR_CHECK(nvs_flash_erase());
//     ret = nvs_flash_init();
//     }
//     ESP_ERROR_CHECK(ret);
    
//     save_wifi_info(WIFI_SSID, WIFI_PASS);
    
//     char ssid[32];
//     char password[64];
//     read_wifi_info(ssid, sizeof(ssid), password, sizeof(password));
//     printf("SSID: %s\n", ssid);
//     printf("Password: %s\n", password);
// }