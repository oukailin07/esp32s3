#include "esp_timer.h"
#include "lvgl.h"
#include "ui_mjpeg_player.h"
#include <string.h>
#include "esp32_s3_szp.h"
#include "esp_spiffs.h"
#include "sd_card.h"
//#include "mjpeg_frame.h"
//#include "img_converters.h"
#include "esp_log.h"
static char s_current_path[256] = "/sdcard"; // 当前路径
static lv_obj_t* s_lv_back_dir_btn = NULL;   // 返回按钮
#define TAG     "ui_jpeg"

//文件页面标题
static lv_obj_t* s_lv_file_title = NULL;

//文件列表
static lv_obj_t* s_lv_file_list = NULL;

//文件列表页面
static lv_obj_t* s_file_page = NULL;

//播放页面
static lv_obj_t* s_player_page = NULL;

//页面图像
lv_obj_t* s_lv_player_img = NULL;
//返回按键
static lv_obj_t* s_lv_back_img = NULL;
//停止播放按键
static lv_obj_t* s_lv_pause_img = NULL;
//播放定时器
 lv_timer_t* s_player_timer = NULL;
//定义一个标志，用于指示暂停/启动播放
static bool s_pause_flag = false;
lv_coord_t hor_res;
lv_coord_t ver_res;
void lv_player_timer(struct _lv_timer_t * t);

void lv_back_dir_event_cb(lv_event_t *e)
{
    // 返回上级目录
    char *last_slash = strrchr(s_current_path, '/');
    if (last_slash && last_slash != s_current_path) {
        *last_slash = '\0';
    } else {
        strcpy(s_current_path, "/sdcard");
    }
    refresh_file_list_page();
}

void refresh_file_list_page()
{
    lv_obj_clean(s_lv_file_list);

    const char (*filelist)[256] = NULL;
    int filenum = sdcard_filelist_in_dir(&filelist, s_current_path);
    for(int i = 0; i < filenum; i++) {
        lv_obj_t* btn = lv_list_add_btn(s_lv_file_list, NULL, *filelist);
        lv_obj_set_style_text_font(btn, &lv_font_montserrat_18, 0);
        lv_obj_add_event_cb(btn, lv_list_event_cb, LV_EVENT_CLICKED, NULL);
        filelist++;
    }

    // 如果不在根目录，显示返回按钮
    if (strcmp(s_current_path, "/sdcard") != 0 && s_lv_back_dir_btn == NULL) {
        s_lv_back_dir_btn = lv_btn_create(s_file_page);
        lv_obj_set_size(s_lv_back_dir_btn, 80, 40);
        lv_obj_align(s_lv_back_dir_btn, LV_ALIGN_BOTTOM_LEFT, 10, -10);
        lv_obj_t* label = lv_label_create(s_lv_back_dir_btn);
        lv_label_set_text(label, "Back");
        lv_obj_center(label);
        lv_obj_add_event_cb(s_lv_back_dir_btn, lv_back_dir_event_cb, LV_EVENT_CLICKED, NULL);
    }
    else if (strcmp(s_current_path, "/sdcard") == 0 && s_lv_back_dir_btn) {
        lv_obj_del(s_lv_back_dir_btn);
        s_lv_back_dir_btn = NULL;
    }
}

