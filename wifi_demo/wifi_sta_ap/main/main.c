#include <stdio.h>
#include "esp_wifi.h"
#include "esp_log.h"


#include "wifi_sta_ap.h"
void app_main(void)
{

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
}
