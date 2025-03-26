#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"
#include "lvgl.h"
#include "lv_port_disp.h"
#include "lv_port_indev.h"
//#include "lv_demos.h"
#include "lib_lcd7735.h"
static esp_timer_handle_t lvgl_tick_timer = NULL;
 
// 定时回调函数，每 1ms 触发
static void lv_tick_task(void *arg) {
    lv_tick_inc(1);
}
 
// 初始化 LVGL Tick 定时器
void lvgl_tick_timer_init(void) {
    const esp_timer_create_args_t timer_args = {
        .callback = &lv_tick_task,
        .arg = NULL,
        .dispatch_method = ESP_TIMER_TASK,
        .name = "lv_tick_timer"
    };
 
    esp_timer_create(&timer_args, &lvgl_tick_timer);
    esp_timer_start_periodic(lvgl_tick_timer, 1000); // 1ms 触发
}
 
void app_main(void)
{
    lvgl_tick_timer_init();  // 初始化 LVGL 时基
 
    lv_init();              /* 初始化LVGL图形库 */
    lv_port_disp_init();    /* lvgl显示接口初始化,放在lv_init()的后面 */
    //lv_port_indev_init();   /* lvgl输入接口初始化,放在lv_init()的后面 */
 
    // /*在屏幕中间创建一个120*50大小的按钮*/
    // lv_obj_t* switch_obj = lv_switch_create(lv_scr_act());
    // lv_obj_set_size(switch_obj, 120, 50);
    // lv_obj_align(switch_obj, LV_ALIGN_CENTER, 0, 0);
 
    //lv_demo_music();
    //很简单,只需要修改一下对应的变量名即可
 
    LV_IMG_DECLARE(ezgif);
    lv_obj_t* img = lv_gif_create(lv_scr_act());
    lv_gif_set_src(img, &ezgif);
    lv_obj_align(img, LV_ALIGN_CENTER, 0, 0);
    
    while (1) 
    {
        //LCD_DrawRectangle(0, 0, 128-1, 160-1, 0xF800);
        lv_task_handler();  // LVGL 任务管理
        vTaskDelay(pdMS_TO_TICKS(10));  // 延迟 10ms
    }
}