void lv_list_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);
    ESP_LOGI(TAG,"list btn clicked");
    
    switch(code)
    {
        case LV_EVENT_CLICKED:
        {    
            const char* text = lv_list_get_btn_text(s_lv_file_list,obj);
            if(text)
            {
                // 判断是否是文件夹（假设文件夹没有扩展名）
                if (strchr(text, '.') == NULL) {
                    // 进入文件夹
                    strcat(s_current_path, "/");
                    strcat(s_current_path, text);
                    refresh_file_list_page();
                }
                // 判断是否.mjpeg文件
                else if(strstr(text,".mjpeg") || strstr(text,".MJPEG") || strstr(text,".MJP"))
                {
                    if(s_player_timer)  //当前存在播放定时器，说明处于播放中，不允许重复再播放
                    {
                        return;
                    }
                    lv_scr_load(s_player_page); //加载视频播放页面
                    ESP_LOGI(TAG,"start play file:%s",text);
                }
                else if(strstr(text,".gif"))
                {
                    ESP_LOGI(TAG,"start play file:%s",text);
                    char path[512]= {0};
                    snprintf(path, sizeof(path), "%s/%s", s_current_path, text);
                    ESP_LOGI(TAG,"start play file:%s",path);
                    //创建一个播放页面
                    s_player_page = lv_obj_create(NULL);
                    lv_obj_set_style_bg_color(s_player_page,lv_color_black(),0);
                    //lv_scr_load(s_player_page); //加载视频播放页面
                    lv_scr_load_anim(s_player_page, LV_SCR_LOAD_ANIM_NONE, 100, 100, false);
                    s_lv_player_img = lv_gif_create(lv_scr_act());
                    lv_obj_align(s_lv_player_img,LV_ALIGN_TOP_MID,0,0);
                    lv_gif_set_src(s_lv_player_img, path);
                }
            }
            break;
        }
        default:break;
    }
}

// void lv_player_timer(struct _lv_timer_t * t)
// {
//     static jpeg_frame_data_t frame_data = {0, 0};

//     // ==== 帧率统计相关 ====
//     static int frame_count = 0;
//     static int64_t last_time_us = 0;
//     int64_t now_us = esp_timer_get_time(); // 当前时间，单位微秒


//     frame_count++;
//     if (last_time_us == 0) {
//         last_time_us = now_us;
//     }
//     if (now_us - last_time_us >= 1000000) {  // 每秒统计一次
//         float fps = frame_count * 1000000.0f / (now_us - last_time_us);
//         printf("Current display FPS: %.2f\n", fps);
//         last_time_us = now_us;
//         frame_count = 0;
//     }

//     if (frame_data.frame) {
//         free(frame_data.frame);
//         frame_data.frame = NULL;
//         frame_data.len = 0;
//     }
//     // 获取一帧jpg图像
//     jpeg_frame_get_one(&frame_data);
//     if (frame_data.len) {
//         static lv_img_dsc_t img_dsc;
//         memset(&img_dsc, 0, sizeof(lv_img_dsc_t));
//         static uint8_t *rgb565_data = NULL;
//         uint16_t width = 0, height = 0;

//         if (rgb565_data) {
//             free(rgb565_data);
//             rgb565_data = NULL;
//         }

//         // 转为RGB565格式
//         if (jpg2rgb565(frame_data.frame, frame_data.len, &rgb565_data, &width, &height, JPG_SCALE_NONE)) {
//             img_dsc.header.cf = LV_IMG_CF_TRUE_COLOR;
//             img_dsc.data = rgb565_data;
//             img_dsc.data_size = (uint32_t)width * (uint32_t)height * 2;
//             img_dsc.header.h = height;
//             img_dsc.header.w = width;
//             lv_img_set_src(s_lv_player_img, &img_dsc);
//         }

//     } 
//     else {
//         lv_timer_del(s_player_timer);
//         s_player_timer = NULL;
//     }
// }



void lv_player_btn_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);
    if(code == LV_EVENT_CLICKED)
    {
        if(obj == s_lv_back_img)        //点击了返回按键，停止播放，关闭jpeg解析任务，返回文件列表
        {
            //if(s_player_timer)
                //lv_timer_del(s_player_timer);
            //s_player_timer = NULL;

            lv_obj_del(s_lv_player_img);
            s_lv_player_img = NULL;
            lv_scr_load(s_file_page);
        }
        else if(obj == s_lv_pause_img)  //暂停按键
        {
            s_pause_flag = !s_pause_flag;
        }
    }
}

