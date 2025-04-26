#ifndef _UI_MJPEG_PLAYER_H_
#define _UI_MJPEG_PLAYER_H_

/** 创建mjpeg视频播放ui
 * @param 无
 * @return 无
*/
void ui_mjpeg_create(void);
void refresh_file_list_page();
void lv_list_event_cb(lv_event_t * e);
void lv_back_dir_event_cb(lv_event_t *e);
void lv_player_btn_event_cb(lv_event_t * e);
#endif
