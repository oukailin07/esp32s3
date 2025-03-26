//#pragma execution_character_set("utf-8")
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_http_client.h"
#include "esp_log.h"  // 添加ESP_LOG头文件
#include "lvgl.h"
#include "lvgl_ui/lvgl_wifi_scan_ui.h"

static const char *TAG = "WIFI_SCAN_UI";
QueueHandle_t wifi_scan_q;
/* 信号强度图标函数声明（放在文件顶部）*/
static const char* get_wifi_icon(int8_t rssi);
/* 样式定义 */
static lv_style_t header_style;
static lv_style_t list_style;
static lv_style_t item_style;

static void create_styles(void) {
    // 标题样式
    lv_style_init(&header_style);
    lv_style_set_bg_color(&header_style, lv_color_white());
    lv_style_set_pad_all(&header_style, 0);
    lv_style_set_border_width(&header_style, 0);
    
    // 列表样式
    lv_style_init(&list_style);
    lv_style_set_bg_color(&list_style, lv_color_hex(0xF2F2F7)); // iOS风格浅灰色背景
    lv_style_set_pad_all(&list_style, 0);
    lv_style_set_pad_row(&list_style, 0); // 行间距为0
    
    // 项目样式
    lv_style_init(&item_style);
    lv_style_set_bg_color(&item_style, lv_color_white());
    lv_style_set_border_side(&item_style, LV_BORDER_SIDE_BOTTOM);
    lv_style_set_border_color(&item_style, lv_color_hex(0xE5E5EA)); // 底部边框色
    lv_style_set_border_width(&item_style, 1);
    lv_style_set_pad_all(&item_style, 0);
    lv_style_set_height(&item_style, 20); // 固定高度20px
}

/* 创建无线局域网标题栏 */
static void create_header(lv_obj_t *parent) {
    // 标题容器
    lv_obj_t *header = lv_obj_create(parent);
    lv_obj_remove_style_all(header); // 清除所有默认样式
    lv_obj_set_size(header, 128, 20);
    lv_obj_set_style_bg_color(header, lv_color_white(), 0);
    lv_obj_align(header, LV_ALIGN_TOP_LEFT, 0, 0); // 确保定位在顶部
    
    // 标题文本
    lv_obj_t *label = lv_label_create(header);
    LV_FONT_DECLARE(lv_font_source_han_sans_bold_12);
    lv_obj_set_style_text_font(label, &lv_font_source_han_sans_bold_12, 0);
    lv_label_set_text(label, "无线局域网");
    lv_obj_align(label, LV_ALIGN_LEFT_MID, 10, 0);
    
    // 开关控件
    lv_obj_t *sw = lv_switch_create(header);
        // 在创建开关后直接设置颜色
    //lv_obj_set_style_bg_color(sw, lv_color_hex(0x00C853), LV_PART_MAIN | LV_STATE_CHECKED);
    //lv_obj_set_style_bg_color(sw, lv_color_hex(0x9E9E9E), LV_PART_MAIN);
    // 滑块颜色
    lv_obj_set_style_bg_color(sw, lv_color_hex(0x43CD80), LV_PART_INDICATOR | LV_STATE_CHECKED);
    lv_obj_set_style_bg_color(sw, lv_color_hex(0x43CD80), LV_PART_INDICATOR);
    lv_obj_set_size(sw, 30, 15);
    lv_obj_align(sw, LV_ALIGN_RIGHT_MID, -10, 0);
    lv_obj_add_state(sw, LV_STATE_CHECKED);
    
    // 底部边框
    lv_obj_set_style_border_side(header, LV_BORDER_SIDE_BOTTOM, 0);
    lv_obj_set_style_border_width(header, 1, 0);
    lv_obj_set_style_border_color(header, lv_color_hex(0xE5E5EA), 0);
}

