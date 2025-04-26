/*
* Copyright 2025 NXP
* NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
* terms, then you may not retain, install, activate or otherwise use the software.
*/
#include <stdlib.h>  // 添加在文件顶部
#include "events_init.h"
#include <stdio.h>
#include "lvgl.h"
#include "custom.h"
#include "wifi.h"
#include "esp_log.h"
static const char *TAG = "events_init";
#if LV_USE_GUIDER_SIMULATOR && LV_USE_FREEMASTER
#include "freemaster_client.h"
#endif
extern void add_wifi_info_btn();
extern lv_ui_top top;
extern lv_ui guider_ui;
static int32_t bar_height = 240;
static int32_t press_start_y = 0;
static int32_t panel_start_y = 0;
static bool dragging = false;
void screen_event_handler(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *page = (lv_obj_t *)lv_event_get_user_data(e);
    lv_indev_t *indev = lv_indev_get_act();
    lv_point_t point;
    lv_indev_get_point(indev, &point);

    if(code == LV_EVENT_PRESSED) {
        // 只在顶部或底部区域按下时才启用拖动
        if(point.y < SLIDE_ZONE || point.y > (SCREEN_HEIGHT - SLIDE_ZONE)) {
            press_start_y = point.y;
            panel_start_y = lv_obj_get_y(page);
            dragging = true;
            printf("PRESSED - touch_y: %d, panel_y: %ld\n", point.y, panel_start_y);
        } else {
            dragging = false; // 不在滑动区
        }
    }

    if(code == LV_EVENT_PRESSING && dragging) {
        int32_t dy = point.y - press_start_y;
    
        if(abs(dy) > 2) {
            int32_t new_y = panel_start_y + dy;
            new_y = LV_CLAMP(-240, new_y, 0);
            lv_obj_set_y(page, new_y);
            printf("DRAGGING - dy: %ld, new_y: %ld\n", dy, new_y);
        }
    }

    if(code == LV_EVENT_RELEASED && dragging) {
        dragging = false;
        int32_t current_y = lv_obj_get_y(page);
        int32_t dy = current_y - panel_start_y;
    
        printf("current_y:%ld press_start_y:%ld panel_start_y:%ld dy:%ld\n", current_y, press_start_y, panel_start_y, dy);
    
        // 优先判断是否是非主页、状态栏收起、上滑（或拉到了最顶），起始位置在底部
        if(!is_home_page() && current_y == -240 && press_start_y > (SCREEN_HEIGHT - SLIDE_ZONE)) {
            printf("Swipe up from bottom on non-home page -> Back to Home cur_page:%d\n",guider_ui.page_flage);
            
            if(guider_ui.page_flage == FILE_FOLDER_PAGE) {
                //guider_ui.page_flage = HOME_PAGE;
                printf("file_folder_page -> Back to Home\n");
                ui_load_scr_animation(&guider_ui, &guider_ui.screen, guider_ui.screen_del, &guider_ui.file_folder_page_del, setup_scr_screen, LV_SCR_LOAD_ANIM_OVER_BOTTOM, 200, 200, true, true);
            } else if(guider_ui.page_flage == TIME_PAGE) {
                //guider_ui.page_flage = HOME_PAGE;
                printf("time_page -> Back to Home\n");
                ui_load_scr_animation(&guider_ui, &guider_ui.screen, guider_ui.screen_del, &guider_ui.time_del, setup_scr_screen, LV_SCR_LOAD_ANIM_OVER_BOTTOM, 200, 200, true, true);
            }else if(guider_ui.page_flage == CAMERA_PAGE) {
                guider_ui.page_flage = HOME_PAGE;
                printf("camera_page -> Back to Home\n");
                vTaskDelay(pdMS_TO_TICKS(200));   // 根据任务周期调整
                ui_load_scr_animation(&guider_ui, &guider_ui.screen, guider_ui.screen_del, &guider_ui.camera_del, setup_scr_screen, LV_SCR_LOAD_ANIM_OVER_BOTTOM, 200, 200, true, true);
            }else if(guider_ui.page_flage == WIFI_SET_PAGE) {
                guider_ui.page_flage = HOME_PAGE;
                printf("wifi_page -> Back to Home\n");
                vTaskDelay(pdMS_TO_TICKS(200));   // 根据任务周期调整
                ui_load_scr_animation(&guider_ui, &guider_ui.screen, guider_ui.screen_del, &guider_ui.wifi_set_page_del, setup_scr_screen, LV_SCR_LOAD_ANIM_OVER_BOTTOM, 200, 200, true, true);
            }else if(guider_ui.page_flage == BLUETOOTH_PAGE) {
                guider_ui.page_flage = HOME_PAGE;
                printf("bluetooth_page -> Back to Home\n");
                vTaskDelay(pdMS_TO_TICKS(200));   // 根据任务周期调整
                ui_load_scr_animation(&guider_ui, &guider_ui.screen, guider_ui.screen_del, &guider_ui.bluetooth_page_del, setup_scr_screen, LV_SCR_LOAD_ANIM_OVER_BOTTOM, 200, 200, true, true);
            }else if(guider_ui.page_flage == SETTING_PAGE) {
                guider_ui.page_flage = HOME_PAGE;
                printf("setting_page -> Back to Home\n");
                vTaskDelay(pdMS_TO_TICKS(200));   // 根据任务周期调整
                ui_load_scr_animation(&guider_ui, &guider_ui.screen, guider_ui.screen_del, &guider_ui.setting_page_del, setup_scr_screen, LV_SCR_LOAD_ANIM_OVER_BOTTOM, 200, 200, true, true);
            }
    
            return;
        }
    
        // 如果滑动距离太小，就认为是点击
        if(abs(dy) < 5) {
            printf("Tap detected, ignore drag animation\n");
            return;
        }
    
        // 正常展开/收起动画
        lv_anim_t a;
        lv_anim_init(&a);
        lv_anim_set_var(&a, page);
        lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_y);
        lv_anim_set_time(&a, 300);
    
        if(dy > 0) {
            lv_anim_set_values(&a, current_y, 0);  // 展开
            printf("Swipe down -> Expand\n");
        } else {
            lv_anim_set_values(&a, current_y, -240);  // 收起
            printf("Swipe up -> Collapse\n");
        }
    
        lv_anim_start(&a);
    }
}

