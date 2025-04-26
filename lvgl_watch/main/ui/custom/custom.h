/*
* Copyright 2023 NXP
* NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
* terms, then you may not retain, install, activate or otherwise use the software.
*/

#ifndef __CUSTOM_H_
#define __CUSTOM_H_
#ifdef __cplusplus
extern "C" {
#endif

#include "gui_guider.h"

typedef struct
{
	lv_obj_t *cur_page;
    lv_obj_t *top_state;
    lv_obj_t *page_slider_1;
    lv_obj_t *page_slider_2;
    lv_obj_t *page_imgbtn_1;
	lv_obj_t *page_imgbtn_1_label;
    lv_obj_t *screen_imgbtn_1;
    lv_style_t style_knob;
    lv_style_t style_slider_indicator;
    lv_style_t style_slider_knob;
    lv_obj_t *sun_icon;
    lv_obj_t *screen_top_brightness;
    lv_obj_t *screen_top_volume;
    lv_obj_t *screen_top_state;
    lv_obj_t *screen_top_wifi;
    lv_obj_t *screen_top_wifi_label;
    lv_obj_t *screen_top_bluetooth;
    lv_obj_t *screen_top_bluetooth_label;
    lv_obj_t *screen_top_weather;
    lv_obj_t *screen_imgbtn_9;
    lv_obj_t *screen_imgbtn_9_label;
    lv_obj_t *screen_top_label_region;
    lv_obj_t *screen_top_label_temperature;
    lv_style_t style_knob2;
    lv_style_t style_slider_indicator2;
    lv_style_t style_slider_knob2;
    lv_obj_t *screen_top_set;
    lv_obj_t *screen_top_set_label;
}lv_ui_top;

typedef enum {
    HOME_PAGE,  /**< The main rectangle*/
    FILE_FOLDER_PAGE,/**< The border if style_border_post = true*/
    TIME_PAGE,  /**< The scrollbar*/
    CAMERA_PAGE,
    WIFI_SET_PAGE,
    BLUETOOTH_PAGE,
    SETTING_PAGE,
} page;
void custom_init(lv_ui_top *ui);
void add_slide_trigger(lv_obj_t *page);
bool is_home_page(void);
void back_to_home(void);
void refresh_file_list1(lv_ui *ui);
void add_file_list_btn(lv_ui *ui ,char *file_name);
void show_file_list_main(lv_ui *ui);
void view_file(char * file_name);
#ifdef __cplusplus
}
#endif
#endif /* EVENT_CB_H_ */
