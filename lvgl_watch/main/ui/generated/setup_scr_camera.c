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
#include "esp32_s3_szp.h"
extern lv_ui guider_ui;
// 摄像头图像
lv_img_dsc_t img_camera_dsc = {
  .header.cf = LV_IMG_CF_TRUE_COLOR,
  .header.always_zero = 0,
  .header.reserved = 0,
  .header.w = 320,
  .header.h = 240,
  .data_size = 240*320*2,
};

// 摄像头处理任务
static void task_process_camera(void *arg)
{
    while (guider_ui.page_flage == CAMERA_PAGE)
    {
       
        camera_fb_t *frame = esp_camera_fb_get();
        if (frame&& frame->buf) {
            lvgl_port_lock(0); // LVGL 线程锁
            img_camera_dsc.data = frame->buf;
            if (guider_ui.img_camera && lv_obj_is_valid(guider_ui.img_camera)) { // 检查对象是否有效
                img_camera_dsc.data = frame->buf;
                lv_img_set_src(guider_ui.img_camera, &img_camera_dsc);
            }
            lvgl_port_unlock();
            esp_camera_fb_return(frame);
        } else {
            printf("Failed to get camera frame!\n");
        }
        vTaskDelay(1);
    }
    lvgl_port_lock(0); // LVGL 线程锁
    // 确保摄像头反初始化
    if (esp_camera_deinit() != ESP_OK) {
        printf("Failed to deinit camera!\n");
    }
    dvp_pwdn(1); // 摄像头掉电
    esp_camera_fb_return(NULL); // 确保释放所有帧
    printf("task_process_camera free!\n");
    lvgl_port_unlock();
    vTaskDelete(NULL);
}

void setup_scr_camera(lv_ui *ui)
{
    printf("open setup_scr_camera");
    ui->cur_page = ui->camera;
    ui->page_flage = CAMERA_PAGE;
    if (bsp_camera_init() != ESP_OK) {
        printf("Camera init failed!\n");
        return;
    }
    //Write codes camera
    ui->camera = lv_obj_create(NULL);
    lv_obj_set_size(ui->camera, 320, 240);
    lv_obj_set_scrollbar_mode(ui->camera, LV_SCROLLBAR_MODE_OFF);
    //Write style for camera, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->camera, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    ui->img_camera = lv_img_create(ui->camera);
    lv_obj_set_pos(ui->img_camera, 0, 0);
    lv_obj_set_size(ui->img_camera, 320, 240);
    // 初始化摄像头硬件


    xTaskCreatePinnedToCore(task_process_camera, "task_process_camera", 4 * 1024, NULL, 5, NULL, 1);
    //The custom code of camera.

    add_slide_trigger(ui->camera);
    //Update current screen layout.
    lv_obj_update_layout(ui->camera);

}
