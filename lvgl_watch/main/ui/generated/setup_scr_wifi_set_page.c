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
static const char *TAG = "setup_scr_wifi_set_page";

extern bool wifi_flag;
void add_wifi_info_btn();
void setup_scr_wifi_set_page(lv_ui *ui)
{
    ESP_LOGI(TAG,"setup_scr_wifi_set_page");
    //Write codes wifi_set_page
    ui->page_flage = WIFI_SET_PAGE;
    ui->wifi_set_page = lv_obj_create(NULL);
    lv_obj_set_size(ui->wifi_set_page, 320, 240);
    lv_obj_set_scrollbar_mode(ui->wifi_set_page, LV_SCROLLBAR_MODE_OFF);

    //Write style for wifi_set_page, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->wifi_set_page, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->wifi_set_page, lv_color_hex(0xdedede), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->wifi_set_page, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes wifi_set_page_cont_1
    ui->wifi_set_page_cont_1 = lv_obj_create(ui->wifi_set_page);
    lv_obj_set_pos(ui->wifi_set_page_cont_1, 4, 27);
    lv_obj_set_size(ui->wifi_set_page_cont_1, 310, 90);
    lv_obj_set_scrollbar_mode(ui->wifi_set_page_cont_1, LV_SCROLLBAR_MODE_OFF);

    //Write style for wifi_set_page_cont_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->wifi_set_page_cont_1, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui->wifi_set_page_cont_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->wifi_set_page_cont_1, lv_color_hex(0xfbfbfb), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui->wifi_set_page_cont_1, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->wifi_set_page_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->wifi_set_page_cont_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->wifi_set_page_cont_1, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->wifi_set_page_cont_1, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->wifi_set_page_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->wifi_set_page_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->wifi_set_page_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->wifi_set_page_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->wifi_set_page_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes wifi_set_page_STA
    ui->wifi_set_page_STA = lv_obj_create(ui->wifi_set_page_cont_1);
    lv_obj_set_pos(ui->wifi_set_page_STA, 20, 3);
    lv_obj_set_size(ui->wifi_set_page_STA, 260, 40);
    lv_obj_set_scrollbar_mode(ui->wifi_set_page_STA, LV_SCROLLBAR_MODE_OFF);

    //Write style for wifi_set_page_STA, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->wifi_set_page_STA, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui->wifi_set_page_STA, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->wifi_set_page_STA, lv_color_hex(0xd6d6d6), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui->wifi_set_page_STA, LV_BORDER_SIDE_BOTTOM, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->wifi_set_page_STA, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->wifi_set_page_STA, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->wifi_set_page_STA, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->wifi_set_page_STA, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->wifi_set_page_STA, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->wifi_set_page_STA, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->wifi_set_page_STA, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->wifi_set_page_STA, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->wifi_set_page_STA, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes wifi_set_page_label_2
    ui->wifi_set_page_label_2 = lv_label_create(ui->wifi_set_page_STA);
    lv_label_set_text(ui->wifi_set_page_label_2, "STA");
    lv_label_set_long_mode(ui->wifi_set_page_label_2, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->wifi_set_page_label_2, 9, 6);
    lv_obj_set_size(ui->wifi_set_page_label_2, 65, 30);

    //Write style for wifi_set_page_label_2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->wifi_set_page_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->wifi_set_page_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->wifi_set_page_label_2, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->wifi_set_page_label_2, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->wifi_set_page_label_2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->wifi_set_page_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->wifi_set_page_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->wifi_set_page_label_2, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->wifi_set_page_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->wifi_set_page_label_2, 8, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->wifi_set_page_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->wifi_set_page_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->wifi_set_page_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->wifi_set_page_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes wifi_set_page_sw_1
    ui->wifi_set_page_sw_1 = lv_switch_create(ui->wifi_set_page_STA);
    lv_obj_set_pos(ui->wifi_set_page_sw_1, 216, 6);
    lv_obj_set_size(ui->wifi_set_page_sw_1, 41, 23);
    if (wifi_flag) {
        lv_obj_clear_state(ui->wifi_set_page_sw_1, LV_STATE_CHECKED);
        
    } else {
        lv_obj_add_state(ui->wifi_set_page_sw_1, LV_STATE_CHECKED);
    }
    //Write style for wifi_set_page_sw_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->wifi_set_page_sw_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->wifi_set_page_sw_1, lv_color_hex(0x00b500), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->wifi_set_page_sw_1, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->wifi_set_page_sw_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->wifi_set_page_sw_1, 15, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->wifi_set_page_sw_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for wifi_set_page_sw_1, Part: LV_PART_INDICATOR, State: LV_STATE_CHECKED.
    lv_obj_set_style_bg_opa(ui->wifi_set_page_sw_1, 223, LV_PART_INDICATOR|LV_STATE_CHECKED);
    lv_obj_set_style_bg_color(ui->wifi_set_page_sw_1, lv_color_hex(0x13359a), LV_PART_INDICATOR|LV_STATE_CHECKED);
    lv_obj_set_style_bg_grad_dir(ui->wifi_set_page_sw_1, LV_GRAD_DIR_HOR, LV_PART_INDICATOR|LV_STATE_CHECKED);
    lv_obj_set_style_bg_grad_color(ui->wifi_set_page_sw_1, lv_color_hex(0x2195f6), LV_PART_INDICATOR|LV_STATE_CHECKED);
    lv_obj_set_style_bg_main_stop(ui->wifi_set_page_sw_1, 0, LV_PART_INDICATOR|LV_STATE_CHECKED);
    lv_obj_set_style_bg_grad_stop(ui->wifi_set_page_sw_1, 255, LV_PART_INDICATOR|LV_STATE_CHECKED);
    lv_obj_set_style_border_width(ui->wifi_set_page_sw_1, 0, LV_PART_INDICATOR|LV_STATE_CHECKED);

    //Write style for wifi_set_page_sw_1, Part: LV_PART_KNOB, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->wifi_set_page_sw_1, 255, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->wifi_set_page_sw_1, lv_color_hex(0xffffff), LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->wifi_set_page_sw_1, LV_GRAD_DIR_NONE, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->wifi_set_page_sw_1, 0, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->wifi_set_page_sw_1, 10, LV_PART_KNOB|LV_STATE_DEFAULT);

    char ssid[32];
    char password[64];
    if(read_wifi_info(ssid, sizeof(ssid), password, sizeof(password)) == ESP_OK && wifi_flag)
    {
        printf("SSID: %s\n", ssid);
        printf("Password: %s\n", password);
            //Write codes wifi_set_page_connect_sta
        ui->wifi_set_page_connect_sta = lv_obj_create(ui->wifi_set_page_cont_1);
        lv_obj_set_pos(ui->wifi_set_page_connect_sta, 20, 46);
        lv_obj_set_size(ui->wifi_set_page_connect_sta, 260, 40);
        lv_obj_set_scrollbar_mode(ui->wifi_set_page_connect_sta, LV_SCROLLBAR_MODE_OFF);

        //Write style for wifi_set_page_connect_sta, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
        lv_obj_set_style_border_width(ui->wifi_set_page_connect_sta, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
        lv_obj_set_style_border_opa(ui->wifi_set_page_connect_sta, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
        lv_obj_set_style_border_color(ui->wifi_set_page_connect_sta, lv_color_hex(0xd6d6d6), LV_PART_MAIN|LV_STATE_DEFAULT);
        lv_obj_set_style_border_side(ui->wifi_set_page_connect_sta, LV_BORDER_SIDE_BOTTOM, LV_PART_MAIN|LV_STATE_DEFAULT);
        lv_obj_set_style_radius(ui->wifi_set_page_connect_sta, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
        lv_obj_set_style_bg_opa(ui->wifi_set_page_connect_sta, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(ui->wifi_set_page_connect_sta, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
        lv_obj_set_style_bg_grad_dir(ui->wifi_set_page_connect_sta, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
        lv_obj_set_style_pad_top(ui->wifi_set_page_connect_sta, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
        lv_obj_set_style_pad_bottom(ui->wifi_set_page_connect_sta, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
        lv_obj_set_style_pad_left(ui->wifi_set_page_connect_sta, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
        lv_obj_set_style_pad_right(ui->wifi_set_page_connect_sta, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
        lv_obj_set_style_shadow_width(ui->wifi_set_page_connect_sta, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

        //Write codes wifi_set_page_label_4
        ui->wifi_set_page_label_4 = lv_label_create(ui->wifi_set_page_connect_sta);
        lv_label_set_text(ui->wifi_set_page_label_4, ssid);
        lv_label_set_long_mode(ui->wifi_set_page_label_4, LV_LABEL_LONG_WRAP);
        lv_obj_set_pos(ui->wifi_set_page_label_4, 9, 6);
        lv_obj_set_size(ui->wifi_set_page_label_4, 164, 31);

        //Write style for wifi_set_page_label_4, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
        lv_obj_set_style_border_width(ui->wifi_set_page_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
        lv_obj_set_style_radius(ui->wifi_set_page_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(ui->wifi_set_page_label_4, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
        lv_obj_set_style_text_font(ui->wifi_set_page_label_4, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
        lv_obj_set_style_text_opa(ui->wifi_set_page_label_4, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
        lv_obj_set_style_text_letter_space(ui->wifi_set_page_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
        lv_obj_set_style_text_line_space(ui->wifi_set_page_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
        lv_obj_set_style_text_align(ui->wifi_set_page_label_4, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
        lv_obj_set_style_bg_opa(ui->wifi_set_page_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
        lv_obj_set_style_pad_top(ui->wifi_set_page_label_4, 8, LV_PART_MAIN|LV_STATE_DEFAULT);
        lv_obj_set_style_pad_right(ui->wifi_set_page_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
        lv_obj_set_style_pad_bottom(ui->wifi_set_page_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
        lv_obj_set_style_pad_left(ui->wifi_set_page_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
        lv_obj_set_style_shadow_width(ui->wifi_set_page_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    }
    else
    {
        ui->wifi_set_page_connect_sta = lv_obj_create(ui->wifi_set_page_cont_1);
        lv_obj_set_pos(ui->wifi_set_page_connect_sta, 20, 46);
        lv_obj_set_size(ui->wifi_set_page_connect_sta, 260, 40);
        lv_obj_set_scrollbar_mode(ui->wifi_set_page_connect_sta, LV_SCROLLBAR_MODE_OFF);
        lv_obj_add_flag(ui->wifi_set_page_connect_sta,LV_OBJ_FLAG_HIDDEN);
        //Write style for wifi_set_page_connect_sta, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
        lv_obj_set_style_border_width(ui->wifi_set_page_connect_sta, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
        lv_obj_set_style_border_opa(ui->wifi_set_page_connect_sta, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
        lv_obj_set_style_border_color(ui->wifi_set_page_connect_sta, lv_color_hex(0xd6d6d6), LV_PART_MAIN|LV_STATE_DEFAULT);
        lv_obj_set_style_border_side(ui->wifi_set_page_connect_sta, LV_BORDER_SIDE_BOTTOM, LV_PART_MAIN|LV_STATE_DEFAULT);
        lv_obj_set_style_radius(ui->wifi_set_page_connect_sta, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
        lv_obj_set_style_bg_opa(ui->wifi_set_page_connect_sta, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(ui->wifi_set_page_connect_sta, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
        lv_obj_set_style_bg_grad_dir(ui->wifi_set_page_connect_sta, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
        lv_obj_set_style_pad_top(ui->wifi_set_page_connect_sta, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
        lv_obj_set_style_pad_bottom(ui->wifi_set_page_connect_sta, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
        lv_obj_set_style_pad_left(ui->wifi_set_page_connect_sta, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
        lv_obj_set_style_pad_right(ui->wifi_set_page_connect_sta, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
        lv_obj_set_style_shadow_width(ui->wifi_set_page_connect_sta, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

        //Write codes wifi_set_page_label_4
        ui->wifi_set_page_label_4 = lv_label_create(ui->wifi_set_page_connect_sta);
        //lv_label_set_text(ui->wifi_set_page_label_4, ssid);
        lv_label_set_long_mode(ui->wifi_set_page_label_4, LV_LABEL_LONG_WRAP);
        lv_obj_set_pos(ui->wifi_set_page_label_4, 9, 6);
        lv_obj_set_size(ui->wifi_set_page_label_4, 164, 31);

        //Write style for wifi_set_page_label_4, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
        lv_obj_set_style_border_width(ui->wifi_set_page_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
        lv_obj_set_style_radius(ui->wifi_set_page_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(ui->wifi_set_page_label_4, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
        lv_obj_set_style_text_font(ui->wifi_set_page_label_4, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
        lv_obj_set_style_text_opa(ui->wifi_set_page_label_4, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
        lv_obj_set_style_text_letter_space(ui->wifi_set_page_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
        lv_obj_set_style_text_line_space(ui->wifi_set_page_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
        lv_obj_set_style_text_align(ui->wifi_set_page_label_4, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
        lv_obj_set_style_bg_opa(ui->wifi_set_page_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
        lv_obj_set_style_pad_top(ui->wifi_set_page_label_4, 8, LV_PART_MAIN|LV_STATE_DEFAULT);
        lv_obj_set_style_pad_right(ui->wifi_set_page_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
        lv_obj_set_style_pad_bottom(ui->wifi_set_page_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
        lv_obj_set_style_pad_left(ui->wifi_set_page_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
        lv_obj_set_style_shadow_width(ui->wifi_set_page_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    }


    //Write codes wifi_set_page_label_1
    ui->wifi_set_page_label_1 = lv_label_create(ui->wifi_set_page);
    lv_label_set_text(ui->wifi_set_page_label_1, "set");
    lv_label_set_long_mode(ui->wifi_set_page_label_1, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->wifi_set_page_label_1, 29, 7);
    lv_obj_set_size(ui->wifi_set_page_label_1, 60, 20);
    lv_obj_add_flag(ui->wifi_set_page_label_1, LV_OBJ_FLAG_CLICKABLE);

    //Write style for wifi_set_page_label_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->wifi_set_page_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->wifi_set_page_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->wifi_set_page_label_1, lv_color_hex(0x2eb6fd), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->wifi_set_page_label_1, &lv_font_montserratMedium_20, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->wifi_set_page_label_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->wifi_set_page_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->wifi_set_page_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->wifi_set_page_label_1, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->wifi_set_page_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->wifi_set_page_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->wifi_set_page_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->wifi_set_page_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->wifi_set_page_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->wifi_set_page_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes wifi_set_page_label_3
    ui->wifi_set_page_label_3 = lv_label_create(ui->wifi_set_page);
    lv_label_set_text(ui->wifi_set_page_label_3, "otherNetwork");
    lv_label_set_long_mode(ui->wifi_set_page_label_3, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->wifi_set_page_label_3, -5, 123);
    lv_obj_set_size(ui->wifi_set_page_label_3, 142, 30);

    //Write style for wifi_set_page_label_3, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->wifi_set_page_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->wifi_set_page_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->wifi_set_page_label_3, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->wifi_set_page_label_3, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->wifi_set_page_label_3, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->wifi_set_page_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->wifi_set_page_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->wifi_set_page_label_3, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->wifi_set_page_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->wifi_set_page_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->wifi_set_page_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->wifi_set_page_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->wifi_set_page_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->wifi_set_page_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes wifi_set_page_cont_2
    ui->wifi_set_page_cont_2 = lv_obj_create(ui->wifi_set_page);
    lv_obj_set_pos(ui->wifi_set_page_cont_2, 4, 140);
    lv_obj_set_size(ui->wifi_set_page_cont_2, 310, 94);
    lv_obj_set_scrollbar_mode(ui->wifi_set_page_cont_2, LV_SCROLLBAR_MODE_OFF);
    lv_obj_add_flag(ui->wifi_set_page_cont_2, LV_OBJ_FLAG_EVENT_BUBBLE);

    //Write style for wifi_set_page_cont_2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->wifi_set_page_cont_2, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui->wifi_set_page_cont_2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->wifi_set_page_cont_2, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui->wifi_set_page_cont_2, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->wifi_set_page_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->wifi_set_page_cont_2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->wifi_set_page_cont_2, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->wifi_set_page_cont_2, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->wifi_set_page_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->wifi_set_page_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->wifi_set_page_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->wifi_set_page_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->wifi_set_page_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT); 
    if (!guider_ui.wifi_set_page_wifi_info_btn) {
        guider_ui.wifi_set_page_wifi_info_btn = malloc(sizeof(lv_scan_wifi_info_btn));
        if (!guider_ui.wifi_set_page_wifi_info_btn) {
            ESP_LOGE(TAG, "Memory allocation failed for wifi_set_page_wifi_info_btn");
        }
    }
    if(!wifi_flag)
    {
        lv_obj_add_flag(ui->wifi_set_page_cont_2,LV_OBJ_FLAG_HIDDEN);     
    }
    else
    {
        add_wifi_info_btn();
    }
    
    

    //The custom code of wifi_set_page.

    add_slide_trigger(ui->wifi_set_page);
    //Update current screen layout.
    lv_obj_update_layout(ui->wifi_set_page);

    //Init events for screen.
    events_init_wifi_set_page(ui);
}
extern wifi_ap_record_t g_ap_info[DEFAULT_SCAN_LIST_SIZE];
void add_wifi_info_btn()
{
    ESP_LOGE(TAG, "add_wifi_info_btn");
    uint16_t number = DEFAULT_SCAN_LIST_SIZE;
    lv_obj_clean(guider_ui.wifi_set_page_cont_2);
    if(guider_ui.wifi_set_page_wifi_info_btn) {
        memset(guider_ui.wifi_set_page_wifi_info_btn, 0, sizeof(lv_scan_wifi_info_btn));
    }
    //esp_wifi_stop();
    esp_wifi_disconnect();
    number = wifi_scan(g_ap_info);
    for (int i = 0; i < number; i++) {
        ESP_LOGI(TAG, "SSID \t\t%s", g_ap_info[i].ssid);
        ESP_LOGI(TAG, "RSSI \t\t%d", g_ap_info[i].rssi);
        char ssid_str[33]; // SSID 最长 32 字节 + 1 个结束符
        memcpy(ssid_str, g_ap_info[i].ssid, 32);
        ssid_str[32] = '\0'; // 确保字符串终止
        //Write codes wifi_set_page_connect_sta
        if(guider_ui.wifi_set_page_cont_2 == NULL)
        {
            ESP_LOGE(TAG, "guider_ui.wifi_set_page_cont_2 == NULL");
            return;
        }
        guider_ui.wifi_set_page_wifi_info_btn->wifi_set_page_wifi_scan_info[i] = lv_obj_create(guider_ui.wifi_set_page_cont_2);
        lv_obj_set_pos(guider_ui.wifi_set_page_wifi_info_btn->wifi_set_page_wifi_scan_info[i], 20, 4+i*41);
        lv_obj_set_size(guider_ui.wifi_set_page_wifi_info_btn->wifi_set_page_wifi_scan_info[i], 260, 40);
        lv_obj_set_scrollbar_mode(guider_ui.wifi_set_page_wifi_info_btn->wifi_set_page_wifi_scan_info[i], LV_SCROLLBAR_MODE_OFF);

        //Write style for wifi_set_page_connect_sta, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
        lv_obj_set_style_border_width(guider_ui.wifi_set_page_wifi_info_btn->wifi_set_page_wifi_scan_info[i], 2, LV_PART_MAIN|LV_STATE_DEFAULT);
        lv_obj_set_style_border_opa(guider_ui.wifi_set_page_wifi_info_btn->wifi_set_page_wifi_scan_info[i], 255, LV_PART_MAIN|LV_STATE_DEFAULT);
        lv_obj_set_style_border_color(guider_ui.wifi_set_page_wifi_info_btn->wifi_set_page_wifi_scan_info[i], lv_color_hex(0xd6d6d6), LV_PART_MAIN|LV_STATE_DEFAULT);
        lv_obj_set_style_border_side(guider_ui.wifi_set_page_wifi_info_btn->wifi_set_page_wifi_scan_info[i], LV_BORDER_SIDE_BOTTOM, LV_PART_MAIN|LV_STATE_DEFAULT);
        lv_obj_set_style_radius(guider_ui.wifi_set_page_wifi_info_btn->wifi_set_page_wifi_scan_info[i], 0, LV_PART_MAIN|LV_STATE_DEFAULT);
        lv_obj_set_style_bg_opa(guider_ui.wifi_set_page_wifi_info_btn->wifi_set_page_wifi_scan_info[i], 255, LV_PART_MAIN|LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(guider_ui.wifi_set_page_wifi_info_btn->wifi_set_page_wifi_scan_info[i], lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
        lv_obj_set_style_bg_grad_dir(guider_ui.wifi_set_page_wifi_info_btn->wifi_set_page_wifi_scan_info[i], LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
        lv_obj_set_style_pad_top(guider_ui.wifi_set_page_wifi_info_btn->wifi_set_page_wifi_scan_info[i], 0, LV_PART_MAIN|LV_STATE_DEFAULT);
        lv_obj_set_style_pad_bottom(guider_ui.wifi_set_page_wifi_info_btn->wifi_set_page_wifi_scan_info[i], 0, LV_PART_MAIN|LV_STATE_DEFAULT);
        lv_obj_set_style_pad_left(guider_ui.wifi_set_page_wifi_info_btn->wifi_set_page_wifi_scan_info[i], 0, LV_PART_MAIN|LV_STATE_DEFAULT);
        lv_obj_set_style_pad_right(guider_ui.wifi_set_page_wifi_info_btn->wifi_set_page_wifi_scan_info[i], 0, LV_PART_MAIN|LV_STATE_DEFAULT);
        lv_obj_set_style_shadow_width(guider_ui.wifi_set_page_wifi_info_btn->wifi_set_page_wifi_scan_info[i], 0, LV_PART_MAIN|LV_STATE_DEFAULT);

        //Write codes wifi_set_page_label_4
        guider_ui.wifi_set_page_wifi_info_btn->wifi_set_page_label_wifi_scan_info_ssid[i] = lv_label_create(guider_ui.wifi_set_page_wifi_info_btn->wifi_set_page_wifi_scan_info[i]);
        lv_label_set_text(guider_ui.wifi_set_page_wifi_info_btn->wifi_set_page_label_wifi_scan_info_ssid[i], ssid_str);
        lv_label_set_long_mode(guider_ui.wifi_set_page_wifi_info_btn->wifi_set_page_label_wifi_scan_info_ssid[i], LV_LABEL_LONG_WRAP);
        lv_obj_set_pos(guider_ui.wifi_set_page_wifi_info_btn->wifi_set_page_label_wifi_scan_info_ssid[i], 9, 6);
        lv_obj_set_size(guider_ui.wifi_set_page_wifi_info_btn->wifi_set_page_label_wifi_scan_info_ssid[i], 164, 31);

        //Write style for wifi_set_page_label_4, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
        lv_obj_set_style_border_width(guider_ui.wifi_set_page_wifi_info_btn->wifi_set_page_label_wifi_scan_info_ssid[i], 0, LV_PART_MAIN|LV_STATE_DEFAULT);
        lv_obj_set_style_radius(guider_ui.wifi_set_page_wifi_info_btn->wifi_set_page_label_wifi_scan_info_ssid[i], 0, LV_PART_MAIN|LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(guider_ui.wifi_set_page_wifi_info_btn->wifi_set_page_label_wifi_scan_info_ssid[i], lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
        lv_obj_set_style_text_font(guider_ui.wifi_set_page_wifi_info_btn->wifi_set_page_label_wifi_scan_info_ssid[i], &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
        lv_obj_set_style_text_opa(guider_ui.wifi_set_page_wifi_info_btn->wifi_set_page_label_wifi_scan_info_ssid[i], 255, LV_PART_MAIN|LV_STATE_DEFAULT);
        lv_obj_set_style_text_letter_space(guider_ui.wifi_set_page_wifi_info_btn->wifi_set_page_label_wifi_scan_info_ssid[i], 0, LV_PART_MAIN|LV_STATE_DEFAULT);
        lv_obj_set_style_text_line_space(guider_ui.wifi_set_page_wifi_info_btn->wifi_set_page_label_wifi_scan_info_ssid[i], 0, LV_PART_MAIN|LV_STATE_DEFAULT);
        lv_obj_set_style_text_align(guider_ui.wifi_set_page_wifi_info_btn->wifi_set_page_label_wifi_scan_info_ssid[i], LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
        lv_obj_set_style_bg_opa(guider_ui.wifi_set_page_wifi_info_btn->wifi_set_page_label_wifi_scan_info_ssid[i], 0, LV_PART_MAIN|LV_STATE_DEFAULT);
        lv_obj_set_style_pad_top(guider_ui.wifi_set_page_wifi_info_btn->wifi_set_page_label_wifi_scan_info_ssid[i], 8, LV_PART_MAIN|LV_STATE_DEFAULT);
        lv_obj_set_style_pad_right(guider_ui.wifi_set_page_wifi_info_btn->wifi_set_page_label_wifi_scan_info_ssid[i], 0, LV_PART_MAIN|LV_STATE_DEFAULT);
        lv_obj_set_style_pad_bottom(guider_ui.wifi_set_page_wifi_info_btn->wifi_set_page_label_wifi_scan_info_ssid[i], 0, LV_PART_MAIN|LV_STATE_DEFAULT);
        lv_obj_set_style_pad_left(guider_ui.wifi_set_page_wifi_info_btn->wifi_set_page_label_wifi_scan_info_ssid[i], 0, LV_PART_MAIN|LV_STATE_DEFAULT);
        lv_obj_set_style_shadow_width(guider_ui.wifi_set_page_wifi_info_btn->wifi_set_page_label_wifi_scan_info_ssid[i], 0, LV_PART_MAIN|LV_STATE_DEFAULT);  
    }
    esp_wifi_start();
    guider_ui.wifi_set_page_wifi_info_btn->wifi_set_page_label_wifi_scan_info_ssid_index = number;
    guider_ui.wifi_set_page_wifi_info_btn->wifi_set_page_wifi_scan_info_index = number;
}