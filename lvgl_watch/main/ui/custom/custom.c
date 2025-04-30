/*
* Copyright 2023 NXP
* NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
* terms, then you may not retain, install, activate or otherwise use the software.
*/


/*********************
 *      INCLUDES
 *********************/
#include <stdio.h>
#include "lvgl.h"
#include "custom.h"
#include "events_init.h"
#include "gui_guider.h"
#include "esp_log.h"
#include "wifi.h"
#include "esp32_s3_szp.h"
static const char *TAG = "custom";
/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/
extern lv_ui guider_ui;
extern bool wifi_flag;
lv_ui_top top;

static lv_obj_t *control_center_panel;
static lv_obj_t *slider;


void brightness_slider_event_cb(lv_event_t *e)
{
    lv_obj_t *slider = lv_event_get_target(e);
    int value = lv_slider_get_value(slider);

    // 这里你可以调用自己的亮度控制函数
    LV_LOG_USER("当前亮度: %d", value);
}

void show_control_center(bool show)
{
    if (show) {
        lv_anim_t a;
        lv_anim_init(&a);
        lv_anim_set_var(&a, control_center_panel);
        lv_anim_set_values(&a, -240, 0); // 从上往下滑出
        lv_anim_set_time(&a, 300);
        lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_y);
        lv_anim_start(&a);
    } else {
        lv_anim_t a;
        lv_anim_init(&a);
        lv_anim_set_var(&a, control_center_panel);
        lv_anim_set_values(&a, 0, -240); // 向上滑回去
        lv_anim_set_time(&a, 300);
        lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_y);
        lv_anim_start(&a);
    }
}

