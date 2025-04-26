/*
* Copyright 2025 NXP
* NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
* terms, then you may not retain, install, activate or otherwise use the software.
*/

#ifndef GUI_GUIDER_H
#define GUI_GUIDER_H
#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl.h"

enum{
	NO_VIEW_FILE,
	VIEW_FILE_MJPEG,
	VIEW_FILE_JPG,
	VIEW_FILE_PNG,
	VIEW_FILE_BMP,
	VIEW_FILE_MP3,
	VIEW_FILE_TXT,
	VIEW_FILE_GIF,
	VIEW_FILE_UNDEFINED,
};

typedef struct
{
	lv_obj_t *wifi_set_page_wifi_scan_info[10];
	int wifi_set_page_wifi_scan_info_index;
	lv_obj_t *wifi_set_page_label_wifi_scan_info_ssid[10];
	int wifi_set_page_label_wifi_scan_info_ssid_index;
}lv_scan_wifi_info_btn;

typedef struct
{
	int page_flage;
	lv_obj_t *screen;
	bool screen_del;
	lv_obj_t *screen_img_1;
	lv_obj_t *screen_img_2;
	lv_obj_t *screen_img_3;
	lv_obj_t *screen_img_4;
	lv_obj_t *screen_img_5;
	lv_obj_t *screen_img_6;
	lv_obj_t *screen_img_7;
	lv_obj_t *screen_img_8;
	lv_obj_t *screen_tileview_1;
	lv_obj_t *screen_tileview_1_tile;
	lv_obj_t *screen_cont_1;
	lv_obj_t *screen_slider_1;
	lv_obj_t *screen_slider_2;
	lv_obj_t *screen_imgbtn_1;
	lv_obj_t *screen_imgbtn_1_label;
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
	lv_obj_t *file_folder_page;
	lv_obj_t *img_camera;
	lv_obj_t *cur_page;
	bool file_folder_page_del;
	int view_file_flag;
	char s_current_path[256];
	lv_obj_t *file_folder_page_view_file_container;
	lv_obj_t *file_folder_page_label_1;
	lv_obj_t *file_folder_page_list_1;
	lv_obj_t *file_folder_page_list_1_item[256];
	int file_folder_page_list_1_item_index;
	lv_obj_t *file_folder_page_btn_1;
	lv_obj_t *file_folder_page_btn_1_label;
	lv_obj_t *file_folder_page_list_2;
	lv_obj_t *file_folder_page_list_2_item0;
	lv_obj_t *file_folder_page_btn_2;
	lv_obj_t *file_folder_page_btn_2_label;
	lv_obj_t *file_folder_page_btn_3;
	lv_obj_t *file_folder_page_btn_3_label;
	lv_obj_t *file_folder_page_btn_4;
	lv_obj_t *file_folder_page_btn_4_label;
	lv_obj_t *file_folder_page_label_upper_level;
	lv_obj_t *file_folder_page_btn_back;
	lv_obj_t *file_folder_page_btn_back_label;
	lv_obj_t *time;
	bool time_del;
	lv_obj_t *time_cont_1;
	lv_obj_t *time_label_1;
	lv_obj_t *time_btn_2;
	lv_obj_t *time_btn_2_label;
	lv_obj_t *time_btn_1;
	lv_obj_t *time_btn_1_label;
	lv_obj_t *time_imgbtn_1;
	lv_obj_t *time_imgbtn_1_label;
	lv_obj_t *time_imgbtn_2;
	lv_obj_t *time_imgbtn_2_label;
	lv_obj_t *time_imgbtn_3;
	lv_obj_t *time_imgbtn_3_label;
	lv_obj_t *time_imgbtn_4;
	lv_obj_t *time_imgbtn_4_label;
	lv_obj_t *time_cont_2;
	lv_obj_t *time_btn_3;
	lv_obj_t *time_btn_3_label;
	lv_obj_t *time_label_2;
	lv_obj_t *time_btn_4;
	lv_obj_t *time_btn_4_label;
	lv_obj_t *time_roller_2;
	lv_obj_t *time_roller_1;
	lv_obj_t *time_list_1;
	lv_obj_t *time_list_1_item0;
	lv_obj_t *time_list_1_item1;
	lv_obj_t *time_list_1_item2;
	lv_obj_t *camera;
	bool camera_del;
	lv_obj_t *wifi_set_page;
	bool wifi_set_page_del;
	lv_scan_wifi_info_btn *wifi_set_page_wifi_info_btn;
	lv_obj_t *wifi_set_page_cont_1;
	lv_obj_t *wifi_set_page_STA;
	lv_obj_t *wifi_set_page_label_2;
	lv_obj_t *wifi_set_page_sw_1;
	lv_obj_t *wifi_set_page_connect_sta;
	lv_obj_t *wifi_set_page_label_4;
	lv_obj_t *wifi_set_page_label_1;
	lv_obj_t *wifi_set_page_label_3;
	lv_obj_t *wifi_set_page_cont_2;
	lv_obj_t *setting_page;
	bool setting_page_del;
	lv_obj_t *setting_page_label_1;
	lv_obj_t *setting_page_cont_1;
	lv_obj_t *setting_page_cont_3;
	lv_obj_t *setting_page_img_2;
	lv_obj_t *setting_page_label_3;
	lv_obj_t *setting_page_label_5;
	lv_obj_t *setting_page_cont_2;
	lv_obj_t *setting_page_img_1;
	lv_obj_t *setting_page_label_2;
	lv_obj_t *setting_page_label_4;
	lv_obj_t *setting_page_label_wifi_name;
	lv_obj_t *bluetooth_page;
	bool bluetooth_page_del;
}lv_ui;

