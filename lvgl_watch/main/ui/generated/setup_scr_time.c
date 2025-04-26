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



void setup_scr_time(lv_ui *ui)
{
    printf("open setup_scr_time\n");
    ui->cur_page = ui->time;
    ui->page_flage = TIME_PAGE;
    //Write codes time
    ui->time = lv_obj_create(NULL);
    lv_obj_set_size(ui->time, 320, 240);
    lv_obj_set_scrollbar_mode(ui->time, LV_SCROLLBAR_MODE_OFF);

    //Write style for time, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->time, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->time, lv_color_hex(0x181111), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->time, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_clear_flag(ui->time, LV_OBJ_FLAG_SCROLLABLE);
    //Write codes time_cont_1
    ui->time_cont_1 = lv_obj_create(ui->time);
    lv_obj_set_pos(ui->time_cont_1, 1, 34);
    lv_obj_set_size(ui->time_cont_1, 318, 167);
    lv_obj_set_scrollbar_mode(ui->time_cont_1, LV_SCROLLBAR_MODE_OFF);

    //Write style for time_cont_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->time_cont_1, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui->time_cont_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->time_cont_1, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui->time_cont_1, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->time_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->time_cont_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->time_cont_1, lv_color_hex(0x0b0b0b), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->time_cont_1, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->time_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->time_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->time_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->time_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->time_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes time_label_1
    ui->time_label_1 = lv_label_create(ui->time_cont_1);
    lv_label_set_text(ui->time_label_1, "闹钟");
    lv_label_set_long_mode(ui->time_label_1, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->time_label_1, 108, -29);
    lv_obj_set_size(ui->time_label_1, 100, 32);

    //Write style for time_label_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->time_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->time_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->time_label_1, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->time_label_1, &lv_font_montserratMedium_18, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->time_label_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->time_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->time_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->time_label_1, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->time_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->time_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->time_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->time_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->time_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->time_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes time_btn_2
    ui->time_btn_2 = lv_btn_create(ui->time);
    ui->time_btn_2_label = lv_label_create(ui->time_btn_2);
    lv_label_set_text(ui->time_btn_2_label, "Eedie");
    lv_label_set_long_mode(ui->time_btn_2_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->time_btn_2_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->time_btn_2, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->time_btn_2_label, LV_PCT(100));
    lv_obj_set_pos(ui->time_btn_2, 6, 4);
    lv_obj_set_size(ui->time_btn_2, 60, 30);

    //Write style for time_btn_2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->time_btn_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->time_btn_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->time_btn_2, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->time_btn_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->time_btn_2, lv_color_hex(0xfb7100), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->time_btn_2, &lv_font_montserratMedium_18, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->time_btn_2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->time_btn_2, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes time_btn_1
    ui->time_btn_1 = lv_btn_create(ui->time);
    ui->time_btn_1_label = lv_label_create(ui->time_btn_1);
    lv_label_set_text(ui->time_btn_1_label, "+");
    lv_label_set_long_mode(ui->time_btn_1_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->time_btn_1_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->time_btn_1, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->time_btn_1_label, LV_PCT(100));
    lv_obj_set_pos(ui->time_btn_1, 286, 0);
    lv_obj_set_size(ui->time_btn_1, 31, 30);

    //Write style for time_btn_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->time_btn_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->time_btn_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->time_btn_1, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->time_btn_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->time_btn_1, lv_color_hex(0xfb7100), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->time_btn_1, &lv_font_montserratMedium_30, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->time_btn_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->time_btn_1, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes time_imgbtn_1
    ui->time_imgbtn_1 = lv_imgbtn_create(ui->time);
    lv_obj_add_flag(ui->time_imgbtn_1, LV_OBJ_FLAG_CHECKABLE);
    lv_imgbtn_set_src(ui->time_imgbtn_1, LV_IMGBTN_STATE_RELEASED, NULL, &_zhongyingwen_alpha_50x50, NULL);
    lv_imgbtn_set_src(ui->time_imgbtn_1, LV_IMGBTN_STATE_CHECKED_RELEASED, NULL, &_zhongyingwen1_alpha_50x50, NULL);
    ui->time_imgbtn_1_label = lv_label_create(ui->time_imgbtn_1);
    lv_label_set_text(ui->time_imgbtn_1_label, "");
    lv_label_set_long_mode(ui->time_imgbtn_1_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->time_imgbtn_1_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->time_imgbtn_1, 0, LV_STATE_DEFAULT);
    lv_obj_set_pos(ui->time_imgbtn_1, 6, 195);
    lv_obj_set_size(ui->time_imgbtn_1, 50, 50);

    //Write style for time_imgbtn_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_text_color(ui->time_imgbtn_1, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->time_imgbtn_1, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->time_imgbtn_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->time_imgbtn_1, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->time_imgbtn_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->time_imgbtn_1, true, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->time_imgbtn_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for time_imgbtn_1, Part: LV_PART_MAIN, State: LV_STATE_PRESSED.
    lv_obj_set_style_img_recolor_opa(ui->time_imgbtn_1, 0, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_img_opa(ui->time_imgbtn_1, 255, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_color(ui->time_imgbtn_1, lv_color_hex(0xFF33FF), LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_font(ui->time_imgbtn_1, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_opa(ui->time_imgbtn_1, 255, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_shadow_width(ui->time_imgbtn_1, 0, LV_PART_MAIN|LV_STATE_PRESSED);

    //Write style for time_imgbtn_1, Part: LV_PART_MAIN, State: LV_STATE_CHECKED.
    lv_obj_set_style_img_recolor_opa(ui->time_imgbtn_1, 0, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_img_opa(ui->time_imgbtn_1, 255, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_color(ui->time_imgbtn_1, lv_color_hex(0xFF33FF), LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_font(ui->time_imgbtn_1, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_opa(ui->time_imgbtn_1, 255, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_shadow_width(ui->time_imgbtn_1, 0, LV_PART_MAIN|LV_STATE_CHECKED);

    //Write style for time_imgbtn_1, Part: LV_PART_MAIN, State: LV_IMGBTN_STATE_RELEASED.
    lv_obj_set_style_img_recolor_opa(ui->time_imgbtn_1, 0, LV_PART_MAIN|LV_IMGBTN_STATE_RELEASED);
    lv_obj_set_style_img_opa(ui->time_imgbtn_1, 255, LV_PART_MAIN|LV_IMGBTN_STATE_RELEASED);

    //Write codes time_imgbtn_2
    ui->time_imgbtn_2 = lv_imgbtn_create(ui->time);
    lv_obj_add_flag(ui->time_imgbtn_2, LV_OBJ_FLAG_CHECKABLE);
    lv_imgbtn_set_src(ui->time_imgbtn_2, LV_IMGBTN_STATE_RELEASED, NULL, &_naozhong2_alpha_50x50, NULL);
    lv_imgbtn_set_src(ui->time_imgbtn_2, LV_IMGBTN_STATE_CHECKED_RELEASED, NULL, &_naozhong1_alpha_50x50, NULL);
    ui->time_imgbtn_2_label = lv_label_create(ui->time_imgbtn_2);
    lv_label_set_text(ui->time_imgbtn_2_label, "");
    lv_label_set_long_mode(ui->time_imgbtn_2_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->time_imgbtn_2_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->time_imgbtn_2, 0, LV_STATE_DEFAULT);
    lv_obj_set_pos(ui->time_imgbtn_2, 85, 195);
    lv_obj_set_size(ui->time_imgbtn_2, 50, 50);

    //Write style for time_imgbtn_2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_text_color(ui->time_imgbtn_2, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->time_imgbtn_2, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->time_imgbtn_2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->time_imgbtn_2, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->time_imgbtn_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->time_imgbtn_2, true, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->time_imgbtn_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for time_imgbtn_2, Part: LV_PART_MAIN, State: LV_STATE_PRESSED.
    lv_obj_set_style_img_recolor_opa(ui->time_imgbtn_2, 0, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_img_opa(ui->time_imgbtn_2, 255, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_color(ui->time_imgbtn_2, lv_color_hex(0xFF33FF), LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_font(ui->time_imgbtn_2, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_opa(ui->time_imgbtn_2, 255, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_shadow_width(ui->time_imgbtn_2, 0, LV_PART_MAIN|LV_STATE_PRESSED);

    //Write style for time_imgbtn_2, Part: LV_PART_MAIN, State: LV_STATE_CHECKED.
    lv_obj_set_style_img_recolor_opa(ui->time_imgbtn_2, 0, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_img_opa(ui->time_imgbtn_2, 255, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_color(ui->time_imgbtn_2, lv_color_hex(0xFF33FF), LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_font(ui->time_imgbtn_2, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_opa(ui->time_imgbtn_2, 255, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_shadow_width(ui->time_imgbtn_2, 0, LV_PART_MAIN|LV_STATE_CHECKED);

    //Write style for time_imgbtn_2, Part: LV_PART_MAIN, State: LV_IMGBTN_STATE_RELEASED.
    lv_obj_set_style_img_recolor_opa(ui->time_imgbtn_2, 0, LV_PART_MAIN|LV_IMGBTN_STATE_RELEASED);
    lv_obj_set_style_img_opa(ui->time_imgbtn_2, 255, LV_PART_MAIN|LV_IMGBTN_STATE_RELEASED);

    //Write codes time_imgbtn_3
    ui->time_imgbtn_3 = lv_imgbtn_create(ui->time);
    lv_obj_add_flag(ui->time_imgbtn_3, LV_OBJ_FLAG_CHECKABLE);
    lv_imgbtn_set_src(ui->time_imgbtn_3, LV_IMGBTN_STATE_RELEASED, NULL, &_miaobiao_alpha_50x50, NULL);
    lv_imgbtn_set_src(ui->time_imgbtn_3, LV_IMGBTN_STATE_CHECKED_RELEASED, NULL, &_miaobiao1_alpha_50x50, NULL);
    ui->time_imgbtn_3_label = lv_label_create(ui->time_imgbtn_3);
    lv_label_set_text(ui->time_imgbtn_3_label, "");
    lv_label_set_long_mode(ui->time_imgbtn_3_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->time_imgbtn_3_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->time_imgbtn_3, 0, LV_STATE_DEFAULT);
    lv_obj_set_pos(ui->time_imgbtn_3, 165, 195);
    lv_obj_set_size(ui->time_imgbtn_3, 50, 50);

    //Write style for time_imgbtn_3, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_text_color(ui->time_imgbtn_3, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->time_imgbtn_3, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->time_imgbtn_3, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->time_imgbtn_3, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->time_imgbtn_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->time_imgbtn_3, true, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->time_imgbtn_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for time_imgbtn_3, Part: LV_PART_MAIN, State: LV_STATE_PRESSED.
    lv_obj_set_style_img_recolor_opa(ui->time_imgbtn_3, 0, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_img_opa(ui->time_imgbtn_3, 255, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_color(ui->time_imgbtn_3, lv_color_hex(0xFF33FF), LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_font(ui->time_imgbtn_3, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_opa(ui->time_imgbtn_3, 255, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_shadow_width(ui->time_imgbtn_3, 0, LV_PART_MAIN|LV_STATE_PRESSED);

    //Write style for time_imgbtn_3, Part: LV_PART_MAIN, State: LV_STATE_CHECKED.
    lv_obj_set_style_img_recolor_opa(ui->time_imgbtn_3, 0, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_img_opa(ui->time_imgbtn_3, 255, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_color(ui->time_imgbtn_3, lv_color_hex(0xFF33FF), LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_font(ui->time_imgbtn_3, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_opa(ui->time_imgbtn_3, 255, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_shadow_width(ui->time_imgbtn_3, 0, LV_PART_MAIN|LV_STATE_CHECKED);

    //Write style for time_imgbtn_3, Part: LV_PART_MAIN, State: LV_IMGBTN_STATE_RELEASED.
    lv_obj_set_style_img_recolor_opa(ui->time_imgbtn_3, 0, LV_PART_MAIN|LV_IMGBTN_STATE_RELEASED);
    lv_obj_set_style_img_opa(ui->time_imgbtn_3, 255, LV_PART_MAIN|LV_IMGBTN_STATE_RELEASED);

    //Write codes time_imgbtn_4
    ui->time_imgbtn_4 = lv_imgbtn_create(ui->time);
    lv_obj_add_flag(ui->time_imgbtn_4, LV_OBJ_FLAG_CHECKABLE);
    lv_imgbtn_set_src(ui->time_imgbtn_4, LV_IMGBTN_STATE_RELEASED, NULL, &_jishiqi_alpha_46x39, NULL);
    lv_imgbtn_set_src(ui->time_imgbtn_4, LV_IMGBTN_STATE_CHECKED_RELEASED, NULL, &_jishiqi1_alpha_46x39, NULL);
    ui->time_imgbtn_4_label = lv_label_create(ui->time_imgbtn_4);
    lv_label_set_text(ui->time_imgbtn_4_label, "");
    lv_label_set_long_mode(ui->time_imgbtn_4_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->time_imgbtn_4_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->time_imgbtn_4, 0, LV_STATE_DEFAULT);
    lv_obj_set_pos(ui->time_imgbtn_4, 251, 201);
    lv_obj_set_size(ui->time_imgbtn_4, 46, 39);

    //Write style for time_imgbtn_4, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_text_color(ui->time_imgbtn_4, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->time_imgbtn_4, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->time_imgbtn_4, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->time_imgbtn_4, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->time_imgbtn_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->time_imgbtn_4, true, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->time_imgbtn_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for time_imgbtn_4, Part: LV_PART_MAIN, State: LV_STATE_PRESSED.
    lv_obj_set_style_img_recolor_opa(ui->time_imgbtn_4, 0, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_img_opa(ui->time_imgbtn_4, 255, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_color(ui->time_imgbtn_4, lv_color_hex(0xFF33FF), LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_font(ui->time_imgbtn_4, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_opa(ui->time_imgbtn_4, 255, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_shadow_width(ui->time_imgbtn_4, 0, LV_PART_MAIN|LV_STATE_PRESSED);

    //Write style for time_imgbtn_4, Part: LV_PART_MAIN, State: LV_STATE_CHECKED.
    lv_obj_set_style_img_recolor_opa(ui->time_imgbtn_4, 0, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_img_opa(ui->time_imgbtn_4, 255, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_color(ui->time_imgbtn_4, lv_color_hex(0xFF33FF), LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_font(ui->time_imgbtn_4, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_opa(ui->time_imgbtn_4, 255, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_shadow_width(ui->time_imgbtn_4, 0, LV_PART_MAIN|LV_STATE_CHECKED);

    //Write style for time_imgbtn_4, Part: LV_PART_MAIN, State: LV_IMGBTN_STATE_RELEASED.
    lv_obj_set_style_img_recolor_opa(ui->time_imgbtn_4, 0, LV_PART_MAIN|LV_IMGBTN_STATE_RELEASED);
    lv_obj_set_style_img_opa(ui->time_imgbtn_4, 255, LV_PART_MAIN|LV_IMGBTN_STATE_RELEASED);

    //Write codes time_cont_2
    ui->time_cont_2 = lv_obj_create(ui->time);
    lv_obj_set_pos(ui->time_cont_2, 0, 15);
    lv_obj_set_size(ui->time_cont_2, 320, 226);
    lv_obj_set_scrollbar_mode(ui->time_cont_2, LV_SCROLLBAR_MODE_OFF);
    lv_obj_add_flag(ui->time_cont_2, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui->time_cont_2, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui->time_cont_2, LV_OBJ_FLAG_EVENT_BUBBLE);
    lv_obj_add_flag(ui->time_cont_2, LV_OBJ_FLAG_CLICKABLE);

    //Write style for time_cont_2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->time_cont_2, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui->time_cont_2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->time_cont_2, lv_color_hex(0x2195f6), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui->time_cont_2, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->time_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->time_cont_2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->time_cont_2, lv_color_hex(0x2e2a2a), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->time_cont_2, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->time_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->time_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->time_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->time_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->time_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes time_btn_3
    ui->time_btn_3 = lv_btn_create(ui->time_cont_2);
    ui->time_btn_3_label = lv_label_create(ui->time_btn_3);
    lv_label_set_text(ui->time_btn_3_label, "back");
    lv_label_set_long_mode(ui->time_btn_3_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->time_btn_3_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->time_btn_3, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->time_btn_3_label, LV_PCT(100));
    lv_obj_set_pos(ui->time_btn_3, 4, 3);
    lv_obj_set_size(ui->time_btn_3, 60, 30);

    //Write style for time_btn_3, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->time_btn_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->time_btn_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->time_btn_3, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->time_btn_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->time_btn_3, lv_color_hex(0xfb7100), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->time_btn_3, &lv_font_montserratMedium_18, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->time_btn_3, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->time_btn_3, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes time_label_2
    ui->time_label_2 = lv_label_create(ui->time_cont_2);
    lv_label_set_text(ui->time_label_2, "add");
    lv_label_set_long_mode(ui->time_label_2, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->time_label_2, 109, 10);
    lv_obj_set_size(ui->time_label_2, 100, 32);

    //Write style for time_label_2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->time_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->time_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->time_label_2, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->time_label_2, &lv_font_montserratMedium_18, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->time_label_2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->time_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->time_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->time_label_2, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->time_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->time_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->time_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->time_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->time_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->time_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes time_btn_4
    ui->time_btn_4 = lv_btn_create(ui->time_cont_2);
    ui->time_btn_4_label = lv_label_create(ui->time_btn_4);
    lv_label_set_text(ui->time_btn_4_label, "save");
    lv_label_set_long_mode(ui->time_btn_4_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->time_btn_4_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->time_btn_4, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->time_btn_4_label, LV_PCT(100));
    lv_obj_set_pos(ui->time_btn_4, 253, 3);
    lv_obj_set_size(ui->time_btn_4, 60, 30);

    //Write style for time_btn_4, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->time_btn_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->time_btn_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->time_btn_4, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->time_btn_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->time_btn_4, lv_color_hex(0xfb7100), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->time_btn_4, &lv_font_montserratMedium_18, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->time_btn_4, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->time_btn_4, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes time_roller_2
    ui->time_roller_2 = lv_roller_create(ui->time_cont_2);
    lv_roller_set_options(ui->time_roller_2, "01\n02\n03\n04\n05\n06\n07\n08\n09\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\n26\n27\n28\n29\n30\n31\n32\n33\n34\n35\n36\n37\n38\n39\n40\n41\n42\n43\n44\n45\n46\n47\n48\n49\n50\n51\n52\n53\n54\n55\n56\n57\n58\n59\n00", LV_ROLLER_MODE_INFINITE);
    lv_obj_set_pos(ui->time_roller_2, 164, 46);
    lv_obj_set_width(ui->time_roller_2, 38);

    //Write style for time_roller_2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_radius(ui->time_roller_2, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->time_roller_2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->time_roller_2, lv_color_hex(0x444444), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->time_roller_2, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->time_roller_2, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->time_roller_2, &lv_font_montserratMedium_14, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->time_roller_2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->time_roller_2, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->time_roller_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->time_roller_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for time_roller_2, Part: LV_PART_SELECTED, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->time_roller_2, 255, LV_PART_SELECTED|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->time_roller_2, lv_color_hex(0x4a5260), LV_PART_SELECTED|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->time_roller_2, LV_GRAD_DIR_NONE, LV_PART_SELECTED|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->time_roller_2, lv_color_hex(0x2195f6), LV_PART_SELECTED|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->time_roller_2, &lv_font_montserratMedium_20, LV_PART_SELECTED|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->time_roller_2, 255, LV_PART_SELECTED|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->time_roller_2, LV_TEXT_ALIGN_CENTER, LV_PART_SELECTED|LV_STATE_DEFAULT);

    lv_roller_set_visible_row_count(ui->time_roller_2, 2);
    //Write codes time_roller_1
    ui->time_roller_1 = lv_roller_create(ui->time_cont_2);
    lv_roller_set_options(ui->time_roller_1, "01\n02\n03\n04\n05\n06\n07\n08\n09\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n00", LV_ROLLER_MODE_INFINITE);
    lv_obj_set_pos(ui->time_roller_1, 115, 47);
    lv_obj_set_width(ui->time_roller_1, 38);

    //Write style for time_roller_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_radius(ui->time_roller_1, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->time_roller_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->time_roller_1, lv_color_hex(0x444444), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->time_roller_1, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->time_roller_1, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->time_roller_1, &lv_font_montserratMedium_14, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->time_roller_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->time_roller_1, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->time_roller_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->time_roller_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for time_roller_1, Part: LV_PART_SELECTED, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->time_roller_1, 255, LV_PART_SELECTED|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->time_roller_1, lv_color_hex(0x4a5260), LV_PART_SELECTED|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->time_roller_1, LV_GRAD_DIR_NONE, LV_PART_SELECTED|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->time_roller_1, lv_color_hex(0x2195f6), LV_PART_SELECTED|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->time_roller_1, &lv_font_montserratMedium_20, LV_PART_SELECTED|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->time_roller_1, 255, LV_PART_SELECTED|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->time_roller_1, LV_TEXT_ALIGN_CENTER, LV_PART_SELECTED|LV_STATE_DEFAULT);

    lv_roller_set_visible_row_count(ui->time_roller_1, 2);
    //Write codes time_list_1
    ui->time_list_1 = lv_list_create(ui->time_cont_2);
    ui->time_list_1_item0 = lv_list_add_btn(ui->time_list_1, LV_SYMBOL_AUDIO, "save");
    ui->time_list_1_item1 = lv_list_add_btn(ui->time_list_1, LV_SYMBOL_NEXT, "labe");
    ui->time_list_1_item2 = lv_list_add_btn(ui->time_list_1, LV_SYMBOL_AUDIO, "lingsheng");
    lv_obj_set_pos(ui->time_list_1, 0, 119);
    lv_obj_set_size(ui->time_list_1, 317, 103);
    lv_obj_set_scrollbar_mode(ui->time_list_1, LV_SCROLLBAR_MODE_OFF);

    //Write style state: LV_STATE_DEFAULT for &style_time_list_1_main_main_default
    static lv_style_t style_time_list_1_main_main_default;
    ui_init_style(&style_time_list_1_main_main_default);

    lv_style_set_pad_top(&style_time_list_1_main_main_default, 5);
    lv_style_set_pad_left(&style_time_list_1_main_main_default, 0);
    lv_style_set_pad_right(&style_time_list_1_main_main_default, 0);
    lv_style_set_pad_bottom(&style_time_list_1_main_main_default, 0);
    lv_style_set_bg_opa(&style_time_list_1_main_main_default, 255);
    lv_style_set_bg_color(&style_time_list_1_main_main_default, lv_color_hex(0x282294));
    lv_style_set_bg_grad_dir(&style_time_list_1_main_main_default, LV_GRAD_DIR_VER);
    lv_style_set_bg_grad_color(&style_time_list_1_main_main_default, lv_color_hex(0x404040));
    lv_style_set_bg_main_stop(&style_time_list_1_main_main_default, 0);
    lv_style_set_bg_grad_stop(&style_time_list_1_main_main_default, 0);
    lv_style_set_border_width(&style_time_list_1_main_main_default, 2);
    lv_style_set_border_opa(&style_time_list_1_main_main_default, 255);
    lv_style_set_border_color(&style_time_list_1_main_main_default, lv_color_hex(0xe1e6ee));
    lv_style_set_border_side(&style_time_list_1_main_main_default, LV_BORDER_SIDE_FULL);
    lv_style_set_radius(&style_time_list_1_main_main_default, 5);
    lv_style_set_shadow_width(&style_time_list_1_main_main_default, 0);
    lv_obj_add_style(ui->time_list_1, &style_time_list_1_main_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style state: LV_STATE_DEFAULT for &style_time_list_1_main_scrollbar_default
    static lv_style_t style_time_list_1_main_scrollbar_default;
    ui_init_style(&style_time_list_1_main_scrollbar_default);

    lv_style_set_radius(&style_time_list_1_main_scrollbar_default, 3);
    lv_style_set_bg_opa(&style_time_list_1_main_scrollbar_default, 255);
    lv_style_set_bg_color(&style_time_list_1_main_scrollbar_default, lv_color_hex(0xffffff));
    lv_style_set_bg_grad_dir(&style_time_list_1_main_scrollbar_default, LV_GRAD_DIR_NONE);
    lv_obj_add_style(ui->time_list_1, &style_time_list_1_main_scrollbar_default, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);

    //Write style state: LV_STATE_DEFAULT for &style_time_list_1_extra_btns_main_default
    static lv_style_t style_time_list_1_extra_btns_main_default;
    ui_init_style(&style_time_list_1_extra_btns_main_default);

    lv_style_set_pad_top(&style_time_list_1_extra_btns_main_default, 5);
    lv_style_set_pad_left(&style_time_list_1_extra_btns_main_default, 10);
    lv_style_set_pad_right(&style_time_list_1_extra_btns_main_default, 0);
    lv_style_set_pad_bottom(&style_time_list_1_extra_btns_main_default, 10);
    lv_style_set_border_width(&style_time_list_1_extra_btns_main_default, 1);
    lv_style_set_border_opa(&style_time_list_1_extra_btns_main_default, 255);
    lv_style_set_border_color(&style_time_list_1_extra_btns_main_default, lv_color_hex(0x6f6969));
    lv_style_set_border_side(&style_time_list_1_extra_btns_main_default, LV_BORDER_SIDE_FULL);
    lv_style_set_text_color(&style_time_list_1_extra_btns_main_default, lv_color_hex(0xfef6ea));
    lv_style_set_text_font(&style_time_list_1_extra_btns_main_default, &lv_font_montserratMedium_18);
    lv_style_set_text_opa(&style_time_list_1_extra_btns_main_default, 255);
    lv_style_set_radius(&style_time_list_1_extra_btns_main_default, 0);
    lv_style_set_bg_opa(&style_time_list_1_extra_btns_main_default, 0);
    lv_obj_add_style(ui->time_list_1_item2, &style_time_list_1_extra_btns_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_add_style(ui->time_list_1_item1, &style_time_list_1_extra_btns_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_add_style(ui->time_list_1_item0, &style_time_list_1_extra_btns_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style state: LV_STATE_DEFAULT for &style_time_list_1_extra_texts_main_default
    static lv_style_t style_time_list_1_extra_texts_main_default;
    ui_init_style(&style_time_list_1_extra_texts_main_default);

    lv_style_set_pad_top(&style_time_list_1_extra_texts_main_default, 6);
    lv_style_set_pad_left(&style_time_list_1_extra_texts_main_default, 5);
    lv_style_set_pad_right(&style_time_list_1_extra_texts_main_default, 0);
    lv_style_set_pad_bottom(&style_time_list_1_extra_texts_main_default, 0);
    lv_style_set_border_width(&style_time_list_1_extra_texts_main_default, 0);
    lv_style_set_text_color(&style_time_list_1_extra_texts_main_default, lv_color_hex(0x0D3055));
    lv_style_set_text_font(&style_time_list_1_extra_texts_main_default, &lv_font_montserratMedium_18);
    lv_style_set_text_opa(&style_time_list_1_extra_texts_main_default, 255);
    lv_style_set_radius(&style_time_list_1_extra_texts_main_default, 3);
    lv_style_set_transform_width(&style_time_list_1_extra_texts_main_default, 0);
    lv_style_set_bg_opa(&style_time_list_1_extra_texts_main_default, 0);

    //The custom code of time.
    add_slide_trigger(ui->time);
    
    //Update current screen layout.
    lv_obj_update_layout(ui->time);

    //Init events for screen.
    events_init_time(ui);
}