void create_brightness_slider(lv_obj_t *parent)
{
    slider = lv_slider_create(parent);
    lv_obj_set_size(slider, 40, 200);  // 窄而高
    lv_obj_center(slider);
    lv_slider_set_range(slider, 0, 100);
    lv_slider_set_value(slider, 50, LV_ANIM_OFF);
    lv_obj_set_style_radius(slider, LV_RADIUS_CIRCLE, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(slider, LV_OPA_30, LV_PART_MAIN); // 半透明槽

    // 滑块样式
    static lv_style_t style_knob;
    lv_style_init(&style_knob);
    lv_style_set_radius(&style_knob, LV_RADIUS_CIRCLE);
    lv_style_set_bg_color(&style_knob, lv_palette_main(LV_PALETTE_YELLOW));
    lv_style_set_shadow_width(&style_knob, 10);
    lv_style_set_shadow_color(&style_knob, lv_color_black());
    lv_style_set_shadow_opa(&style_knob, LV_OPA_50);

    lv_obj_add_style(slider, &style_knob, LV_PART_KNOB);
    lv_obj_add_event_cb(slider, brightness_slider_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
}

void ui_create_control_center()
{
    // 创建控制中心背景面板（默认隐藏）
    control_center_panel = lv_obj_create(lv_scr_act());
    lv_obj_set_size(control_center_panel, 240, 240);
    lv_obj_set_style_bg_color(control_center_panel, lv_color_black(), 0);
    lv_obj_set_style_bg_opa(control_center_panel, LV_OPA_60, 0);
    lv_obj_set_style_radius(control_center_panel, 20, 0);
    lv_obj_set_y(control_center_panel, -240); // 初始隐藏（上滑出屏幕）

    create_brightness_slider(control_center_panel);
}

/**
 * Create a demo application
 */
void add_slide_trigger(lv_obj_t *page) {
    top.cur_page = page;


    //Write codes screen_cont_1
    top.top_state = lv_obj_create(page);
    lv_obj_set_pos(top.top_state, 0, -240);
    lv_obj_set_size(top.top_state, 320, 240);
    lv_obj_set_scrollbar_mode(top.top_state, LV_SCROLLBAR_MODE_OFF);
    lv_obj_add_flag(top.top_state, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_flag(top.top_state, LV_OBJ_FLAG_EVENT_BUBBLE);
    lv_obj_set_scroll_dir(top.top_state, LV_DIR_VER);  // 允许垂直滚动
    //Write style for screen_cont_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(top.top_state, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(top.top_state, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(top.top_state, lv_color_hex(0x2195f6), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(top.top_state, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(top.top_state, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(top.top_state, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(top.top_state, lv_color_hex(0x8e8f8e), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(top.top_state, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(top.top_state, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(top.top_state, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(top.top_state, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(top.top_state, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(top.top_state, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_slider_1
    top.page_slider_1 = lv_slider_create(top.top_state);
    lv_slider_set_range(top.page_slider_1, 0, 100);
    lv_slider_set_mode(top.page_slider_1, LV_SLIDER_MODE_NORMAL);
    lv_slider_set_value(top.page_slider_1, 0, LV_ANIM_OFF);
    // lv_obj_set_pos(top.page_slider_1, 233, -142);
    lv_obj_set_pos(top.page_slider_1, 203, 115);
    lv_obj_set_size(top.page_slider_1, 40, 100);

    // 主体样式（灰色背景）
    lv_style_init(&top.style_knob);
    lv_style_set_bg_color(&top.style_knob, lv_color_hex(0x383d3b));
    lv_style_set_bg_opa(&top.style_knob, LV_OPA_60);
    lv_style_set_radius(&top.style_knob, LV_RADIUS_CIRCLE);
    lv_obj_add_style(top.page_slider_1, &top.style_knob, LV_PART_MAIN);

    // ==== 设置 indicator 样式（白色填充） ====
    lv_style_init(&top.style_slider_indicator);
    lv_style_set_bg_color(&top.style_slider_indicator, lv_color_white());
    lv_style_set_bg_opa(&top.style_slider_indicator, LV_OPA_COVER);
    lv_style_set_radius(&top.style_slider_indicator, LV_RADIUS_CIRCLE);
    lv_obj_add_style(top.page_slider_1, &top.style_slider_indicator, LV_PART_INDICATOR);

    lv_style_init(&top.style_slider_knob);
    lv_style_set_bg_opa(&top.style_slider_knob, LV_OPA_TRANSP);  // 设置完全透明
    lv_obj_add_style(top.page_slider_1, &top.style_slider_knob, LV_PART_KNOB);

    // ==== 添加太阳图标 ====
    top.sun_icon = lv_label_create(top.page_slider_1);
    lv_label_set_text(top.sun_icon, LV_SYMBOL_SETTINGS);  // 使用 LVGL 内置符号
    lv_obj_set_style_text_font(top.sun_icon, LV_FONT_DEFAULT, 0); // 可换成大字体
    lv_obj_align_to(top.sun_icon, top.page_slider_1, LV_ALIGN_OUT_TOP_MID, 0, -10); // 图标在滑块上方

    //Write codes screen_slider_2
    top.page_slider_2 = lv_slider_create(top.top_state);
    lv_slider_set_range(top.page_slider_2, 0, 100);
    lv_slider_set_mode(top.page_slider_2, LV_SLIDER_MODE_NORMAL);
    lv_slider_set_value(top.page_slider_2, 0, LV_ANIM_OFF);
    // lv_obj_set_pos(top.page_slider_2, 275, -142);
    lv_obj_set_pos(top.page_slider_2, 266, 115);
    lv_obj_set_size(top.page_slider_2, 40, 100);

    // 主体样式（灰色背景）
    lv_style_init(&top.style_knob2);
    lv_style_set_bg_color(&top.style_knob2, lv_color_hex(0x383d3b));
    lv_style_set_bg_opa(&top.style_knob2, LV_OPA_60);
    lv_style_set_radius(&top.style_knob2, LV_RADIUS_CIRCLE);
    lv_obj_add_style(top.page_slider_2, &top.style_knob2, LV_PART_MAIN);

    // ==== 设置 indicator 样式（白色填充） ====
    lv_style_init(&top.style_slider_indicator2);
    lv_style_set_bg_color(&top.style_slider_indicator2, lv_color_white());
    lv_style_set_bg_opa(&top.style_slider_indicator2, LV_OPA_COVER);
    lv_style_set_radius(&top.style_slider_indicator2, LV_RADIUS_CIRCLE);
    lv_obj_add_style(top.page_slider_2, &top.style_slider_indicator2, LV_PART_INDICATOR);

    lv_style_init(&top.style_slider_knob2);
    lv_style_set_bg_opa(&top.style_slider_knob2, LV_OPA_TRANSP);  // 设置完全透明
    lv_obj_add_style(top.page_slider_2, &top.style_slider_knob2, LV_PART_KNOB);

    // ==== 添加太阳图标 ====
    top.sun_icon = lv_label_create(top.page_slider_2);
    lv_label_set_text(top.sun_icon, LV_SYMBOL_SETTINGS);  // 使用 LVGL 内置符号
    lv_obj_set_style_text_font(top.sun_icon, LV_FONT_DEFAULT, 0); // 可换成大字体
    lv_obj_align_to(top.sun_icon, top.page_slider_2, LV_ALIGN_OUT_TOP_MID, 0, -10); // 图标在滑块上方

    //Write codes screen_imgbtn_1
    top.screen_imgbtn_1 = lv_imgbtn_create(top.top_state);
    lv_obj_add_flag(top.screen_imgbtn_1, LV_OBJ_FLAG_CHECKABLE);
    lv_imgbtn_set_src(top.screen_imgbtn_1, LV_IMGBTN_STATE_RELEASED, NULL, &_ios_alpha_50x50, NULL);
    top.page_imgbtn_1_label = lv_label_create(top.screen_imgbtn_1);
    lv_label_set_text(top.page_imgbtn_1_label, "");
    lv_label_set_long_mode(top.page_imgbtn_1_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(top.page_imgbtn_1_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(top.screen_imgbtn_1, 0, LV_STATE_DEFAULT);
    // lv_obj_set_pos(top.screen_imgbtn_1, 17, -121);
    lv_obj_set_pos(top.screen_imgbtn_1, 9, 181);
    lv_obj_set_size(top.screen_imgbtn_1, 50, 50);
    lv_obj_add_flag(top.screen_imgbtn_1, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_flag(top.screen_imgbtn_1, LV_OBJ_FLAG_EVENT_BUBBLE);

    //Write style for screen_imgbtn_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_text_color(top.screen_imgbtn_1, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(top.screen_imgbtn_1, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(top.screen_imgbtn_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(top.screen_imgbtn_1, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(top.screen_imgbtn_1, 50, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(top.screen_imgbtn_1, true, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(top.screen_imgbtn_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_imgbtn_1, Part: LV_PART_MAIN, State: LV_STATE_PRESSED.
    lv_obj_set_style_img_recolor_opa(top.screen_imgbtn_1, 0, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_img_opa(top.screen_imgbtn_1, 255, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_color(top.screen_imgbtn_1, lv_color_hex(0xFF33FF), LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_font(top.screen_imgbtn_1, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_opa(top.screen_imgbtn_1, 255, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_shadow_width(top.screen_imgbtn_1, 0, LV_PART_MAIN|LV_STATE_PRESSED);

    //Write style for screen_imgbtn_1, Part: LV_PART_MAIN, State: LV_STATE_CHECKED.
    lv_obj_set_style_img_recolor_opa(top.screen_imgbtn_1, 0, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_img_opa(top.screen_imgbtn_1, 255, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_color(top.screen_imgbtn_1, lv_color_hex(0xFF33FF), LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_font(top.screen_imgbtn_1, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_opa(top.screen_imgbtn_1, 255, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_shadow_width(top.screen_imgbtn_1, 0, LV_PART_MAIN|LV_STATE_CHECKED);

    //Write style for screen_imgbtn_1, Part: LV_PART_MAIN, State: LV_IMGBTN_STATE_RELEASED.
    lv_obj_set_style_img_recolor_opa(top.screen_imgbtn_1, 0, LV_PART_MAIN|LV_IMGBTN_STATE_RELEASED);
    lv_obj_set_style_img_opa(top.screen_imgbtn_1, 255, LV_PART_MAIN|LV_IMGBTN_STATE_RELEASED);
    lv_obj_add_event_cb(page, screen_event_handler, LV_EVENT_ALL, top.top_state);
    //Write codes screen_top_state
    top.screen_top_state = lv_obj_create(top.top_state);
    lv_obj_set_pos(top.screen_top_state, 6, 11);
    lv_obj_set_size(top.screen_top_state, 146, 95);
    lv_obj_set_scrollbar_mode(top.screen_top_state, LV_SCROLLBAR_MODE_OFF);
    lv_obj_add_flag(top.screen_top_state, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_flag(top.screen_top_state, LV_OBJ_FLAG_EVENT_BUBBLE);

    //Write style for screen_top_state, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(top.screen_top_state, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(top.screen_top_state, 25, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(top.screen_top_state, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(top.screen_top_state, lv_color_hex(0x534e4e), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(top.screen_top_state, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(top.screen_top_state, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(top.screen_top_state, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(top.screen_top_state, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(top.screen_top_state, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(top.screen_top_state, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_top_wifi
    top.screen_top_wifi = lv_imgbtn_create(top.screen_top_state);
    lv_obj_add_flag(top.screen_top_wifi, LV_OBJ_FLAG_CHECKABLE);
    lv_imgbtn_set_src(top.screen_top_wifi, LV_IMGBTN_STATE_RELEASED, NULL, &_wifi_alpha_40x40, NULL);
    lv_imgbtn_set_src(top.screen_top_wifi, LV_IMGBTN_STATE_CHECKED_RELEASED, NULL, &_wifi2_alpha_40x40, NULL);
    top.screen_top_wifi_label = lv_label_create(top.screen_top_wifi);
    lv_label_set_text(top.screen_top_wifi_label, "");
    lv_label_set_long_mode(top.screen_top_wifi_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(top.screen_top_wifi_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(top.screen_top_wifi, 0, LV_STATE_DEFAULT);
    lv_obj_set_pos(top.screen_top_wifi, 19, 9);
    lv_obj_set_size(top.screen_top_wifi, 40, 40);
    lv_obj_add_flag(top.screen_top_wifi, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_flag(top.screen_top_wifi, LV_OBJ_FLAG_EVENT_BUBBLE);
    if(wifi_flag)
    {
        ESP_LOGI(TAG,"wifi状态：开");
        lv_imgbtn_set_state(top.screen_top_wifi, LV_IMGBTN_STATE_CHECKED_RELEASED);
    }
    else
    {
        ESP_LOGI(TAG,"wifi状态：关");
    }
    //Write style for screen_top_wifi, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_text_color(top.screen_top_wifi, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(top.screen_top_wifi, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(top.screen_top_wifi, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(top.screen_top_wifi, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(top.screen_top_wifi, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(top.screen_top_wifi, true, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(top.screen_top_wifi, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_top_wifi, Part: LV_PART_MAIN, State: LV_STATE_PRESSED.
    lv_obj_set_style_img_recolor_opa(top.screen_top_wifi, 0, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_img_opa(top.screen_top_wifi, 255, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_color(top.screen_top_wifi, lv_color_hex(0xFF33FF), LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_font(top.screen_top_wifi, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_opa(top.screen_top_wifi, 255, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_shadow_width(top.screen_top_wifi, 0, LV_PART_MAIN|LV_STATE_PRESSED);

    //Write style for screen_top_wifi, Part: LV_PART_MAIN, State: LV_STATE_CHECKED.
    lv_obj_set_style_img_recolor_opa(top.screen_top_wifi, 0, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_img_opa(top.screen_top_wifi, 255, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_color(top.screen_top_wifi, lv_color_hex(0xFF33FF), LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_font(top.screen_top_wifi, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_opa(top.screen_top_wifi, 255, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_shadow_width(top.screen_top_wifi, 0, LV_PART_MAIN|LV_STATE_CHECKED);

    //Write style for screen_top_wifi, Part: LV_PART_MAIN, State: LV_IMGBTN_STATE_RELEASED.
    lv_obj_set_style_img_recolor_opa(top.screen_top_wifi, 0, LV_PART_MAIN|LV_IMGBTN_STATE_RELEASED);
    lv_obj_set_style_img_opa(top.screen_top_wifi, 255, LV_PART_MAIN|LV_IMGBTN_STATE_RELEASED);

    //Write codes screen_1_imgbtn_3
    top.screen_top_bluetooth = lv_imgbtn_create(top.screen_top_state);
    lv_obj_add_flag(top.screen_top_bluetooth, LV_OBJ_FLAG_CHECKABLE);
    lv_imgbtn_set_src(top.screen_top_bluetooth, LV_IMGBTN_STATE_RELEASED, NULL, &_lanya_alpha_40x40, NULL);
    lv_imgbtn_set_src(top.screen_top_bluetooth, LV_IMGBTN_STATE_CHECKED_RELEASED, NULL, &_lanya2_alpha_40x40, NULL);
    top.screen_top_bluetooth_label = lv_label_create(top.screen_top_bluetooth);
    lv_label_set_text(top.screen_top_bluetooth_label, "");
    lv_label_set_long_mode(top.screen_top_bluetooth_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(top.screen_top_bluetooth_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(top.screen_top_bluetooth, 0, LV_STATE_DEFAULT);
    lv_obj_set_pos(top.screen_top_bluetooth, 77, 6);
    lv_obj_set_size(top.screen_top_bluetooth, 40, 40);

    //Write style for screen_1_imgbtn_3, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_text_color(top.screen_top_bluetooth, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(top.screen_top_bluetooth, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(top.screen_top_bluetooth, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(top.screen_top_bluetooth, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(top.screen_top_bluetooth, 25, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(top.screen_top_bluetooth, true, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(top.screen_top_bluetooth, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_1_imgbtn_3, Part: LV_PART_MAIN, State: LV_STATE_PRESSED.
    lv_obj_set_style_img_recolor_opa(top.screen_top_bluetooth, 0, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_img_opa(top.screen_top_bluetooth, 255, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_color(top.screen_top_bluetooth, lv_color_hex(0xFF33FF), LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_font(top.screen_top_bluetooth, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_opa(top.screen_top_bluetooth, 255, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_shadow_width(top.screen_top_bluetooth, 0, LV_PART_MAIN|LV_STATE_PRESSED);

    //Write style for screen_1_imgbtn_3, Part: LV_PART_MAIN, State: LV_STATE_CHECKED.
    lv_obj_set_style_img_recolor_opa(top.screen_top_bluetooth, 0, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_img_opa(top.screen_top_bluetooth, 255, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_color(top.screen_top_bluetooth, lv_color_hex(0xFF33FF), LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_font(top.screen_top_bluetooth, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_opa(top.screen_top_bluetooth, 255, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_shadow_width(top.screen_top_bluetooth, 0, LV_PART_MAIN|LV_STATE_CHECKED);

    //Write style for screen_1_imgbtn_3, Part: LV_PART_MAIN, State: LV_IMGBTN_STATE_RELEASED.
    lv_obj_set_style_img_recolor_opa(top.screen_top_bluetooth, 0, LV_PART_MAIN|LV_IMGBTN_STATE_RELEASED);
    lv_obj_set_style_img_opa(top.screen_top_bluetooth, 255, LV_PART_MAIN|LV_IMGBTN_STATE_RELEASED);


    //Write codes screen_imgbtn_10
    top.screen_top_set = lv_imgbtn_create(top.screen_top_state);
    lv_obj_add_flag(top.screen_top_set, LV_OBJ_FLAG_CHECKABLE);
    lv_imgbtn_set_src(top.screen_top_set, LV_IMGBTN_STATE_RELEASED, NULL, &_ios_settings1_alpha_40x40, NULL);
    top.screen_top_set_label = lv_label_create(top.screen_top_set);
    lv_label_set_text(top.screen_top_set_label, "");
    lv_label_set_long_mode(top.screen_top_set_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(top.screen_top_set_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(top.screen_top_set, 0, LV_STATE_DEFAULT);
    lv_obj_set_pos(top.screen_top_set, 20, 49);
    lv_obj_set_size(top.screen_top_set, 40, 40);
    lv_obj_add_flag(top.screen_top_set, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_flag(top.screen_top_set, LV_OBJ_FLAG_EVENT_BUBBLE);

    //Write style for screen_imgbtn_10, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_text_color(top.screen_top_set, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(top.screen_top_set, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(top.screen_top_set, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(top.screen_top_set, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(top.screen_top_set, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(top.screen_top_set, true, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(top.screen_top_set, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_imgbtn_10, Part: LV_PART_MAIN, State: LV_STATE_PRESSED.
    lv_obj_set_style_img_recolor_opa(top.screen_top_set, 0, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_img_opa(top.screen_top_set, 255, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_color(top.screen_top_set, lv_color_hex(0xFF33FF), LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_font(top.screen_top_set, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_opa(top.screen_top_set, 255, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_shadow_width(top.screen_top_set, 0, LV_PART_MAIN|LV_STATE_PRESSED);

    //Write style for screen_imgbtn_10, Part: LV_PART_MAIN, State: LV_STATE_CHECKED.
    lv_obj_set_style_img_recolor_opa(top.screen_top_set, 0, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_img_opa(top.screen_top_set, 255, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_color(top.screen_top_set, lv_color_hex(0xFF33FF), LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_font(top.screen_top_set, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_opa(top.screen_top_set, 255, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_shadow_width(top.screen_top_set, 0, LV_PART_MAIN|LV_STATE_CHECKED);

    //Write style for screen_imgbtn_10, Part: LV_PART_MAIN, State: LV_IMGBTN_STATE_RELEASED.
    lv_obj_set_style_img_recolor_opa(top.screen_top_set, 0, LV_PART_MAIN|LV_IMGBTN_STATE_RELEASED);
    lv_obj_set_style_img_opa(top.screen_top_set, 255, LV_PART_MAIN|LV_IMGBTN_STATE_RELEASED);




    //Write codes screen_top_weather
    top.screen_top_weather = lv_obj_create(top.top_state);
    lv_obj_set_pos(top.screen_top_weather, 170, 11);
    lv_obj_set_size(top.screen_top_weather, 146, 95);
    lv_obj_set_scrollbar_mode(top.screen_top_weather, LV_SCROLLBAR_MODE_OFF);
    lv_obj_add_flag(top.screen_top_weather, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_flag(top.screen_top_weather, LV_OBJ_FLAG_EVENT_BUBBLE);

    //Write style for screen_top_weather, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(top.screen_top_weather, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(top.screen_top_weather, 25, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(top.screen_top_weather, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(top.screen_top_weather, lv_color_hex(0x534e4e), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(top.screen_top_weather, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(top.screen_top_weather, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(top.screen_top_weather, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(top.screen_top_weather, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(top.screen_top_weather, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(top.screen_top_weather, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_imgbtn_9
    top.screen_imgbtn_9 = lv_imgbtn_create(top.screen_top_weather);
    lv_obj_add_flag(top.screen_imgbtn_9, LV_OBJ_FLAG_CHECKABLE);
    lv_imgbtn_set_src(top.screen_imgbtn_9, LV_IMGBTN_STATE_RELEASED, NULL, &_icon_test_22_alpha_40x40, NULL);
    top.screen_imgbtn_9_label = lv_label_create(top.screen_imgbtn_9);
    lv_label_set_text(top.screen_imgbtn_9_label, "");
    lv_label_set_long_mode(top.screen_imgbtn_9_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(top.screen_imgbtn_9_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(top.screen_imgbtn_9, 0, LV_STATE_DEFAULT);
    lv_obj_set_pos(top.screen_imgbtn_9, 15, 1);
    lv_obj_set_size(top.screen_imgbtn_9, 40, 40);

    //Write style for screen_imgbtn_9, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_text_color(top.screen_imgbtn_9, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(top.screen_imgbtn_9, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(top.screen_imgbtn_9, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(top.screen_imgbtn_9, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(top.screen_imgbtn_9, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(top.screen_imgbtn_9, true, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(top.screen_imgbtn_9, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_imgbtn_9, Part: LV_PART_MAIN, State: LV_STATE_PRESSED.
    lv_obj_set_style_img_recolor_opa(top.screen_imgbtn_9, 0, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_img_opa(top.screen_imgbtn_9, 255, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_color(top.screen_imgbtn_9, lv_color_hex(0xFF33FF), LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_font(top.screen_imgbtn_9, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_opa(top.screen_imgbtn_9, 255, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_shadow_width(top.screen_imgbtn_9, 0, LV_PART_MAIN|LV_STATE_PRESSED);

    //Write style for screen_imgbtn_9, Part: LV_PART_MAIN, State: LV_STATE_CHECKED.
    lv_obj_set_style_img_recolor_opa(top.screen_imgbtn_9, 0, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_img_opa(top.screen_imgbtn_9, 255, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_color(top.screen_imgbtn_9, lv_color_hex(0xFF33FF), LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_font(top.screen_imgbtn_9, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_opa(top.screen_imgbtn_9, 255, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_shadow_width(top.screen_imgbtn_9, 0, LV_PART_MAIN|LV_STATE_CHECKED);
    
    //Write style for screen_imgbtn_9, Part: LV_PART_MAIN, State: LV_IMGBTN_STATE_RELEASED.
    lv_obj_set_style_img_recolor_opa(top.screen_imgbtn_9, 0, LV_PART_MAIN|LV_IMGBTN_STATE_RELEASED);
    lv_obj_set_style_img_opa(top.screen_imgbtn_9, 255, LV_PART_MAIN|LV_IMGBTN_STATE_RELEASED);

    LV_FONT_DECLARE(my_Font_20);	
    //Write codes screen_top_label_region
    top.screen_top_label_region = lv_label_create(top.top_state);
    lv_label_set_text(top.screen_top_label_region, "金华市");
    lv_obj_set_style_text_font(top.screen_top_label_region, &my_Font_20, LV_STATE_DEFAULT);
    lv_label_set_long_mode(top.screen_top_label_region, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(top.screen_top_label_region, 231, 25);
    lv_obj_set_size(top.screen_top_label_region, 78, 28);

    //Write style for screen_top_label_region, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(top.screen_top_label_region, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(top.screen_top_label_region, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(top.screen_top_label_region, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    //lv_obj_set_style_text_font(top.screen_top_label_region, &lv_font_ArchitectsDaughter_20, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(top.screen_top_label_region, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(top.screen_top_label_region, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(top.screen_top_label_region, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(top.screen_top_label_region, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(top.screen_top_label_region, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(top.screen_top_label_region, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(top.screen_top_label_region, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(top.screen_top_label_region, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(top.screen_top_label_region, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(top.screen_top_label_region, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_top_label_temperature
    top.screen_top_label_temperature = lv_label_create(top.top_state);
    lv_label_set_text(top.screen_top_label_temperature, "22℃");
    lv_label_set_long_mode(top.screen_top_label_temperature, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(top.screen_top_label_temperature, 186, 65);
    lv_obj_set_size(top.screen_top_label_temperature, 123, 24);

    //Write style for screen_top_label_temperature, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(top.screen_top_label_temperature, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(top.screen_top_label_temperature, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(top.screen_top_label_temperature, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(top.screen_top_label_temperature, &my_Font_20, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(top.screen_top_label_temperature, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(top.screen_top_label_temperature, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(top.screen_top_label_temperature, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(top.screen_top_label_temperature, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(top.screen_top_label_temperature, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(top.screen_top_label_temperature, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(top.screen_top_label_temperature, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(top.screen_top_label_temperature, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(top.screen_top_label_temperature, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(top.screen_top_label_temperature, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    custom_init(&top);
}

bool is_home_page(void) {
    // 判断当前 page 是否是主页
    // 例如： return current_page == home_page;
    return  guider_ui.cur_page == guider_ui.screen;
}

void back_to_home(void) {
    // 执行返回主页的逻辑，例如隐藏当前页面，显示主页等
    // lv_obj_add_flag(current_page, LV_OBJ_FLAG_HIDDEN);
    // lv_obj_clear_flag(home_page, LV_OBJ_FLAG_HIDDEN);
    setup_scr_screen(&guider_ui);
    
}

static void screen_top_wifi_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_LONG_PRESSED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.wifi_set_page, guider_ui.wifi_set_page_del, &guider_ui.screen_del, setup_scr_wifi_set_page, LV_SCR_LOAD_ANIM_FADE_ON, 200, 200, false, true);
        break;
    }
    default:
        break;
    }
}

static void screen_top_set_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_LONG_PRESSED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.setting_page, guider_ui.setting_page_del, &guider_ui.screen_del, setup_scr_setting_page, LV_SCR_LOAD_ANIM_OVER_LEFT, 200, 200, true, true);
        break;
    }
    default:
        break;
    }
}

static void screen_top_bluetooth_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_LONG_PRESSED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.bluetooth_page, guider_ui.bluetooth_page_del, &guider_ui.screen_del, setup_scr_bluetooth_page, LV_SCR_LOAD_ANIM_OVER_LEFT, 200, 0, true, true);
        break;
    }
    default:
        break;
    }
}

void custom_init(lv_ui_top *top)
{
    
    /* Add your codes here */
    
    lv_obj_add_event_cb(top->screen_top_set, screen_top_set_event_handler, LV_EVENT_ALL, top);
    lv_obj_add_event_cb(top->screen_top_wifi, screen_top_wifi_event_handler, LV_EVENT_ALL, top);
    lv_obj_add_event_cb(top->screen_top_bluetooth, screen_top_bluetooth_event_handler, LV_EVENT_ALL, top);

}

//文件系统ui
#include "sd_card.h"


void refresh_file_list1(lv_ui *ui)
{
    lv_obj_clean(guider_ui.file_folder_page_list_1);
    memset(ui->file_folder_page_list_1_item, 0, sizeof(lv_obj_t*) * 256);
    ui->file_folder_page_list_1_item_index = 0;
    const char (*filelist)[256] = NULL;
    int filenum = sdcard_filelist_in_dir(&filelist, ui->s_current_path);
    for(int i = 0; i < filenum; i++) {
        add_file_list_btn(ui,*filelist);
        filelist++;
    }
    // 提取当前目录名
    const char *folder_name = strrchr(ui->s_current_path, '/');
    if (folder_name) {
        folder_name++;  // 跳过斜杠
        if (*folder_name == '\0') {
            // 如果是根目录"/"，则设置为"Root"
            lv_label_set_text(ui->file_folder_page_label_upper_level, "Root");
        } else {
            lv_label_set_text(ui->file_folder_page_label_upper_level, folder_name);
        }
    } else {
        // 如果没有'/'（理论上不可能），也设置为"Root"
        lv_label_set_text(ui->file_folder_page_label_upper_level, "Root");
    }

    // 如果不在根目录，显示返回按钮
    if (strcmp(ui->s_current_path, "/") != 0) {
        lv_obj_clear_flag(guider_ui.file_folder_page_btn_back, LV_OBJ_FLAG_HIDDEN);
    }
    else if (strcmp(ui->s_current_path, "/") == 0) {
        lv_obj_add_flag(guider_ui.file_folder_page_btn_back, LV_OBJ_FLAG_HIDDEN);
        show_file_list_main(ui);
    }

}

void show_file_list_main(lv_ui *ui)
{
    //Write codes file_folder_page_list_1
    ui->file_folder_page_list_1_item[0] = lv_list_add_btn(ui->file_folder_page_list_1, LV_SYMBOL_SD_CARD, "spiffs");
    lv_obj_set_size(ui->file_folder_page_list_1_item[0],318,40);
    ui->file_folder_page_list_1_item[1] = lv_list_add_btn(ui->file_folder_page_list_1, LV_SYMBOL_SD_CARD, "sdcard");
    lv_obj_set_size(ui->file_folder_page_list_1_item[1],318,40);
    //Write style state: LV_STATE_DEFAULT for &style_file_folder_page_list_1_extra_btns_main_default
    static lv_style_t style_file_folder_page_list_1_extra_btns_main_default;
    ui_init_style(&style_file_folder_page_list_1_extra_btns_main_default);

    lv_style_set_pad_top(&style_file_folder_page_list_1_extra_btns_main_default, 5);
    lv_style_set_pad_left(&style_file_folder_page_list_1_extra_btns_main_default, 5);
    lv_style_set_pad_right(&style_file_folder_page_list_1_extra_btns_main_default, 5);
    lv_style_set_pad_bottom(&style_file_folder_page_list_1_extra_btns_main_default, 5);
    lv_style_set_border_width(&style_file_folder_page_list_1_extra_btns_main_default, 2);
    
    lv_style_set_border_opa(&style_file_folder_page_list_1_extra_btns_main_default, 255);
    lv_style_set_border_color(&style_file_folder_page_list_1_extra_btns_main_default, lv_color_hex(0xd0d0d0));
    lv_style_set_border_side(&style_file_folder_page_list_1_extra_btns_main_default, LV_BORDER_SIDE_BOTTOM);

    lv_style_set_text_color(&style_file_folder_page_list_1_extra_btns_main_default, lv_color_hex(0x0D3055));
    lv_style_set_text_font(&style_file_folder_page_list_1_extra_btns_main_default, &lv_font_montserratMedium_16);
    lv_style_set_text_opa(&style_file_folder_page_list_1_extra_btns_main_default, 255);
    lv_style_set_radius(&style_file_folder_page_list_1_extra_btns_main_default, 3);
    lv_style_set_bg_opa(&style_file_folder_page_list_1_extra_btns_main_default, 255);
    lv_style_set_bg_color(&style_file_folder_page_list_1_extra_btns_main_default, lv_color_hex(0xffffff));
    lv_style_set_bg_grad_dir(&style_file_folder_page_list_1_extra_btns_main_default, LV_GRAD_DIR_NONE);
    lv_obj_add_style(ui->file_folder_page_list_1_item[0], &style_file_folder_page_list_1_extra_btns_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_add_style(ui->file_folder_page_list_1_item[1], &style_file_folder_page_list_1_extra_btns_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_add_event_cb(ui->file_folder_page_list_1_item[0], file_folder_page_list_btn_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->file_folder_page_list_1_item[1], file_folder_page_list_btn_event_handler, LV_EVENT_ALL, ui);
}

void add_file_list_btn(lv_ui *ui ,char *file_name)
{
    //Write codes file_folder_page_list_1
    // 默认图标
    const char *icon = LV_SYMBOL_FILE;  // 默认是文件
    // 判断文件类型
    if (strchr(file_name, '.') != NULL) {
        const char *extension = strrchr(file_name, '.');
        
        if (strcmp(extension, ".jpg") == 0 || strcmp(extension, ".png") == 0) {
            icon = LV_SYMBOL_IMAGE;  // 图片文件
        }
        else if (strcmp(extension, ".mp3") == 0 || strcmp(extension, ".wav") == 0) {
            icon = LV_SYMBOL_AUDIO;  // 音频文件
        }
        else if (strcmp(extension, ".txt") == 0) {
            icon = LV_SYMBOL_STOP;  // 文本文件
        }
        else if (strcmp(extension, ".zip") == 0 || strcmp(extension, ".rar") == 0) {
            icon = LV_SYMBOL_DOWNLOAD;  // 压缩文件
        }
    } else {
        // 没有扩展名，假设是文件夹
        icon = LV_SYMBOL_DIRECTORY;  // 文件夹
    }
    ui->file_folder_page_list_1_item[0] = lv_list_add_btn(ui->file_folder_page_list_1, icon, file_name);
    lv_obj_set_size(ui->file_folder_page_list_1_item[ui->file_folder_page_list_1_item_index],318,40);

    //Write style state: LV_STATE_DEFAULT for &style_file_folder_page_list_1_extra_btns_main_default
    static lv_style_t style_file_folder_page_list_1_extra_btns_main_default;
    ui_init_style(&style_file_folder_page_list_1_extra_btns_main_default);

    lv_style_set_pad_top(&style_file_folder_page_list_1_extra_btns_main_default, 5);
    lv_style_set_pad_left(&style_file_folder_page_list_1_extra_btns_main_default, 5);
    lv_style_set_pad_right(&style_file_folder_page_list_1_extra_btns_main_default, 5);
    lv_style_set_pad_bottom(&style_file_folder_page_list_1_extra_btns_main_default, 5);
    lv_style_set_border_width(&style_file_folder_page_list_1_extra_btns_main_default, 2);
    
    lv_style_set_border_opa(&style_file_folder_page_list_1_extra_btns_main_default, 255);
    lv_style_set_border_color(&style_file_folder_page_list_1_extra_btns_main_default, lv_color_hex(0xd0d0d0));
    lv_style_set_border_side(&style_file_folder_page_list_1_extra_btns_main_default, LV_BORDER_SIDE_BOTTOM);

    lv_style_set_text_color(&style_file_folder_page_list_1_extra_btns_main_default, lv_color_hex(0x0D3055));
    lv_style_set_text_font(&style_file_folder_page_list_1_extra_btns_main_default, &lv_font_montserratMedium_16);
    lv_style_set_text_opa(&style_file_folder_page_list_1_extra_btns_main_default, 255);
    lv_style_set_radius(&style_file_folder_page_list_1_extra_btns_main_default, 3);
    lv_style_set_bg_opa(&style_file_folder_page_list_1_extra_btns_main_default, 255);
    lv_style_set_bg_color(&style_file_folder_page_list_1_extra_btns_main_default, lv_color_hex(0xffffff));
    lv_style_set_bg_grad_dir(&style_file_folder_page_list_1_extra_btns_main_default, LV_GRAD_DIR_NONE);
    lv_obj_add_style(ui->file_folder_page_list_1_item[ui->file_folder_page_list_1_item_index], &style_file_folder_page_list_1_extra_btns_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_add_event_cb(ui->file_folder_page_list_1_item[ui->file_folder_page_list_1_item_index], file_folder_page_list_btn_event_handler, LV_EVENT_ALL, ui);
}

void view_file(char * file_name)
{
    if (guider_ui.file_folder_page_list_1) {
        lv_obj_clean(guider_ui.file_folder_page_list_1);
    }
    memset(guider_ui.file_folder_page_list_1_item, 0, sizeof(lv_obj_t*) * 256);
    guider_ui.file_folder_page_list_1_item_index = 0;
    // 创建显示容器
    // 防止重复创建时旧容器未清理干净
    if (guider_ui.file_folder_page_view_file_container) {
        lv_obj_del(guider_ui.file_folder_page_view_file_container);
        guider_ui.file_folder_page_view_file_container = NULL;
    }
    if (!guider_ui.file_folder_page) {
        ESP_LOGE(TAG, "file_folder_page is NULL!");
        return;
    }
    guider_ui.file_folder_page_view_file_container = lv_obj_create(guider_ui.file_folder_page);  // 或者你放到你的界面中 ui->某个页面上
    lv_obj_set_size(guider_ui.file_folder_page_view_file_container, 320, 200);
    lv_obj_set_pos(guider_ui.file_folder_page_view_file_container, 0, 40);
    lv_obj_set_style_bg_color(guider_ui.file_folder_page_view_file_container, lv_color_hex(0x000000), LV_PART_MAIN);  // 设置背景颜色可选
    lv_obj_move_foreground(guider_ui.file_folder_page_view_file_container);
    if(strstr(file_name, ".mjpeg"))
    {
        // 播放 mjpeg 视频
        // 你可以放一个动画 canvas 或逐帧播放逻辑
        guider_ui.view_file_flag = VIEW_FILE_MJPEG;
        ESP_LOGI(TAG, "播放 MJPEG 视频: %s", file_name);

        // 例如: mjpeg_play_on_canvas(container, file_name);
    }
    else if(strstr(file_name, ".txt"))
    {
        // 显示文本
        ESP_LOGI(TAG, "打开 TXT 文本: %s", file_name);
        guider_ui.view_file_flag = VIEW_FILE_TXT;
        lv_obj_t *label = lv_label_create(guider_ui.file_folder_page_view_file_container);
        lv_obj_set_style_text_color(label, lv_color_hex(0xfbfbfb), LV_PART_MAIN|LV_STATE_DEFAULT);
        lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP);  // 自动换行
        lv_obj_set_width(label, 300);
        lv_obj_align(label, LV_ALIGN_TOP_LEFT, 10, 10);
        // 示例：读取文本内容显示
        FILE *f = fopen(file_name, "r");
        if (f) {
            char buf[512] = {0};
            fread(buf, 1, sizeof(buf)-1, f);
            fclose(f);
            ESP_LOGI(TAG, "读取到文件%s的内容是:%s", file_name,buf);
            lv_label_set_text(label, buf);
        } else {
            lv_label_set_text(label, "无法打开文件");
        }
    }
    else if(strstr(file_name, ".jpg") || strstr(file_name, ".bmp") || strstr(file_name, ".png"))
    {
        // 显示图片
        ESP_LOGI(TAG, "显示图片: %s", file_name);
        guider_ui.view_file_flag = VIEW_FILE_JPG;
        lv_obj_t *img = lv_img_create(guider_ui.file_folder_page_view_file_container);
        lv_img_set_src(img, file_name);  
        lv_obj_center(img);
    }
    else if(strstr(file_name, ".gif"))
    {
        // 显示图片
        ESP_LOGI(TAG, "播放gif: %s", file_name);
        guider_ui.view_file_flag = VIEW_FILE_JPG;
        lv_obj_t *img = lv_gif_create(guider_ui.file_folder_page_view_file_container);
        lv_gif_set_src(img, file_name);  
        lv_obj_center(img);
    }
    else if(strstr(file_name, ".mp3"))
    {
        // 播放音乐（仅控制播放，没有可视内容）
        ESP_LOGI(TAG, "播放 MP3 音乐: %s", file_name);
        guider_ui.view_file_flag = VIEW_FILE_MP3;
        lv_obj_t *label = lv_label_create(guider_ui.file_folder_page_view_file_container);
        lv_label_set_text(label, "正在播放音乐...");
        lv_obj_center(label);

        // 调用音频播放函数
        // play_mp3(file_name);
    }
    else
    {
        ESP_LOGW(TAG, "未知文件类型: %s", file_name);
        guider_ui.view_file_flag = VIEW_FILE_UNDEFINED;
        lv_obj_t *label = lv_label_create(guider_ui.file_folder_page_view_file_container);
        lv_label_set_text(label, "无法识别的文件类型");
        lv_obj_center(label);
    }
}


lv_obj_t *gif_start;

// AI人出现在屏幕
void ai_gui_in(void)
{   
    lvgl_port_lock(0);
    gif_start = lv_gif_create(lv_scr_act());
    lv_gif_set_src(gif_start, "/img/disdain.gif");
    lv_obj_align(gif_start, LV_ALIGN_CENTER, 0, 0);
    lvgl_port_unlock();
}

// AI人退出屏幕
void ai_gui_out(void)
{
    lvgl_port_lock(0);
    lv_obj_del(gif_start);
    lvgl_port_unlock();
}