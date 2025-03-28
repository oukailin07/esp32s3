/* Hello World Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "spi_flash_mmap.h"
#include "nvs_flash.h"
#include "esp_event.h"

#include "driver/gpio.h"
#include "esp_sleep.h"
#include <time.h>
#include <sys/time.h>
#include "esp_timer.h"

#include "esp_task_wdt.h"

#include "ds_test.h"
//#include "ds_wifi_scan.h"
#include "ds_ft6336.h"
#include "ds_screen.h"
#include "ds_spi.h"
//#include "ds_wifi_ap.h"
//#include "ds_wifi_sta.h"
#include "ds_system_data.h"
#include "ds_nvs.h"
#include "ds_https_request.h"
#include "ds_http_request.h"
#include "ds_timer.h"
#include "ds_spiffs.h"
#include "ds_ui_timepage.h"
#include "ds_ui_page_manage.h"
#include "ds_ui_weatherpage.h"
#include "ds_ui_wordpage.h"
#include "ds_ui_tomatopage.h"
#include "ds_ui_fans.h"
#include "ds_font.h"
#include "ds_paint.h"
#include "ds_wifi_ap_sta.h"
#include "ds_pwm.h"
#include "ds_gpio.h"
#include "spi_flash_mmap.h"
#include "esp_chip_info.h"
#ifdef CONFIG_IDF_TARGET_ESP32
#define CHIP_NAME "ESP32"
#endif

#ifdef CONFIG_IDF_TARGET_ESP32S2BETA
#define CHIP_NAME "ESP32-S2 Beta"
#endif

#define BUTTON_GPIO_NUM_DEFAULT  4
#define TWDT_TIMEOUT_S          3

static void sleep_mode_init(){

    while (true) {
        set_tp_wackup_timeleft(600);
        vTaskDelay(pdMS_TO_TICKS(100));
        //ap&sta关闭、且当前在主页、且超过5min未触摸时才进入低功耗
        do {
            //esp_task_wdt_reset(); 
            vTaskDelay(pdMS_TO_TICKS(1000));
            count_tp_wackup_timeleft();
            get_tp_wackup_timeleft();
            
            if(get_is_ap_sta_open() == false && ds_ui_get_now_show_page() == PAGE_TYPE_MEMU && get_tp_wackup_timeleft() == 0){
                break;
            }
        } while (1);

        ds_touch_gpio_isr_remove();
        gpio_wakeup_enable(BUTTON_GPIO_NUM_DEFAULT,GPIO_INTR_LOW_LEVEL);
        /* Wake up in 60 seconds, or when button is pressed */
        // esp_sleep_enable_timer_wakeup(60000000);
        esp_sleep_enable_gpio_wakeup();

        printf("Entering light sleep\n");

        /* Get timestamp before entering sleep */
        int64_t t_before_us = esp_timer_get_time();

        /* Enter sleep mode */
        esp_light_sleep_start();
        /* Execution continues here after wakeup */

        /* Get timestamp after waking up from sleep */
        int64_t t_after_us = esp_timer_get_time();

        /* Determine wake up reason */
        const char* wakeup_reason;
        uint32_t wackup_timeleft = 600;
        switch (esp_sleep_get_wakeup_cause()) {
            case ESP_SLEEP_WAKEUP_TIMER:
                wakeup_reason = "timer";
                wackup_timeleft = 5;
                break;
            case ESP_SLEEP_WAKEUP_GPIO:
                wakeup_reason = "pin";
                break;
            default:
                wakeup_reason = "other";
                break;
        }
        printf("Returned from light sleep, reason: %s, t=%lld ms, slept for %lld ms\n",
                wakeup_reason, t_after_us / 1000, (t_after_us - t_before_us) / 1000);

        set_tp_wackup_timeleft(wackup_timeleft);
        gpio_wakeup_disable(BUTTON_GPIO_NUM_DEFAULT);
        esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_ALL);
        ds_touch_gpio_init();
        ds_touch_gpio_isr_add();
        reset_tp_action_manage();
    }
}

