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



void setup_scr_file_folder_page(lv_ui *ui)
{
    printf("open setup_scr_file_folder_page");
    ui->cur_page = ui->file_folder_page;
    ui->page_flage = FILE_FOLDER_PAGE;
    ui->file_folder_page_list_1_item_index = NO_VIEW_FILE;
    ui->view_file_flag = false;
    memset(ui->s_current_path,0,sizeof(ui->s_current_path));
    strcpy(ui->s_current_path, "");
    //Write codes file_folder_page
    ui->file_folder_page = lv_obj_create(NULL);
    lv_obj_set_size(ui->file_folder_page, 320, 240);
    lv_obj_set_scrollbar_mode(ui->file_folder_page, LV_SCROLLBAR_MODE_OFF);

    //Write style for file_folder_page, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->file_folder_page, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->file_folder_page, lv_color_hex(0xd0d0d0), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->file_folder_page, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes file_folder_page_list_1
    ui->file_folder_page_list_1 = lv_list_create(ui->file_folder_page);
    ui->file_folder_page_list_1_item[ui->file_folder_page_list_1_item_index] = lv_list_add_btn(ui->file_folder_page_list_1, LV_SYMBOL_SD_CARD, "spiffs");
    lv_obj_set_size(ui->file_folder_page_list_1_item[ui->file_folder_page_list_1_item_index],318,40);
    ui->file_folder_page_list_1_item_index++;
    ui->file_folder_page_list_1_item[ui->file_folder_page_list_1_item_index] = lv_list_add_btn(ui->file_folder_page_list_1, LV_SYMBOL_SD_CARD, "sdcard");
    lv_obj_set_size(ui->file_folder_page_list_1_item[ui->file_folder_page_list_1_item_index],318,40);
    ui->file_folder_page_list_1_item_index++;


    lv_obj_set_pos(ui->file_folder_page_list_1, 0, 40);
    lv_obj_set_size(ui->file_folder_page_list_1, 320, 200);
    lv_obj_set_scrollbar_mode(ui->file_folder_page_list_1, LV_SCROLLBAR_MODE_OFF);
    lv_obj_add_flag(ui->file_folder_page_list_1, LV_OBJ_FLAG_EVENT_BUBBLE);
    lv_obj_add_flag(ui->file_folder_page_list_1, LV_OBJ_FLAG_CLICKABLE);
    
    //Write style state: LV_STATE_DEFAULT for &style_file_folder_page_list_1_main_main_default
    static lv_style_t style_file_folder_page_list_1_main_main_default;
    ui_init_style(&style_file_folder_page_list_1_main_main_default);

    lv_style_set_pad_top(&style_file_folder_page_list_1_main_main_default, 5);
    lv_style_set_pad_left(&style_file_folder_page_list_1_main_main_default, 10);
    lv_style_set_pad_right(&style_file_folder_page_list_1_main_main_default, 5);
    lv_style_set_pad_bottom(&style_file_folder_page_list_1_main_main_default, 5);
    lv_style_set_bg_opa(&style_file_folder_page_list_1_main_main_default, 255);
    lv_style_set_bg_color(&style_file_folder_page_list_1_main_main_default, lv_color_hex(0xffffff));
    lv_style_set_bg_grad_dir(&style_file_folder_page_list_1_main_main_default, LV_GRAD_DIR_NONE);
    lv_style_set_border_width(&style_file_folder_page_list_1_main_main_default, 1);
    lv_style_set_border_opa(&style_file_folder_page_list_1_main_main_default, 255);
    lv_style_set_border_color(&style_file_folder_page_list_1_main_main_default, lv_color_hex(0xe1e6ee));
    lv_style_set_border_side(&style_file_folder_page_list_1_main_main_default, LV_BORDER_SIDE_NONE);
    lv_style_set_radius(&style_file_folder_page_list_1_main_main_default, 3);
    lv_style_set_shadow_width(&style_file_folder_page_list_1_main_main_default, 0);
    lv_obj_add_style(ui->file_folder_page_list_1, &style_file_folder_page_list_1_main_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style state: LV_STATE_DEFAULT for &style_file_folder_page_list_1_main_scrollbar_default
    static lv_style_t style_file_folder_page_list_1_main_scrollbar_default;
    ui_init_style(&style_file_folder_page_list_1_main_scrollbar_default);

    lv_style_set_radius(&style_file_folder_page_list_1_main_scrollbar_default, 3);
    lv_style_set_bg_opa(&style_file_folder_page_list_1_main_scrollbar_default, 255);
    lv_style_set_bg_color(&style_file_folder_page_list_1_main_scrollbar_default, lv_color_hex(0xffffff));
    lv_style_set_bg_grad_dir(&style_file_folder_page_list_1_main_scrollbar_default, LV_GRAD_DIR_NONE);
    lv_obj_add_style(ui->file_folder_page_list_1, &style_file_folder_page_list_1_main_scrollbar_default, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);

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

    //Write style state: LV_STATE_DEFAULT for &style_file_folder_page_list_1_extra_texts_main_default
    static lv_style_t style_file_folder_page_list_1_extra_texts_main_default;
    ui_init_style(&style_file_folder_page_list_1_extra_texts_main_default);

    lv_style_set_pad_top(&style_file_folder_page_list_1_extra_texts_main_default, 5);
    lv_style_set_pad_left(&style_file_folder_page_list_1_extra_texts_main_default, 5);
    lv_style_set_pad_right(&style_file_folder_page_list_1_extra_texts_main_default, 5);
    lv_style_set_pad_bottom(&style_file_folder_page_list_1_extra_texts_main_default, 5);
    lv_style_set_border_width(&style_file_folder_page_list_1_extra_texts_main_default, 0);
    lv_style_set_text_color(&style_file_folder_page_list_1_extra_texts_main_default, lv_color_hex(0x0D3055));
    lv_style_set_text_font(&style_file_folder_page_list_1_extra_texts_main_default, &lv_font_montserratMedium_12);
    lv_style_set_text_opa(&style_file_folder_page_list_1_extra_texts_main_default, 255);
    lv_style_set_radius(&style_file_folder_page_list_1_extra_texts_main_default, 3);
    lv_style_set_transform_width(&style_file_folder_page_list_1_extra_texts_main_default, 0);
    lv_style_set_bg_opa(&style_file_folder_page_list_1_extra_texts_main_default, 255);
    lv_style_set_bg_color(&style_file_folder_page_list_1_extra_texts_main_default, lv_color_hex(0xffffff));
    lv_style_set_bg_grad_dir(&style_file_folder_page_list_1_extra_texts_main_default, LV_GRAD_DIR_NONE);

    //Write codes file_folder_page_btn_1
    ui->file_folder_page_btn_1 = lv_btn_create(ui->file_folder_page);
    ui->file_folder_page_btn_1_label = lv_label_create(ui->file_folder_page_btn_1);
    lv_label_set_text(ui->file_folder_page_btn_1_label, "...");
    lv_label_set_long_mode(ui->file_folder_page_btn_1_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->file_folder_page_btn_1_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->file_folder_page_btn_1, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->file_folder_page_btn_1_label, LV_PCT(100));
    lv_obj_set_pos(ui->file_folder_page_btn_1, 279, 0);
    lv_obj_set_size(ui->file_folder_page_btn_1, 40, 40);

    //Write style for file_folder_page_btn_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->file_folder_page_btn_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->file_folder_page_btn_1, lv_color_hex(0xa8a8a8), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->file_folder_page_btn_1, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->file_folder_page_btn_1, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui->file_folder_page_btn_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->file_folder_page_btn_1, lv_color_hex(0x5aa9ec), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui->file_folder_page_btn_1, LV_BORDER_SIDE_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->file_folder_page_btn_1, 20, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->file_folder_page_btn_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->file_folder_page_btn_1, lv_color_hex(0x0098fb), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->file_folder_page_btn_1, &lv_font_montserratMedium_20, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->file_folder_page_btn_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->file_folder_page_btn_1, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes file_folder_page_list_2
    ui->file_folder_page_list_2 = lv_list_create(ui->file_folder_page);
    ui->file_folder_page_list_2_item0 = lv_list_add_btn(ui->file_folder_page_list_2, LV_SYMBOL_DOWNLOAD, "tcp");
    lv_obj_set_pos(ui->file_folder_page_list_2, 129, 6);
    lv_obj_set_size(ui->file_folder_page_list_2, 160, 100);
    lv_obj_set_scrollbar_mode(ui->file_folder_page_list_2, LV_SCROLLBAR_MODE_OFF);
    lv_obj_add_flag(ui->file_folder_page_list_2, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui->file_folder_page_list_2, LV_OBJ_FLAG_HIDDEN);

    //Write style state: LV_STATE_DEFAULT for &style_file_folder_page_list_2_main_main_default
    static lv_style_t style_file_folder_page_list_2_main_main_default;
    ui_init_style(&style_file_folder_page_list_2_main_main_default);

    lv_style_set_pad_top(&style_file_folder_page_list_2_main_main_default, 5);
    lv_style_set_pad_left(&style_file_folder_page_list_2_main_main_default, 5);
    lv_style_set_pad_right(&style_file_folder_page_list_2_main_main_default, 5);
    lv_style_set_pad_bottom(&style_file_folder_page_list_2_main_main_default, 5);
    lv_style_set_bg_opa(&style_file_folder_page_list_2_main_main_default, 255);
    lv_style_set_bg_color(&style_file_folder_page_list_2_main_main_default, lv_color_hex(0xffffff));
    lv_style_set_bg_grad_dir(&style_file_folder_page_list_2_main_main_default, LV_GRAD_DIR_NONE);
    lv_style_set_border_width(&style_file_folder_page_list_2_main_main_default, 1);
    lv_style_set_border_opa(&style_file_folder_page_list_2_main_main_default, 255);
    lv_style_set_border_color(&style_file_folder_page_list_2_main_main_default, lv_color_hex(0xe1e6ee));
    lv_style_set_border_side(&style_file_folder_page_list_2_main_main_default, LV_BORDER_SIDE_FULL);
    lv_style_set_radius(&style_file_folder_page_list_2_main_main_default, 3);
    lv_style_set_shadow_width(&style_file_folder_page_list_2_main_main_default, 0);
    lv_obj_add_style(ui->file_folder_page_list_2, &style_file_folder_page_list_2_main_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style state: LV_STATE_DEFAULT for &style_file_folder_page_list_2_main_scrollbar_default
    static lv_style_t style_file_folder_page_list_2_main_scrollbar_default;
    ui_init_style(&style_file_folder_page_list_2_main_scrollbar_default);

    lv_style_set_radius(&style_file_folder_page_list_2_main_scrollbar_default, 3);
    lv_style_set_bg_opa(&style_file_folder_page_list_2_main_scrollbar_default, 255);
    lv_style_set_bg_color(&style_file_folder_page_list_2_main_scrollbar_default, lv_color_hex(0xffffff));
    lv_style_set_bg_grad_dir(&style_file_folder_page_list_2_main_scrollbar_default, LV_GRAD_DIR_NONE);
    lv_obj_add_style(ui->file_folder_page_list_2, &style_file_folder_page_list_2_main_scrollbar_default, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);

    //Write style state: LV_STATE_DEFAULT for &style_file_folder_page_list_2_extra_btns_main_default
    static lv_style_t style_file_folder_page_list_2_extra_btns_main_default;
    ui_init_style(&style_file_folder_page_list_2_extra_btns_main_default);

    lv_style_set_pad_top(&style_file_folder_page_list_2_extra_btns_main_default, 5);
    lv_style_set_pad_left(&style_file_folder_page_list_2_extra_btns_main_default, 5);
    lv_style_set_pad_right(&style_file_folder_page_list_2_extra_btns_main_default, 5);
    lv_style_set_pad_bottom(&style_file_folder_page_list_2_extra_btns_main_default, 5);
    lv_style_set_border_width(&style_file_folder_page_list_2_extra_btns_main_default, 0);
    lv_style_set_text_color(&style_file_folder_page_list_2_extra_btns_main_default, lv_color_hex(0x0D3055));
    lv_style_set_text_font(&style_file_folder_page_list_2_extra_btns_main_default, &lv_font_montserratMedium_12);
    lv_style_set_text_opa(&style_file_folder_page_list_2_extra_btns_main_default, 255);
    lv_style_set_radius(&style_file_folder_page_list_2_extra_btns_main_default, 3);
    lv_style_set_bg_opa(&style_file_folder_page_list_2_extra_btns_main_default, 255);
    lv_style_set_bg_color(&style_file_folder_page_list_2_extra_btns_main_default, lv_color_hex(0xffffff));
    lv_style_set_bg_grad_dir(&style_file_folder_page_list_2_extra_btns_main_default, LV_GRAD_DIR_NONE);
    lv_obj_add_style(ui->file_folder_page_list_2_item0, &style_file_folder_page_list_2_extra_btns_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style state: LV_STATE_DEFAULT for &style_file_folder_page_list_2_extra_texts_main_default
    static lv_style_t style_file_folder_page_list_2_extra_texts_main_default;
    ui_init_style(&style_file_folder_page_list_2_extra_texts_main_default);

    lv_style_set_pad_top(&style_file_folder_page_list_2_extra_texts_main_default, 5);
    lv_style_set_pad_left(&style_file_folder_page_list_2_extra_texts_main_default, 5);
    lv_style_set_pad_right(&style_file_folder_page_list_2_extra_texts_main_default, 5);
    lv_style_set_pad_bottom(&style_file_folder_page_list_2_extra_texts_main_default, 5);
    lv_style_set_border_width(&style_file_folder_page_list_2_extra_texts_main_default, 0);
    lv_style_set_text_color(&style_file_folder_page_list_2_extra_texts_main_default, lv_color_hex(0x0D3055));
    lv_style_set_text_font(&style_file_folder_page_list_2_extra_texts_main_default, &lv_font_montserratMedium_12);
    lv_style_set_text_opa(&style_file_folder_page_list_2_extra_texts_main_default, 255);
    lv_style_set_radius(&style_file_folder_page_list_2_extra_texts_main_default, 3);
    lv_style_set_transform_width(&style_file_folder_page_list_2_extra_texts_main_default, 0);
    lv_style_set_bg_opa(&style_file_folder_page_list_2_extra_texts_main_default, 255);
    lv_style_set_bg_color(&style_file_folder_page_list_2_extra_texts_main_default, lv_color_hex(0xffffff));
    lv_style_set_bg_grad_dir(&style_file_folder_page_list_2_extra_texts_main_default, LV_GRAD_DIR_NONE);

    //Write codes file_folder_page_btn_2
    ui->file_folder_page_btn_2 = lv_btn_create(ui->file_folder_page);
    ui->file_folder_page_btn_2_label = lv_label_create(ui->file_folder_page_btn_2);
    lv_label_set_text(ui->file_folder_page_btn_2_label, "");
    lv_label_set_long_mode(ui->file_folder_page_btn_2_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->file_folder_page_btn_2_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->file_folder_page_btn_2, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->file_folder_page_btn_2_label, LV_PCT(100));
    lv_obj_set_pos(ui->file_folder_page_btn_2, 50, 17);
    lv_obj_set_size(ui->file_folder_page_btn_2, 10, 10);

    //Write style for file_folder_page_btn_2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->file_folder_page_btn_2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->file_folder_page_btn_2, lv_color_hex(0x2195f6), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->file_folder_page_btn_2, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->file_folder_page_btn_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->file_folder_page_btn_2, 15, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->file_folder_page_btn_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->file_folder_page_btn_2, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->file_folder_page_btn_2, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->file_folder_page_btn_2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->file_folder_page_btn_2, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes file_folder_page_btn_3
    ui->file_folder_page_btn_3 = lv_btn_create(ui->file_folder_page);
    ui->file_folder_page_btn_3_label = lv_label_create(ui->file_folder_page_btn_3);
    lv_label_set_text(ui->file_folder_page_btn_3_label, "");
    lv_label_set_long_mode(ui->file_folder_page_btn_3_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->file_folder_page_btn_3_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->file_folder_page_btn_3, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->file_folder_page_btn_3_label, LV_PCT(100));
    lv_obj_set_pos(ui->file_folder_page_btn_3, 30, 17);
    lv_obj_set_size(ui->file_folder_page_btn_3, 10, 10);

    //Write style for file_folder_page_btn_3, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->file_folder_page_btn_3, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->file_folder_page_btn_3, lv_color_hex(0x00ff24), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->file_folder_page_btn_3, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->file_folder_page_btn_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->file_folder_page_btn_3, 15, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->file_folder_page_btn_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->file_folder_page_btn_3, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->file_folder_page_btn_3, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->file_folder_page_btn_3, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->file_folder_page_btn_3, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes file_folder_page_btn_4
    ui->file_folder_page_btn_4 = lv_btn_create(ui->file_folder_page);
    ui->file_folder_page_btn_4_label = lv_label_create(ui->file_folder_page_btn_4);
    lv_label_set_text(ui->file_folder_page_btn_4_label, "");
    lv_label_set_long_mode(ui->file_folder_page_btn_4_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->file_folder_page_btn_4_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->file_folder_page_btn_4, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->file_folder_page_btn_4_label, LV_PCT(100));
    lv_obj_set_pos(ui->file_folder_page_btn_4, 10, 17);
    lv_obj_set_size(ui->file_folder_page_btn_4, 10, 10);

    //Write style for file_folder_page_btn_4, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->file_folder_page_btn_4, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->file_folder_page_btn_4, lv_color_hex(0xff6500), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->file_folder_page_btn_4, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->file_folder_page_btn_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->file_folder_page_btn_4, 15, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->file_folder_page_btn_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->file_folder_page_btn_4, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->file_folder_page_btn_4, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->file_folder_page_btn_4, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->file_folder_page_btn_4, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes file_folder_page_label_upper_level
    ui->file_folder_page_label_upper_level = lv_label_create(ui->file_folder_page);
    lv_label_set_text(ui->file_folder_page_label_upper_level, "Root");
    lv_label_set_long_mode(ui->file_folder_page_label_upper_level, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->file_folder_page_label_upper_level, 129, 3);
    lv_obj_set_size(ui->file_folder_page_label_upper_level, 100, 32);

    //Write style for file_folder_page_label_upper_level, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->file_folder_page_label_upper_level, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->file_folder_page_label_upper_level, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->file_folder_page_label_upper_level, lv_color_hex(0xfbfbfb), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->file_folder_page_label_upper_level, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->file_folder_page_label_upper_level, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->file_folder_page_label_upper_level, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->file_folder_page_label_upper_level, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->file_folder_page_label_upper_level, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->file_folder_page_label_upper_level, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->file_folder_page_label_upper_level, 10, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->file_folder_page_label_upper_level, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->file_folder_page_label_upper_level, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->file_folder_page_label_upper_level, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->file_folder_page_label_upper_level, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes file_folder_page_btn_back
    ui->file_folder_page_btn_back = lv_btn_create(ui->file_folder_page);
    ui->file_folder_page_btn_back_label = lv_label_create(ui->file_folder_page_btn_back);
    lv_label_set_text(ui->file_folder_page_btn_back_label, "<");
    lv_label_set_long_mode(ui->file_folder_page_btn_back_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->file_folder_page_btn_back_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->file_folder_page_btn_back, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->file_folder_page_btn_back_label, LV_PCT(100));
    lv_obj_set_pos(ui->file_folder_page_btn_back, 124, 6);
    lv_obj_set_size(ui->file_folder_page_btn_back, 39, 30);
    lv_obj_add_flag(ui->file_folder_page_btn_back, LV_OBJ_FLAG_HIDDEN);

    //Write style for file_folder_page_btn_back, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->file_folder_page_btn_back, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->file_folder_page_btn_back, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->file_folder_page_btn_back, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->file_folder_page_btn_back, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->file_folder_page_btn_back, lv_color_hex(0x739eed), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->file_folder_page_btn_back, &lv_font_montserratMedium_30, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->file_folder_page_btn_back, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->file_folder_page_btn_back, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    //The custom code of file_folder_page.
    add_slide_trigger(ui->file_folder_page);
    
    //Update current screen layout.
    lv_obj_update_layout(ui->file_folder_page);
    printf("ui->file_folder_page_list_1_item_index:%d\n",ui->file_folder_page_list_1_item_index);
    //Init events for screen.
    events_init_file_folder_page(ui);
}
