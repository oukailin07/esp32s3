#ifndef WIFI_H
#define WIFI_H


#include "esp_event.h"
#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_vfs_fat.h"
#include "string.h"

#define DEFAULT_SCAN_LIST_SIZE 10

void app_wifi_init(char *ssid, char *pswd);
void app_wifi_del(void);
uint16_t wifi_scan(wifi_ap_record_t *ap_info);
void wifi_init(void);
bool is_wifi_connected();
esp_err_t print_current_wifi_info(wifi_ap_record_t *ap_info);
esp_err_t read_wifi_info(char* ssid, size_t ssid_len, char* password, size_t pass_len);
void save_wifi_info(const char* ssid, const char* password);
#endif // WIFI_H