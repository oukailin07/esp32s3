#ifndef BSP_SPIFFS_H
#define BSP_SPIFFS_H
#include <string.h>
#include "esp_err.h"
#include "esp_spiffs.h"
#include "esp_log.h"
#include <dirent.h>
#include <sys/stat.h>  // 添加头文件
esp_err_t bsp_spiffs_init(char *partition_label, char *mount_point, size_t max_files);
void list_dir(const char *path);
#define EXAMPLE_MAX_CHAR_SIZE    64
#endif // SPIFFS_H