#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"
#include "lvgl.h"
#include "lv_port_disp.h"
#include "lv_port_indev.h"


#include <string.h>
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_event.h"
#include "nvs_flash.h"

//#include "lv_demos.h"
#include "lib_lcd7735.h"
#include "wifi_scan.h"
static esp_timer_handle_t lvgl_tick_timer = NULL;
 
// 定时回调函数，每 1ms 触发
static void lv_tick_task(void *arg) {
    lv_tick_inc(1);
}
 
// 初始化 LVGL Tick 定时器
void lvgl_tick_timer_init(void) {
    const esp_timer_create_args_t timer_args = {
        .callback = &lv_tick_task,
        .arg = NULL,
        .dispatch_method = ESP_TIMER_TASK,
        .name = "lv_tick_timer"
    };
 
    esp_timer_create(&timer_args, &lvgl_tick_timer);
    esp_timer_start_periodic(lvgl_tick_timer, 1000); // 1ms 触发
}
 
void app_main(void)
{
    lvgl_tick_timer_init();  // 初始化 LVGL 时基
 
    lv_init();              /* 初始化LVGL图形库 */
    lv_port_disp_init();    /* lvgl显示接口初始化,放在lv_init()的后面 */
    //lv_port_indev_init();   /* lvgl输入接口初始化,放在lv_init()的后面 */

    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK( ret );

    wifi_scan();
    while (1) 
    {
        //LCD_DrawRectangle(0, 0, 128-1, 160-1, 0xF800);
        lv_task_handler();  // LVGL 任务管理
        vTaskDelay(pdMS_TO_TICKS(10));  // 延迟 10ms
    }
}