/* 创建"我的网络"列表 */
static void create_saved_networks(lv_obj_t *parent) {
    // 分区标题
    lv_obj_t *title = lv_obj_create(parent);
    lv_obj_remove_style_all(title);
    lv_obj_set_size(title, 128, 20);
    lv_obj_set_style_bg_color(title, lv_color_hex(0xF2F2F7), 0); // iOS风格浅灰背景
    lv_obj_align(title, LV_ALIGN_TOP_LEFT, 0, 20); // 接在标题栏下方
    
    lv_obj_t *label = lv_label_create(title);
    LV_FONT_DECLARE(lv_font_source_han_sans_bold_12);
    lv_obj_set_style_text_font(label, &lv_font_source_han_sans_bold_12, 0);
    lv_label_set_text(label, "我的网络");
    lv_obj_align(label, LV_ALIGN_LEFT_MID, 10, 0);
    
    // 示例：已保存网络1
    lv_obj_t *saved1 = lv_obj_create(parent);
    lv_obj_remove_style_all(saved1);
    lv_obj_set_size(saved1, 128, 20);
    lv_obj_set_style_bg_color(saved1, lv_color_white(), 0);
    lv_obj_align(saved1, LV_ALIGN_TOP_LEFT, 0, 40); // 20(header) + 20(title)
    
    lv_obj_t *saved1_label = lv_label_create(saved1);
    lv_label_set_text(saved1_label, "Home_WiFi");
    lv_obj_align(saved1_label, LV_ALIGN_LEFT_MID, 10, 0);
    
    lv_obj_t *check = lv_label_create(saved1);
    lv_label_set_text(check, LV_SYMBOL_OK);
    lv_obj_align(check, LV_ALIGN_RIGHT_MID, -10, 0);
}

static void show_scan_results(lv_obj_t *parent) {
     // 分区标题
     lv_obj_t *title = lv_obj_create(parent);
     lv_obj_remove_style_all(title);
     lv_obj_set_size(title, 128, 20);
     lv_obj_set_style_bg_color(title, lv_color_hex(0xF2F2F7), 0);
     lv_obj_align(title, LV_ALIGN_TOP_LEFT, 0, 60); // 20+20+20(saved)
     
     lv_obj_t *label = lv_label_create(title);
     LV_FONT_DECLARE(lv_font_source_han_sans_bold_12);
     lv_obj_set_style_text_font(label, &lv_font_source_han_sans_bold_12, 0);
     lv_label_set_text(label, "其他网络");
     lv_obj_align(label, LV_ALIGN_LEFT_MID, 10, 0);
 
     // 滚动容器（从y=80开始，高度80）
     lv_obj_t *list = lv_obj_create(parent);
     lv_obj_remove_style_all(list);
     lv_obj_set_size(list, 128, 80);
     lv_obj_set_style_bg_color(list, lv_color_white(), 0);
     lv_obj_align(list, LV_ALIGN_TOP_LEFT, 0, 80);
     lv_obj_set_flex_flow(list, LV_FLEX_FLOW_COLUMN);
     lv_obj_set_style_pad_row(list, 0, 0);

    while(1) {
        wifi_ap_record_t *ap_record = NULL;
        if (xQueueReceive(wifi_scan_q, &ap_record, portMAX_DELAY) == pdPASS) {
            if (ap_record) {
                // 创建网络项目
                lv_obj_t *item = lv_obj_create(list);
                lv_obj_set_size(item, 128, 20);
                lv_obj_add_style(item, &item_style, 0);
                
                // 转换SSID格式
                char ssid[33];
                strncpy(ssid, (const char*)ap_record->ssid, 32);
                ssid[32] = '\0';
                
                // 添加SSID标签
                lv_obj_t *ssid_label = lv_label_create(item);
                lv_label_set_text(ssid_label, ssid);
                lv_obj_align(ssid_label, LV_ALIGN_LEFT_MID, 5, 0);
                
                // 修改后的信号强度显示（使用标准WiFi符号）
                lv_obj_t *rssi_icon = lv_label_create(item);
                lv_label_set_text(rssi_icon, LV_SYMBOL_WIFI); // 使用基础WiFi符号
                lv_obj_align(rssi_icon, LV_ALIGN_RIGHT_MID, -5, 0);
                
                free(ap_record);
            }
        }
    }
}

/* 简化版的信号强度图标函数 */
static const char* get_wifi_icon(int8_t rssi) {
    if(rssi > -50) return LV_SYMBOL_WIFI;
    if(rssi > -60) return LV_SYMBOL_WIFI;
    if(rssi > -70) return LV_SYMBOL_WIFI;
    return "";
}

static void show_scan_wifi(void* arg) {
    // 清空屏幕
    lv_obj_clean(lv_scr_act());
    
    // 创建各部分（使用绝对坐标）
    create_header(lv_scr_act());          // y=0
    create_saved_networks(lv_scr_act()); // y=20
    show_scan_results(lv_scr_act());     // y=60
}

void lvgl_wifi_scan_ui_init(void) {
    wifi_scan_q = xQueueCreate(10, sizeof(wifi_ap_record_t *));
    xTaskCreate(show_scan_wifi, "wifi_ui", 4096, NULL, 5, NULL);
}