void ui_mjpeg_create(void)
{
    //获取屏幕大小
    hor_res = lv_disp_get_hor_res(NULL);     //水平
    ver_res = lv_disp_get_ver_res(NULL);     //垂直
    lv_obj_set_style_bg_color(lv_scr_act(),lv_color_black(),0);
    
    //创建文件列表页面
    s_file_page = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(s_file_page,lv_color_black(),0);

    //创建标题
    s_lv_file_title = lv_label_create(s_file_page);
    lv_label_set_text(s_lv_file_title,"SDCard File");
    lv_obj_set_size(s_lv_file_title,hor_res-60,30);
    lv_obj_align(s_lv_file_title,LV_ALIGN_TOP_MID,0,10);
    lv_obj_set_style_text_font(s_lv_file_title,&lv_font_montserrat_24,0);
    lv_obj_set_style_text_color(s_lv_file_title,lv_color_black(),0);
    lv_obj_set_style_bg_color(s_lv_file_title,lv_color_white(),0);
    lv_obj_set_style_bg_opa(s_lv_file_title,LV_OPA_COVER,0);

    //创建文件列表
    s_lv_file_list = lv_list_create(s_file_page);
    lv_obj_set_size(s_lv_file_list,hor_res-60,ver_res - 100);
    //lv_obj_set_pos(s_lv_file_list,0,40);
    lv_obj_align(s_lv_file_list,LV_ALIGN_BOTTOM_MID,0,-40);
    lv_obj_set_style_text_color(s_lv_file_title,lv_color_black(),0);
    lv_obj_set_style_bg_color(s_lv_file_title,lv_color_white(),0);
    lv_obj_set_style_bg_opa(s_lv_file_title,LV_OPA_COVER,0);
    
    strcpy(s_current_path, "/sdcard");
    refresh_file_list_page();
    // //添加文件列表
    // const char (*filelist)[256] = NULL;
    // //sdcard_check();
    // int filenum = sdcard_filelist(&filelist);
    // for(int i = 0;i < filenum;i++)
    // {
    //     lv_obj_t* btn = lv_list_add_btn(s_lv_file_list,NULL,*filelist);
    //     lv_obj_set_style_text_font(btn,&lv_font_montserrat_18,0);
    //     filelist++;
    //     lv_obj_add_event_cb(btn,lv_list_event_cb,LV_EVENT_CLICKED,NULL);
    // }
 

    //创建一个图像，用于播放jpeg图片视频
    //s_lv_player_img = lv_img_create(s_player_page);
    //lv_obj_align(s_lv_player_img,LV_ALIGN_TOP_MID,0,35);

    //创建一个图像按键，用于返回按键
    s_lv_back_img = lv_imgbtn_create(s_player_page);
    lv_imgbtn_set_src(s_lv_back_img,LV_IMGBTN_STATE_RELEASED,NULL,"/img/back_img_48.png",NULL);
    //lv_imgbtn_set_src(s_lv_back_img,LV_IMGBTN_STATE_RELEASED,NULL,LV_SYMBOL_LEFT,NULL);
    lv_obj_set_size(s_lv_back_img,48,48);
    lv_obj_align(s_lv_back_img,LV_ALIGN_BOTTOM_LEFT,30,-20);
    //添加点击事件处理
    lv_obj_add_event_cb(s_lv_back_img,lv_player_btn_event_cb,LV_EVENT_CLICKED,NULL);

    //创建一个图像按键，用于停止播放
    s_lv_pause_img = lv_imgbtn_create(s_player_page);
    lv_imgbtn_set_src(s_lv_pause_img,LV_IMGBTN_STATE_RELEASED,NULL,"/img/pause_img_48.png",NULL);
    //lv_imgbtn_set_src(s_lv_pause_img,LV_IMGBTN_STATE_RELEASED,NULL,LV_SYMBOL_LEFT,NULL);
    lv_obj_set_size(s_lv_pause_img,48,48);
    lv_obj_align(s_lv_pause_img,LV_ALIGN_BOTTOM_MID,0,-20);
    //添加点击事件处理
    lv_obj_add_event_cb(s_lv_pause_img,lv_player_btn_event_cb,LV_EVENT_CLICKED,NULL);
    
    // jpeg_frame_cfg_t  frame_cfg = 
    // {
    //     .buff_size = 100*1024,
    // };
    // jpeg_frame_config(&frame_cfg);

    //加载文件列表页面
    
    lv_scr_load(s_file_page);

    //lv_scr_load(s_player_page);
}


