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



void setup_scr_screen(lv_ui *ui)
{
    printf("open setup_scr_screen");
    ui->cur_page = ui->screen;
    ui->page_flage = HOME_PAGE;
    //Write codes screen
    ui->screen = lv_obj_create(NULL);
    lv_obj_set_size(ui->screen, 320, 240);
    lv_obj_set_scrollbar_mode(ui->screen, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_add_flag(ui->screen, LV_OBJ_FLAG_OVERFLOW_VISIBLE);
    //Write codes screen_img_1
    ui->screen_img_1 = lv_img_create(ui->screen);
    lv_obj_add_flag(ui->screen_img_1, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_img_1, &_wifi1_alpha_50x50);
    lv_img_set_pivot(ui->screen_img_1, 50,50);
    lv_img_set_angle(ui->screen_img_1, 0);
    lv_obj_set_pos(ui->screen_img_1, 272, 0);
    lv_obj_set_size(ui->screen_img_1, 50, 50);
    lv_obj_add_flag(ui->screen_img_1, LV_OBJ_FLAG_EVENT_BUBBLE);

    //Write style for screen_img_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_img_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_img_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_img_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_img_1, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_img_2
    ui->screen_img_2 = lv_img_create(ui->screen);
    lv_obj_add_flag(ui->screen_img_2, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_img_2, &_icon_alpha_50x50);
    lv_img_set_pivot(ui->screen_img_2, 50,50);
    lv_img_set_angle(ui->screen_img_2, 0);
    lv_obj_set_pos(ui->screen_img_2, 222, 0);
    lv_obj_set_size(ui->screen_img_2, 50, 50);
    lv_obj_add_flag(ui->screen_img_2, LV_OBJ_FLAG_EVENT_BUBBLE);

    //Write style for screen_img_2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_img_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_img_2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_img_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_img_2, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_img_3
    ui->screen_img_3 = lv_img_create(ui->screen);
    lv_obj_add_flag(ui->screen_img_3, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_img_3, &_wenjianjia3_alpha_100x100);
    lv_img_set_pivot(ui->screen_img_3, 50,50);
    lv_img_set_angle(ui->screen_img_3, 0);
    lv_obj_set_pos(ui->screen_img_3, 1, 50);
    lv_obj_set_size(ui->screen_img_3, 100, 100);
    lv_obj_add_flag(ui->screen_img_3, LV_OBJ_FLAG_EVENT_BUBBLE);

    //Write style for screen_img_3, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_img_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_img_3, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_img_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_img_3, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_img_4
    ui->screen_img_4 = lv_img_create(ui->screen);
    lv_obj_add_flag(ui->screen_img_4, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_img_4, &_xiangji_alpha_100x100);
    lv_img_set_pivot(ui->screen_img_4, 50,50);
    lv_img_set_angle(ui->screen_img_4, 0);
    lv_obj_set_pos(ui->screen_img_4, 107, 50);
    lv_obj_set_size(ui->screen_img_4, 100, 100);
    lv_obj_add_flag(ui->screen_img_4, LV_OBJ_FLAG_EVENT_BUBBLE);

    //Write style for screen_img_4, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_img_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_img_4, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_img_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_img_4, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_img_5
    ui->screen_img_5 = lv_img_create(ui->screen);
    lv_obj_add_flag(ui->screen_img_5, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_img_5, &_yuncai_alpha_100x100);
    lv_img_set_pivot(ui->screen_img_5, 50,50);
    lv_img_set_angle(ui->screen_img_5, 0);
    lv_obj_set_pos(ui->screen_img_5, 216, 55);
    lv_obj_set_size(ui->screen_img_5, 100, 100);
    lv_obj_add_flag(ui->screen_img_5, LV_OBJ_FLAG_EVENT_BUBBLE);

    //Write style for screen_img_5, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_img_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_img_5, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_img_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_img_5, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_img_6
    ui->screen_img_6 = lv_img_create(ui->screen);
    lv_obj_add_flag(ui->screen_img_6, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_img_6, &_naozhong_alpha_100x100);
    lv_img_set_pivot(ui->screen_img_6, 50,50);
    lv_img_set_angle(ui->screen_img_6, 0);
    lv_obj_set_pos(ui->screen_img_6, 1, 138);
    lv_obj_set_size(ui->screen_img_6, 100, 100);
    lv_obj_add_flag(ui->screen_img_6, LV_OBJ_FLAG_EVENT_BUBBLE);

    //Write style for screen_img_6, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_img_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_img_6, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_img_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_img_6, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_img_7
    ui->screen_img_7 = lv_img_create(ui->screen);
    lv_obj_add_flag(ui->screen_img_7, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_img_7, &_xiangce_alpha_100x100);
    lv_img_set_pivot(ui->screen_img_7, 50,50);
    lv_img_set_angle(ui->screen_img_7, 0);
    lv_obj_set_pos(ui->screen_img_7, 116, 138);
    lv_obj_set_size(ui->screen_img_7, 100, 100);
    lv_obj_add_flag(ui->screen_img_7, LV_OBJ_FLAG_EVENT_BUBBLE);

    //Write style for screen_img_7, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_img_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_img_7, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_img_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_img_7, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_img_8
    ui->screen_img_8 = lv_img_create(ui->screen);
    lv_obj_add_flag(ui->screen_img_8, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_img_8, &_zhinanzhen_alpha_100x100);
    lv_img_set_pivot(ui->screen_img_8, 50,50);
    lv_img_set_angle(ui->screen_img_8, 0);
    lv_obj_set_pos(ui->screen_img_8, 222, 135);
    lv_obj_set_size(ui->screen_img_8, 100, 100);
    lv_obj_add_flag(ui->screen_img_8, LV_OBJ_FLAG_EVENT_BUBBLE);

    //Write style for screen_img_8, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_img_8, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_img_8, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_img_8, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_img_8, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_tileview_1
    ui->screen_tileview_1 = lv_tileview_create(ui->screen);
    ui->screen_tileview_1_tile = lv_tileview_add_tile(ui->screen_tileview_1, 0, 0, LV_DIR_RIGHT);
    lv_obj_set_pos(ui->screen_tileview_1, 322, 0);
    lv_obj_set_size(ui->screen_tileview_1, 320, 240);
    lv_obj_set_scrollbar_mode(ui->screen_tileview_1, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_tileview_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_tileview_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_tileview_1, lv_color_hex(0xf6f6f6), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_tileview_1, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_tileview_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_tileview_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_tileview_1, Part: LV_PART_SCROLLBAR, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_tileview_1, 255, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_tileview_1, lv_color_hex(0xeaeff3), LV_PART_SCROLLBAR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_tileview_1, LV_GRAD_DIR_NONE, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_tileview_1, 0, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);



    // //Write codes screen_cont_1
    // ui->screen_cont_1 = lv_obj_create(ui->screen);
    // lv_obj_set_pos(ui->screen_cont_1, 0, -240);
    // lv_obj_set_size(ui->screen_cont_1, 320, 240);
    // lv_obj_set_scrollbar_mode(ui->screen_cont_1, LV_SCROLLBAR_MODE_OFF);
    // lv_obj_add_flag(ui->screen_cont_1, LV_OBJ_FLAG_CLICKABLE);
    // lv_obj_add_flag(ui->screen_cont_1, LV_OBJ_FLAG_EVENT_BUBBLE);

    // //Write style for screen_cont_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    // lv_obj_set_style_border_width(ui->screen_cont_1, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    // lv_obj_set_style_border_opa(ui->screen_cont_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    // lv_obj_set_style_border_color(ui->screen_cont_1, lv_color_hex(0x2195f6), LV_PART_MAIN|LV_STATE_DEFAULT);
    // lv_obj_set_style_border_side(ui->screen_cont_1, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
    // lv_obj_set_style_radius(ui->screen_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    // lv_obj_set_style_bg_opa(ui->screen_cont_1, 136, LV_PART_MAIN|LV_STATE_DEFAULT);
    // lv_obj_set_style_bg_color(ui->screen_cont_1, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    // lv_obj_set_style_bg_grad_dir(ui->screen_cont_1, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    // lv_obj_set_style_pad_top(ui->screen_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    // lv_obj_set_style_pad_bottom(ui->screen_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    // lv_obj_set_style_pad_left(ui->screen_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    // lv_obj_set_style_pad_right(ui->screen_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    // lv_obj_set_style_shadow_width(ui->screen_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    // //Write codes screen_slider_1
    // ui->screen_slider_1 = lv_slider_create(ui->screen);
    // lv_slider_set_range(ui->screen_slider_1, 0, 100);
    // lv_slider_set_mode(ui->screen_slider_1, LV_SLIDER_MODE_NORMAL);
    // lv_slider_set_value(ui->screen_slider_1, 50, LV_ANIM_OFF);
    // lv_obj_set_pos(ui->screen_slider_1, 233, -142);
    // lv_obj_set_size(ui->screen_slider_1, 17, 59);

    // //Write style for screen_slider_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    // lv_obj_set_style_bg_opa(ui->screen_slider_1, 60, LV_PART_MAIN|LV_STATE_DEFAULT);
    // lv_obj_set_style_bg_color(ui->screen_slider_1, lv_color_hex(0x2195f6), LV_PART_MAIN|LV_STATE_DEFAULT);
    // lv_obj_set_style_bg_grad_dir(ui->screen_slider_1, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    // lv_obj_set_style_radius(ui->screen_slider_1, 50, LV_PART_MAIN|LV_STATE_DEFAULT);
    // lv_obj_set_style_outline_width(ui->screen_slider_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    // lv_obj_set_style_shadow_width(ui->screen_slider_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    // //Write style for screen_slider_1, Part: LV_PART_INDICATOR, State: LV_STATE_DEFAULT.
    // lv_obj_set_style_bg_opa(ui->screen_slider_1, 255, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    // lv_obj_set_style_bg_color(ui->screen_slider_1, lv_color_hex(0x2195f6), LV_PART_INDICATOR|LV_STATE_DEFAULT);
    // lv_obj_set_style_bg_grad_dir(ui->screen_slider_1, LV_GRAD_DIR_NONE, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    // lv_obj_set_style_radius(ui->screen_slider_1, 50, LV_PART_INDICATOR|LV_STATE_DEFAULT);

    // //Write style for screen_slider_1, Part: LV_PART_KNOB, State: LV_STATE_DEFAULT.
    // lv_obj_set_style_bg_opa(ui->screen_slider_1, 255, LV_PART_KNOB|LV_STATE_DEFAULT);
    // lv_obj_set_style_bg_color(ui->screen_slider_1, lv_color_hex(0x2195f6), LV_PART_KNOB|LV_STATE_DEFAULT);
    // lv_obj_set_style_bg_grad_dir(ui->screen_slider_1, LV_GRAD_DIR_NONE, LV_PART_KNOB|LV_STATE_DEFAULT);
    // lv_obj_set_style_radius(ui->screen_slider_1, 50, LV_PART_KNOB|LV_STATE_DEFAULT);

    // //Write codes screen_slider_2
    // ui->screen_slider_2 = lv_slider_create(ui->screen);
    // lv_slider_set_range(ui->screen_slider_2, 0, 100);
    // lv_slider_set_mode(ui->screen_slider_2, LV_SLIDER_MODE_NORMAL);
    // lv_slider_set_value(ui->screen_slider_2, 50, LV_ANIM_OFF);
    // lv_obj_set_pos(ui->screen_slider_2, 275, -142);
    // lv_obj_set_size(ui->screen_slider_2, 17, 59);

    // //Write style for screen_slider_2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    // lv_obj_set_style_bg_opa(ui->screen_slider_2, 60, LV_PART_MAIN|LV_STATE_DEFAULT);
    // lv_obj_set_style_bg_color(ui->screen_slider_2, lv_color_hex(0x2195f6), LV_PART_MAIN|LV_STATE_DEFAULT);
    // lv_obj_set_style_bg_grad_dir(ui->screen_slider_2, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    // lv_obj_set_style_radius(ui->screen_slider_2, 50, LV_PART_MAIN|LV_STATE_DEFAULT);
    // lv_obj_set_style_outline_width(ui->screen_slider_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    // lv_obj_set_style_shadow_width(ui->screen_slider_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    // //Write style for screen_slider_2, Part: LV_PART_INDICATOR, State: LV_STATE_DEFAULT.
    // lv_obj_set_style_bg_opa(ui->screen_slider_2, 255, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    // lv_obj_set_style_bg_color(ui->screen_slider_2, lv_color_hex(0x2195f6), LV_PART_INDICATOR|LV_STATE_DEFAULT);
    // lv_obj_set_style_bg_grad_dir(ui->screen_slider_2, LV_GRAD_DIR_NONE, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    // lv_obj_set_style_radius(ui->screen_slider_2, 50, LV_PART_INDICATOR|LV_STATE_DEFAULT);

    // //Write style for screen_slider_2, Part: LV_PART_KNOB, State: LV_STATE_DEFAULT.
    // lv_obj_set_style_bg_opa(ui->screen_slider_2, 255, LV_PART_KNOB|LV_STATE_DEFAULT);
    // lv_obj_set_style_bg_color(ui->screen_slider_2, lv_color_hex(0x2195f6), LV_PART_KNOB|LV_STATE_DEFAULT);
    // lv_obj_set_style_bg_grad_dir(ui->screen_slider_2, LV_GRAD_DIR_NONE, LV_PART_KNOB|LV_STATE_DEFAULT);
    // lv_obj_set_style_radius(ui->screen_slider_2, 50, LV_PART_KNOB|LV_STATE_DEFAULT);

    // //Write codes screen_imgbtn_1
    // ui->screen_imgbtn_1 = lv_imgbtn_create(ui->screen);
    // lv_obj_add_flag(ui->screen_imgbtn_1, LV_OBJ_FLAG_CHECKABLE);
    // lv_imgbtn_set_src(ui->screen_imgbtn_1, LV_IMGBTN_STATE_RELEASED, NULL, &_ios_alpha_50x50, NULL);
    // ui->screen_imgbtn_1_label = lv_label_create(ui->screen_imgbtn_1);
    // lv_label_set_text(ui->screen_imgbtn_1_label, "");
    // lv_label_set_long_mode(ui->screen_imgbtn_1_label, LV_LABEL_LONG_WRAP);
    // lv_obj_align(ui->screen_imgbtn_1_label, LV_ALIGN_CENTER, 0, 0);
    // lv_obj_set_style_pad_all(ui->screen_imgbtn_1, 0, LV_STATE_DEFAULT);
    // lv_obj_set_pos(ui->screen_imgbtn_1, 17, -121);
    // lv_obj_set_size(ui->screen_imgbtn_1, 50, 50);
    // lv_obj_add_flag(ui->screen_imgbtn_1, LV_OBJ_FLAG_CLICKABLE);
    // lv_obj_add_flag(ui->screen_imgbtn_1, LV_OBJ_FLAG_EVENT_BUBBLE);

    // //Write style for screen_imgbtn_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    // lv_obj_set_style_text_color(ui->screen_imgbtn_1, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    // lv_obj_set_style_text_font(ui->screen_imgbtn_1, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_DEFAULT);
    // lv_obj_set_style_text_opa(ui->screen_imgbtn_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    // lv_obj_set_style_text_align(ui->screen_imgbtn_1, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    // lv_obj_set_style_radius(ui->screen_imgbtn_1, 50, LV_PART_MAIN|LV_STATE_DEFAULT);
    // lv_obj_set_style_clip_corner(ui->screen_imgbtn_1, true, LV_PART_MAIN|LV_STATE_DEFAULT);
    // lv_obj_set_style_shadow_width(ui->screen_imgbtn_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    // //Write style for screen_imgbtn_1, Part: LV_PART_MAIN, State: LV_STATE_PRESSED.
    // lv_obj_set_style_img_recolor_opa(ui->screen_imgbtn_1, 0, LV_PART_MAIN|LV_STATE_PRESSED);
    // lv_obj_set_style_img_opa(ui->screen_imgbtn_1, 255, LV_PART_MAIN|LV_STATE_PRESSED);
    // lv_obj_set_style_text_color(ui->screen_imgbtn_1, lv_color_hex(0xFF33FF), LV_PART_MAIN|LV_STATE_PRESSED);
    // lv_obj_set_style_text_font(ui->screen_imgbtn_1, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_PRESSED);
    // lv_obj_set_style_text_opa(ui->screen_imgbtn_1, 255, LV_PART_MAIN|LV_STATE_PRESSED);
    // lv_obj_set_style_shadow_width(ui->screen_imgbtn_1, 0, LV_PART_MAIN|LV_STATE_PRESSED);

    // //Write style for screen_imgbtn_1, Part: LV_PART_MAIN, State: LV_STATE_CHECKED.
    // lv_obj_set_style_img_recolor_opa(ui->screen_imgbtn_1, 0, LV_PART_MAIN|LV_STATE_CHECKED);
    // lv_obj_set_style_img_opa(ui->screen_imgbtn_1, 255, LV_PART_MAIN|LV_STATE_CHECKED);
    // lv_obj_set_style_text_color(ui->screen_imgbtn_1, lv_color_hex(0xFF33FF), LV_PART_MAIN|LV_STATE_CHECKED);
    // lv_obj_set_style_text_font(ui->screen_imgbtn_1, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_CHECKED);
    // lv_obj_set_style_text_opa(ui->screen_imgbtn_1, 255, LV_PART_MAIN|LV_STATE_CHECKED);
    // lv_obj_set_style_shadow_width(ui->screen_imgbtn_1, 0, LV_PART_MAIN|LV_STATE_CHECKED);

    // //Write style for screen_imgbtn_1, Part: LV_PART_MAIN, State: LV_IMGBTN_STATE_RELEASED.
    // lv_obj_set_style_img_recolor_opa(ui->screen_imgbtn_1, 0, LV_PART_MAIN|LV_IMGBTN_STATE_RELEASED);
    // lv_obj_set_style_img_opa(ui->screen_imgbtn_1, 255, LV_PART_MAIN|LV_IMGBTN_STATE_RELEASED);
 
           
           
    //Update current screen layout.
    add_slide_trigger(ui->screen);
     
    lv_obj_update_layout(ui->screen);

    events_init_screen(ui);
}
