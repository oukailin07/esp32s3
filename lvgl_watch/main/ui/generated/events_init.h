/*
* Copyright 2025 NXP
* NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
* terms, then you may not retain, install, activate or otherwise use the software.
*/


#ifndef EVENTS_INIT_H_
#define EVENTS_INIT_H_
#ifdef __cplusplus
extern "C" {
#endif

#include "gui_guider.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#define SCREEN_HEIGHT 240

#define SLIDE_ZONE      40         // 可触发滑动的区域高度
#define SLIDE_MAX_OFFSET 80        // 滑动可超出的最大范围
#define SLIDE_HEIGHT    240        // 页面高度，根据实际设置


void events_init_wifi_set_page(lv_ui *ui);
void events_init_setting_page(lv_ui *ui);

void events_init(lv_ui *ui);

void events_init_screen(lv_ui *ui);
void screen_event_handler(lv_event_t *e);
void events_init_time(lv_ui *ui);
void events_init_file_folder_page(lv_ui *ui);
void file_folder_page_list_btn_event_handler(lv_event_t *e);
bool path_join(char *dst, size_t size, const char *base, const char *sub);
#ifdef __cplusplus
}
#endif
#endif /* EVENT_CB_H_ */