static void screen_img_3_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_SHORT_CLICKED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.file_folder_page, guider_ui.file_folder_page_del, &guider_ui.screen_del, setup_scr_file_folder_page, LV_SCR_LOAD_ANIM_OVER_BOTTOM, 200, 0, true, true);
        break;
    }
    default:
        break;
    }
}

static void screen_img_4_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_SHORT_CLICKED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.camera, guider_ui.camera_del, &guider_ui.screen_del, setup_scr_camera, LV_SCR_LOAD_ANIM_OVER_BOTTOM, 200, 0, true, true);
        break;
    }
    default:
        break;
    }
}

static void screen_img_6_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_SHORT_CLICKED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.time, guider_ui.time_del, &guider_ui.screen_del, setup_scr_time, LV_SCR_LOAD_ANIM_MOVE_TOP, 200, 0, false, false);
        break;
    }
    default:
        break;
    }
}



void events_init_screen (lv_ui *ui)
{
    //lv_obj_add_event_cb(ui->screen, screen_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_img_3, screen_img_3_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_img_4, screen_img_4_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_img_6, screen_img_6_event_handler, LV_EVENT_ALL, ui);
}

static void file_folder_page_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        lv_obj_add_flag(guider_ui.file_folder_page_list_2, LV_OBJ_FLAG_HIDDEN);
        break;
    }
    default:
        break;
    }
}


bool path_join(char *dst, size_t size, const char *base, const char *sub) {
    if (!dst || !base || !sub || size == 0) return false;

    // 如果是根目录
    if (strcmp(base, "/") == 0) {
        return snprintf(dst, size, "/%s", sub) < size;
    }

    // 一般目录拼接 base/sub
    return snprintf(dst, size, "%s/%s", base, sub) < size;
}

static void file_folder_page_list_1_item0_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);

    switch (code) {
    case LV_EVENT_CLICKED:
    {
        const char *text = lv_list_get_btn_text(guider_ui.file_folder_page_list_1, obj);

        if (text) {
            // 判断是否是文件夹（假设没有扩展名）
            if (strchr(text, '.') == NULL) {
                char new_path[256];

                if (path_join(new_path, sizeof(new_path), guider_ui.s_current_path, text)) {
                    strncpy(guider_ui.s_current_path, new_path, sizeof(guider_ui.s_current_path) - 1);
                    guider_ui.s_current_path[sizeof(guider_ui.s_current_path) - 1] = '\0';
                    refresh_file_list1(&guider_ui);
                } else {
                    ESP_LOGW(TAG, "路径过长拼接失败: %s + / + %s", guider_ui.s_current_path, text);
                }
                ESP_LOGD(TAG, "拼接后路径 guider_ui.s_current_path: %s", guider_ui.s_current_path);
            }
            else
            {
                char new_path[256];
                if (path_join(new_path, sizeof(new_path), guider_ui.s_current_path, text)) {
                    strncpy(guider_ui.s_current_path, new_path, sizeof(guider_ui.s_current_path) - 1);
                    guider_ui.s_current_path[sizeof(guider_ui.s_current_path) - 1] = '\0';
                    ESP_LOGI(TAG,"要打开文件路径guider_ui.s_current_path:%s\n",guider_ui.s_current_path);
                    view_file(guider_ui.s_current_path);
                } else {
                    ESP_LOGW(TAG, "路径过长拼接失败: %s + / + %s", guider_ui.s_current_path, text);
                }
                ESP_LOGD(TAG, "拼接后路径 guider_ui.s_current_path: %s", guider_ui.s_current_path);
            }
        }
        break;
    }
    default:
        break;
    }
}

