/*
* Copyright 2025 NXP
* NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
* terms, then you may not retain, install, activate or otherwise use the software.
*/

#include "lvgl.h"
#include <stdio.h>
#include "gui_guider.h"
#include "events_init.h"
#include "widgets_init.h"
#include "custom.h"
#include "wifi.h"
#include "esp_log.h"
extern bool wifi_flag;

static const char *TAG = "setup_scr_setting_page";
void setup_scr_setting_page(lv_ui *ui)
{
    ui->page_flage = SETTING_PAGE;
    //Write codes setting_page
    ui->setting_page = lv_obj_create(NULL);
    lv_obj_set_size(ui->setting_page, 320, 240);
    lv_obj_set_scrollbar_mode(ui->setting_page, LV_SCROLLBAR_MODE_OFF);

    //Write style for setting_page, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->setting_page, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->setting_page, lv_color_hex(0xe8e8e8), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->setting_page, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes setting_page_label_1
    ui->setting_page_label_1 = lv_label_create(ui->setting_page);
    lv_label_set_text(ui->setting_page_label_1, "Setting");
    lv_label_set_long_mode(ui->setting_page_label_1, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->setting_page_label_1, 5, 4);
    lv_obj_set_size(ui->setting_page_label_1, 100, 32);

    //Write style for setting_page_label_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->setting_page_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->setting_page_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->setting_page_label_1, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->setting_page_label_1, &lv_font_montserratMedium_25, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->setting_page_label_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->setting_page_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->setting_page_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->setting_page_label_1, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->setting_page_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->setting_page_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->setting_page_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->setting_page_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->setting_page_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->setting_page_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes setting_page_cont_1
    ui->setting_page_cont_1 = lv_obj_create(ui->setting_page);
    lv_obj_set_pos(ui->setting_page_cont_1, 5, 30);
    lv_obj_set_size(ui->setting_page_cont_1, 309, 205);
    lv_obj_set_scrollbar_mode(ui->setting_page_cont_1, LV_SCROLLBAR_MODE_OFF);
    lv_obj_add_flag(ui->setting_page_cont_1, LV_OBJ_FLAG_EVENT_BUBBLE);

    //Write style for setting_page_cont_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->setting_page_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->setting_page_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->setting_page_cont_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->setting_page_cont_1, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->setting_page_cont_1, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->setting_page_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->setting_page_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->setting_page_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->setting_page_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->setting_page_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes setting_page_cont_3
    ui->setting_page_cont_3 = lv_obj_create(ui->setting_page_cont_1);
    lv_obj_set_pos(ui->setting_page_cont_3, 0, 40);
    lv_obj_set_size(ui->setting_page_cont_3, 309, 40);
    lv_obj_set_scrollbar_mode(ui->setting_page_cont_3, LV_SCROLLBAR_MODE_OFF);
    lv_obj_add_flag(ui->setting_page_cont_3, LV_OBJ_FLAG_CLICKABLE);

    //Write style for setting_page_cont_3, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->setting_page_cont_3, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui->setting_page_cont_3, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->setting_page_cont_3, lv_color_hex(0xc0c0c0), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui->setting_page_cont_3, LV_BORDER_SIDE_BOTTOM, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->setting_page_cont_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->setting_page_cont_3, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->setting_page_cont_3, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->setting_page_cont_3, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->setting_page_cont_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->setting_page_cont_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->setting_page_cont_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->setting_page_cont_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->setting_page_cont_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes setting_page_img_2
    ui->setting_page_img_2 = lv_img_create(ui->setting_page_cont_3);
    lv_obj_add_flag(ui->setting_page_img_2, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->setting_page_img_2, &_lanya1_alpha_40x40);
    lv_img_set_pivot(ui->setting_page_img_2, 50,50);
    lv_img_set_angle(ui->setting_page_img_2, 0);
    lv_obj_set_pos(ui->setting_page_img_2, 0, -2);
    lv_obj_set_size(ui->setting_page_img_2, 40, 40);

    //Write style for setting_page_img_2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->setting_page_img_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->setting_page_img_2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->setting_page_img_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->setting_page_img_2, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes setting_page_label_3
    ui->setting_page_label_3 = lv_label_create(ui->setting_page_cont_3);
    lv_label_set_text(ui->setting_page_label_3, "Bluetooth");
    lv_label_set_long_mode(ui->setting_page_label_3, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->setting_page_label_3, 35, 3);
    lv_obj_set_size(ui->setting_page_label_3, 100, 32);

    //Write style for setting_page_label_3, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->setting_page_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->setting_page_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->setting_page_label_3, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->setting_page_label_3, &lv_font_montserratMedium_20, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->setting_page_label_3, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->setting_page_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->setting_page_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->setting_page_label_3, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->setting_page_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->setting_page_label_3, 6, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->setting_page_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->setting_page_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->setting_page_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->setting_page_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes setting_page_label_5
    ui->setting_page_label_5 = lv_label_create(ui->setting_page_cont_3);
    lv_label_set_text(ui->setting_page_label_5, ">");
    lv_label_set_long_mode(ui->setting_page_label_5, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->setting_page_label_5, 276, 0);
    lv_obj_set_size(ui->setting_page_label_5, 23, 34);

    //Write style for setting_page_label_5, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->setting_page_label_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->setting_page_label_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->setting_page_label_5, lv_color_hex(0xcccbcb), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->setting_page_label_5, &lv_font_montserratMedium_30, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->setting_page_label_5, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->setting_page_label_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->setting_page_label_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->setting_page_label_5, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->setting_page_label_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->setting_page_label_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->setting_page_label_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->setting_page_label_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->setting_page_label_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->setting_page_label_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes setting_page_cont_2
    ui->setting_page_cont_2 = lv_obj_create(ui->setting_page_cont_1);
    lv_obj_set_pos(ui->setting_page_cont_2, 0, 0);
    lv_obj_set_size(ui->setting_page_cont_2, 309, 40);
    lv_obj_set_scrollbar_mode(ui->setting_page_cont_2, LV_SCROLLBAR_MODE_OFF);
    lv_obj_add_flag(ui->setting_page_cont_2, LV_OBJ_FLAG_CLICKABLE);

    //Write style for setting_page_cont_2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->setting_page_cont_2, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui->setting_page_cont_2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->setting_page_cont_2, lv_color_hex(0xc0c0c0), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui->setting_page_cont_2, LV_BORDER_SIDE_BOTTOM, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->setting_page_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->setting_page_cont_2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->setting_page_cont_2, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->setting_page_cont_2, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->setting_page_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->setting_page_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->setting_page_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->setting_page_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->setting_page_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes setting_page_img_1
    ui->setting_page_img_1 = lv_img_create(ui->setting_page_cont_2);
    lv_obj_add_flag(ui->setting_page_img_1, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->setting_page_img_1, &_wifi4_alpha_40x40);
    lv_img_set_pivot(ui->setting_page_img_1, 50,50);
    lv_img_set_angle(ui->setting_page_img_1, 0);
    lv_obj_set_pos(ui->setting_page_img_1, 0, -2);
    lv_obj_set_size(ui->setting_page_img_1, 40, 40);

    //Write style for setting_page_img_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->setting_page_img_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->setting_page_img_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->setting_page_img_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->setting_page_img_1, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes setting_page_label_2
    ui->setting_page_label_2 = lv_label_create(ui->setting_page_cont_2);
    lv_label_set_text(ui->setting_page_label_2, "WIFI");
    lv_label_set_long_mode(ui->setting_page_label_2, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->setting_page_label_2, 24, 2);
    lv_obj_set_size(ui->setting_page_label_2, 100, 32);

    //Write style for setting_page_label_2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->setting_page_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->setting_page_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->setting_page_label_2, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->setting_page_label_2, &lv_font_montserratMedium_20, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->setting_page_label_2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->setting_page_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->setting_page_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->setting_page_label_2, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->setting_page_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->setting_page_label_2, 6, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->setting_page_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->setting_page_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->setting_page_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->setting_page_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes setting_page_label_4
    ui->setting_page_label_4 = lv_label_create(ui->setting_page_cont_2);
    lv_label_set_text(ui->setting_page_label_4, ">");
    lv_label_set_long_mode(ui->setting_page_label_4, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->setting_page_label_4, 277, 0);
    lv_obj_set_size(ui->setting_page_label_4, 22, 35);

    //Write style for setting_page_label_4, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->setting_page_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->setting_page_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->setting_page_label_4, lv_color_hex(0xcccbcb), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->setting_page_label_4, &lv_font_montserratMedium_30, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->setting_page_label_4, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->setting_page_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->setting_page_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->setting_page_label_4, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->setting_page_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->setting_page_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->setting_page_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->setting_page_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->setting_page_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->setting_page_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes setting_page_label_wifi_name

    ui->setting_page_label_wifi_name = lv_label_create(ui->setting_page_cont_2);
    lv_label_set_text(ui->setting_page_label_wifi_name, "");
    lv_label_set_long_mode(ui->setting_page_label_wifi_name, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->setting_page_label_wifi_name, 175, 4);
    lv_obj_set_size(ui->setting_page_label_wifi_name, 100, 32);
    if(wifi_flag)
    {
        wifi_ap_record_t ap_info;
        if(print_current_wifi_info(&ap_info) == ESP_OK)
        {
            char ssid_str[33]; // SSID 最长 32 字节 + 1 个结束符
            memcpy(ssid_str, ap_info.ssid, 32);
            ssid_str[32] = '\0'; // 确保字符串终止
            lv_label_set_text(ui->setting_page_label_wifi_name, ssid_str);
        }
        else
        {
            ESP_LOGE(TAG,"get current_wifi_info fail");
            lv_label_set_text(ui->setting_page_label_wifi_name, "");
        }
    }
    else
    {
        ESP_LOGE(TAG,"not wifi_connected");
        lv_label_set_text(ui->setting_page_label_wifi_name, "");
    }
    //Write style for setting_page_label_wifi_name, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->setting_page_label_wifi_name, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->setting_page_label_wifi_name, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->setting_page_label_wifi_name, lv_color_hex(0xcccbcb), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->setting_page_label_wifi_name, &lv_font_montserratMedium_20, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->setting_page_label_wifi_name, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->setting_page_label_wifi_name, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->setting_page_label_wifi_name, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->setting_page_label_wifi_name, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->setting_page_label_wifi_name, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->setting_page_label_wifi_name, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->setting_page_label_wifi_name, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->setting_page_label_wifi_name, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->setting_page_label_wifi_name, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->setting_page_label_wifi_name, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    //The custom code of setting_page.

    add_slide_trigger(ui->setting_page);
    //Update current screen layout.
    lv_obj_update_layout(ui->setting_page);

    //Init events for screen.
    events_init_setting_page(ui);
}