typedef void (*ui_setup_scr_t)(lv_ui * ui);

void ui_init_style(lv_style_t * style);

void ui_load_scr_animation(lv_ui *ui, lv_obj_t ** new_scr, bool new_scr_del, bool * old_scr_del, ui_setup_scr_t setup_scr,
                           lv_scr_load_anim_t anim_type, uint32_t time, uint32_t delay, bool is_clean, bool auto_del);

void ui_animation(void * var, int32_t duration, int32_t delay, int32_t start_value, int32_t end_value, lv_anim_path_cb_t path_cb,
                       uint16_t repeat_cnt, uint32_t repeat_delay, uint32_t playback_time, uint32_t playback_delay,
                       lv_anim_exec_xcb_t exec_cb, lv_anim_start_cb_t start_cb, lv_anim_ready_cb_t ready_cb, lv_anim_deleted_cb_t deleted_cb);


void init_scr_del_flag(lv_ui *ui);

void setup_ui(lv_ui *ui);

void init_keyboard(lv_ui *ui);

extern lv_ui guider_ui;


void setup_scr_screen(lv_ui *ui);
void setup_scr_file_folder_page(lv_ui *ui);
void setup_scr_time(lv_ui *ui);
void setup_scr_camera(lv_ui *ui);
void setup_scr_wifi_set_page(lv_ui *ui);
void setup_scr_setting_page(lv_ui *ui);
void setup_scr_bluetooth_page(lv_ui *ui);
LV_IMG_DECLARE(_wifi1_alpha_50x50);
LV_IMG_DECLARE(_icon_alpha_50x50);
LV_IMG_DECLARE(_wenjianjia3_alpha_100x100);
LV_IMG_DECLARE(_xiangji_alpha_100x100);
LV_IMG_DECLARE(_yuncai_alpha_100x100);
LV_IMG_DECLARE(_naozhong_alpha_100x100);
LV_IMG_DECLARE(_xiangce_alpha_100x100);
LV_IMG_DECLARE(_zhinanzhen_alpha_100x100);
LV_IMG_DECLARE(_ios_alpha_50x50);
LV_IMG_DECLARE(_wifi_alpha_40x40);
LV_IMG_DECLARE(_wifi2_alpha_40x40);
LV_IMG_DECLARE(_lanya_alpha_40x40);
LV_IMG_DECLARE(_lanya2_alpha_40x40);
LV_IMG_DECLARE(_ios_settings1_alpha_40x40);
LV_IMG_DECLARE(_icon_test_22_alpha_40x40);
LV_IMG_DECLARE(_zhongyingwen_alpha_50x50);
LV_IMG_DECLARE(_zhongyingwen1_alpha_50x50);
LV_IMG_DECLARE(_naozhong2_alpha_50x50);
LV_IMG_DECLARE(_naozhong1_alpha_50x50);
LV_IMG_DECLARE(_miaobiao_alpha_50x50);
LV_IMG_DECLARE(_miaobiao1_alpha_50x50);
LV_IMG_DECLARE(_jishiqi_alpha_46x39);
LV_IMG_DECLARE(_jishiqi1_alpha_46x39);
LV_IMG_DECLARE(_lanya1_alpha_40x40);
LV_IMG_DECLARE(_wifi4_alpha_40x40);

LV_FONT_DECLARE(lv_font_montserratMedium_12)
LV_FONT_DECLARE(lv_font_ArchitectsDaughter_20)
LV_FONT_DECLARE(lv_font_montserratMedium_16)
LV_FONT_DECLARE(lv_font_montserratMedium_20)
LV_FONT_DECLARE(lv_font_montserratMedium_30)
LV_FONT_DECLARE(lv_font_montserratMedium_18)
LV_FONT_DECLARE(lv_font_montserratMedium_14)
LV_FONT_DECLARE(lv_font_montserratMedium_25)


#ifdef __cplusplus
}
#endif
#endif