void file_folder_page_list_btn_event_handler(lv_event_t *e)
{
    file_folder_page_list_1_item0_event_handler(e);
}

static void file_folder_page_list_1_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        lv_obj_add_flag(guider_ui.file_folder_page_list_2, LV_OBJ_FLAG_HIDDEN);
        break;
    }
    default:
        break;
    }
}

static void file_folder_page_btn_1_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        lv_obj_clear_flag(guider_ui.file_folder_page_list_2, LV_OBJ_FLAG_HIDDEN);
        break;
    }
    default:
        break;
    }
}

static void file_folder_page_btn_back_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ESP_LOGI(TAG,"guider_ui.s_current_path:%s\n",guider_ui.s_current_path);
        // 返回上级目录
        if(guider_ui.view_file_flag != NO_VIEW_FILE)
        {
            lv_obj_del(guider_ui.file_folder_page_view_file_container);
            guider_ui.file_folder_page_view_file_container = NULL;
            guider_ui.view_file_flag = NO_VIEW_FILE;
        }
        char *last_slash = strrchr(guider_ui.s_current_path, '/');
        if (last_slash && last_slash != guider_ui.s_current_path) {
            *last_slash = '\0';
        } else {
            strcpy(guider_ui.s_current_path, "/");
        }
        refresh_file_list1(&guider_ui);
        break;
    }
    default:
        break;
    }
}

void events_init_file_folder_page (lv_ui *ui)
{
    lv_obj_add_event_cb(ui->file_folder_page, file_folder_page_event_handler, LV_EVENT_ALL, ui);
    printf("ui->file_folder_page_list_1_item_index:%d",ui->file_folder_page_list_1_item_index);
    for(int i =0;i < ui->file_folder_page_list_1_item_index ;i++)
    {
        if (ui->file_folder_page_list_1_item[i]) {
            lv_obj_add_event_cb(ui->file_folder_page_list_1_item[i], file_folder_page_list_1_item0_event_handler, LV_EVENT_ALL, ui);
        } else {
            printf("⚠️ file_folder_page_list_1_item[%d] is NULL!\n", i);
        }
    }

    lv_obj_add_event_cb(ui->file_folder_page_list_1, file_folder_page_list_1_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->file_folder_page_btn_1, file_folder_page_btn_1_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->file_folder_page_btn_back, file_folder_page_btn_back_event_handler, LV_EVENT_ALL, ui);
}

static void time_btn_1_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_animation(guider_ui.time_cont_2, 0, 0, lv_obj_get_y(guider_ui.time_cont_2), 240, &lv_anim_path_ease_out, 0, 0, 300, 0, (lv_anim_exec_xcb_t)lv_obj_set_y, NULL, NULL, NULL);
        lv_obj_clear_flag(guider_ui.time_cont_2, LV_OBJ_FLAG_HIDDEN);
        break;
    }
    default:
        break;
    }
}

static void time_btn_4_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_animation(guider_ui.time_cont_2, 0, 0, lv_obj_get_y(guider_ui.time_cont_2), 240, &lv_anim_path_ease_out, 0, 0, 300, 0, (lv_anim_exec_xcb_t)lv_obj_set_y, NULL, NULL, NULL);
        lv_obj_add_flag(guider_ui.time_cont_2, LV_OBJ_FLAG_HIDDEN);
        break;
    }
    default:
        break;
    }
}

