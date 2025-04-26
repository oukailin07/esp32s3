#include <stdio.h>
#include "esp32_s3_szp.h"
#include "demos/lv_demos.h"
#include "wifi.h"
#include "sd_card.h"
#include "tcp_save_file.h"
#include "ui_mjpeg_player.h"
#include "bsp_spiffs.h"
#include "esp_log.h"
#include "tcp_save_file.h"

#include "nvs_flash.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "lvgl.h"
 
//#include "lvgl_helpers.h"
 
#include "gui_guider.h"
#include "custom.h"
 
 
lv_ui guider_ui;


#define WIFI_SSID "adol-3466"
#define WIFI_PASS "12345678"
static const char *TAG = "main";
void app_main(void)
{
    esp_err_t ret;
    ESP_ERROR_CHECK(bsp_i2c_init());  // I2C初始化
    pca9557_init();  // IO扩展芯片初始化
    pca9557_debug();
    ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    
    ESP_ERROR_CHECK(ret);
    bsp_lvgl_start(); // 初始化液晶屏lvgl接口

    wifi_init();
    app_wifi_init(WIFI_SSID, WIFI_PASS); // 初始化wifi
    save_wifi_info(WIFI_SSID,WIFI_PASS);
    char ssid[32];
    char password[64];
    read_wifi_info(ssid, sizeof(ssid), password, sizeof(password));
    printf("SSID: %s\n", ssid);
    printf("Password: %s\n", password);

    bsp_spiffs_init("ui_img","/img",5);
    sdcard_init();
    setup_ui(&guider_ui);

    //ui_mjpeg_create();
    //tcp_file_start();

    // lv_obj_t *img;
    // img = lv_gif_create(lv_scr_act());
    // lv_gif_set_src(img, "/img/disdain.gif");
    // lv_obj_align(img, LV_ALIGN_RIGHT_MID, 0, 0);


}