static void background_task(void* arg)
{
    int apsta_close_count = 0;
    for(;;){
        printf("background_task run... \n");
        vTaskDelay(pdMS_TO_TICKS(1000));
        //首次更新
        if(has_first_time_httpdata_request() == true && get_wifi_sta_status() == WIFI_STA_MODE_CONNECT_SUCCESS){
            ds_http_request_all();
            set_has_first_time_httpdata_request();
        }
        //下拉请求更新
        if(has_update_httpdata_request() == true){
            if(get_is_ap_sta_open()){
                if(get_wifi_sta_status() == WIFI_STA_MODE_CONNECT_SUCCESS){
                    ds_http_request_all();
                    set_update_httpdata_request(false);
                }
            }else{
                ds_wifi_send_event(AP_STA_START);
                vTaskDelay(pdMS_TO_TICKS(100));
            }
        }
        //关闭wifi AP&STA 计时
        if(get_is_ap_sta_open() == true && apsta_close_count == 0){
            //重置10min计时
            apsta_close_count = (600 - 10);
        }
        if(apsta_close_count > 0){
            apsta_close_count --;
            if(apsta_close_count == 0){
                ds_wifi_send_event(AP_STA_STOP);
                vTaskDelay(pdMS_TO_TICKS(500));
                set_update_httpdata_request(false);
            }
        }
        //设置页面 开启AP&STA
        if(ds_ui_get_now_show_page() == PAGE_TYPE_SETTING){
            if(get_is_ap_sta_open() == false){
                ds_wifi_send_event(AP_STA_START);
                vTaskDelay(pdMS_TO_TICKS(500));
            }
        }
    }
}

void app_main(void)
{
    printf("----- app start! -----\n");

    /* Print chip information */
    esp_chip_info_t chip_info;
    //esp_chip_info(&chip_info);
    

    //printf("silicon revision %d, ", chip_info.revision);

    //printf("%dMB %s flash\n", spi_flash_get_chip_size() / (1024 * 1024),
            //(chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");

    //看门狗初始化
    // 配置任务看门狗
    esp_task_wdt_config_t twdt_config = {
        .timeout_ms = 50000,  // 看门狗超时时间（毫秒）
        .idle_core_mask = 0, // 监控所有核心
        .trigger_panic = false, // 不触发 panic
    };
    esp_task_wdt_init(&twdt_config);
    //esp_task_wdt_delete(NULL);
    //esp_task_wdt_add(NULL);
    //系统相关数据初始化
    ds_system_data_init();
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    //http请求初始化
    ds_http_request_init();
    //ap&sta模式初始化
    ds_wifi_ap_sta_init();   

    // //屏幕、TP外设初始化相关
    //init_ft6336();
    //init_screen_interface();
    //屏幕显示数据初始化
    //ds_ui_timepage_init();
    //ds_ui_page_manage_init();
    //ds_ui_weather_init();
    //ds_ui_wordpage_init();
    //ds_ui_wordpage_init();
    //ds_ui_tomatopage_init();
    //ds_ui_fans_init();
    //定时器初始化
    ds_timer_init();
    //PWM蜂鸣器初始化
    ds_pwm_init();

     //ds_screen_init();
     //ds_screen_partial_test();

    //默认Wifi账号密码设置
    // char *ssid="leo";
    // char *psw="1234567890";
    // ds_nvs_save_wifi_info(ssid,psw);
    if(NVS_WIFI_INFO_HAS_SAVE == ds_nvs_read_wifi_info()){
        //已经存储wifi联网信息
        //ds_ui_page_manage_send_action(PAGE_TYPE_MEMU);
    }else{
        //未存储wifi联网信息
        //ds_ui_page_manage_send_action(PAGE_TYPE_SETTING);
    }

    //启动AP&STA模式
    vTaskDelay(pdMS_TO_TICKS(2000));
    ds_wifi_send_event(AP_STA_START);
    
    xTaskCreatePinnedToCore(background_task, "background_task", 4096, NULL, 10, NULL,1); 
    
    //进入低功耗模式
    sleep_mode_init();

    // char pWriteBuffer[1048];

    for(;;){
        printf("app run \n");
        vTaskDelay(pdMS_TO_TICKS(100));
        esp_reset_reason_t reason = esp_reset_reason();
        printf("Reset reason: %d\n", reason);
        //喂狗
        //esp_task_wdt_reset();  //Comment this line to trigger a TWDT timeout

        //堆栈信息打印，需打开USE_TRACE_FACILITY、USE_STATS_FORMATTING_FUNCTIONS
        // vTaskList((char *)&pWriteBuffer);
        // printf("task_name   task_state  priority   stack  tasK_num\n");
        // printf("%s\n", pWriteBuffer);  
    }
    //esp_task_wdt_delete(NULL);
}