void events_init_time (lv_ui *ui)
{
    lv_obj_add_event_cb(ui->time_btn_1, time_btn_1_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->time_btn_4, time_btn_4_event_handler, LV_EVENT_ALL, ui);
}
extern bool wifi_flag;
static void wifi_set_page_sw_1_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        lv_obj_t * status_obj = lv_event_get_target(e);
        int status = lv_obj_has_state(status_obj, LV_STATE_CHECKED) ? true : false;
        ESP_LOGI(TAG,"ui->wifi_set_page_sw_1 1:%u",lv_obj_get_state(guider_ui.wifi_set_page_sw_1));
        switch (status) {
        case (true)://关 关闭wifi
        {
            ESP_LOGI(TAG,"ui->wifi_set_page_sw_1 2:%u",lv_obj_get_state(guider_ui.wifi_set_page_sw_1));
            lv_imgbtn_set_state(top.screen_top_wifi, LV_IMGBTN_STATE_RELEASED);
            esp_wifi_disconnect();
            wifi_flag= false;
            lv_label_set_text(guider_ui.wifi_set_page_label_4, "");
            lv_obj_add_flag(guider_ui.wifi_set_page_connect_sta, LV_OBJ_FLAG_HIDDEN);
            lv_obj_add_flag(guider_ui.wifi_set_page_label_3, LV_OBJ_FLAG_HIDDEN);//otherNetwork
            lv_obj_add_flag(guider_ui.wifi_set_page_cont_2, LV_OBJ_FLAG_HIDDEN);
            break;
        }
        case (false)://开
        {
            /*
                1.初始化wifi
                2.先重新获取wifi_set_page_label_4(保存在nvs的wifi)
                3.重新扫描wifi,显示扫描出来的wifi
            */
           lv_imgbtn_set_state(top.screen_top_wifi, LV_IMGBTN_STATE_CHECKED_RELEASED);
           esp_wifi_start();
            //app_wifi_init("adol-3466", "12345678"); // 初始化wifi
            save_wifi_info("adol-3466","12345678");
            char ssid[32];
            char password[64];
            lv_obj_clear_flag(guider_ui.wifi_set_page_connect_sta,LV_OBJ_FLAG_HIDDEN);
            if(read_wifi_info(ssid, sizeof(ssid), password, sizeof(password)) == ESP_OK)
            {
                ESP_LOGI(TAG,"SSID: %s\n", ssid);
                ESP_LOGI(TAG,"Password: %s\n", password);
                wifi_flag= true;
                
                lv_label_set_text(guider_ui.wifi_set_page_label_4, ssid);
                lv_obj_clear_flag(guider_ui.wifi_set_page_connect_sta, LV_OBJ_FLAG_HIDDEN);
            }
            else
            {
                lv_label_set_text(guider_ui.wifi_set_page_label_4, "");
                lv_obj_add_flag(guider_ui.wifi_set_page_connect_sta, LV_OBJ_FLAG_HIDDEN);
            }
            
            
            add_wifi_info_btn();
            lv_obj_clear_flag(guider_ui.wifi_set_page_label_3, LV_OBJ_FLAG_HIDDEN);//otherNetwork
            lv_obj_clear_flag(guider_ui.wifi_set_page_cont_2, LV_OBJ_FLAG_HIDDEN);
            break;
        }
        default:
            break;
        }
        break;
    }
    default:
        break;
    }
}

static void wifi_set_page_label_1_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.setting_page, guider_ui.setting_page_del, &guider_ui.wifi_set_page_del, setup_scr_setting_page, LV_SCR_LOAD_ANIM_OVER_LEFT, 200, 200, true, true);
        break;
    }
    default:
        break;
    }
}

void events_init_wifi_set_page (lv_ui *ui)
{
    lv_obj_add_event_cb(ui->wifi_set_page_sw_1, wifi_set_page_sw_1_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->wifi_set_page_label_1, wifi_set_page_label_1_event_handler, LV_EVENT_ALL, ui);
}

static void setting_page_cont_3_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.bluetooth_page, guider_ui.bluetooth_page_del, &guider_ui.setting_page_del, setup_scr_bluetooth_page, LV_SCR_LOAD_ANIM_OVER_LEFT, 300, 0, true, true);
        break;
    }
    case LV_EVENT_PRESSED:
    {
        lv_obj_set_style_bg_color(guider_ui.setting_page_cont_3, lv_color_hex(0xa8a4a4), LV_PART_MAIN);
        break;
    }
    case LV_EVENT_RELEASED:
    {
        lv_obj_set_style_bg_color(guider_ui.setting_page_cont_3, lv_color_hex(0xffffff), LV_PART_MAIN);
        break;
    }
    default:
        break;
    }
}

static void setting_page_cont_2_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.wifi_set_page, guider_ui.wifi_set_page_del, &guider_ui.setting_page_del, setup_scr_wifi_set_page, LV_SCR_LOAD_ANIM_OVER_LEFT, 200, 0, true, true);
        break;
    }
    case LV_EVENT_PRESSED:
    {
        lv_obj_set_style_bg_color(guider_ui.setting_page_cont_2, lv_color_hex(0xa39f9f), LV_PART_MAIN);
        break;
    }
    case LV_EVENT_RELEASED:
    {
        lv_obj_set_style_bg_color(guider_ui.setting_page_cont_2, lv_color_hex(0xffffff), LV_PART_MAIN);
        break;
    }
    default:
        break;
    }
}

void events_init_setting_page (lv_ui *ui)
{
    lv_obj_add_event_cb(ui->setting_page_cont_3, setting_page_cont_3_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->setting_page_cont_2, setting_page_cont_2_event_handler, LV_EVENT_ALL, ui);
}


void events_init(lv_ui *ui)
{

}
