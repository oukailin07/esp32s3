#ifndef TCP_SAVE_FILE_H
#define TCP_SAVE_FILE_H
#include "lwip/sockets.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_timer.h"
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include "lvgl.h"
#include "esp32_s3_szp.h"
#include <ctype.h>  // 确保包含这个头文件
#include <inttypes.h>  // 必须包含这个头文件
void tcp_file_start(void);
#endif