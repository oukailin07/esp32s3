#include "lvgl_wifi_scan_ui.h"

QueueHandle_t wifi_scan_q;

static void read_audio_task(void* arg)
{
    while(1)
    {
        if (xQueueReceive(wifi_scan_q, &read_start_flag, portMAX_DELAY) == pdPASS) 
        {

        }
    }
}

void lvgl_wifi_scan_ui_init(void)
{
    xTaskCreatePinnedToCore(show_scan_wifi, "show_scan_wifi", 4096, NULL, 10, NULL,1);   
    wifi_scan_q = xQueueCreate(10, sizeof(wifi_ap_record_t));
}