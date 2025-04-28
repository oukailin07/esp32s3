#include "bsp_spiffs.h"
static const char *TAG = "bsp_spiffs";
esp_err_t bsp_spiffs_init(char *partition_label, char *mount_point, size_t max_files)
{
    
    esp_vfs_spiffs_conf_t conf = {
        .base_path = mount_point,
        .partition_label = partition_label,
        .max_files = max_files,
        .format_if_mount_failed = false,
    };

    esp_err_t ret_val = esp_vfs_spiffs_register(&conf);

    if (ESP_OK != ret_val) {
        ESP_LOGW(TAG, "spiffs register fail!");
        return ret_val;
    }

    size_t total = 0, used = 0;
    ret_val = esp_spiffs_info(conf.partition_label, &total, &used);
    if (ret_val != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get SPIFFS partition information (%s)", esp_err_to_name(ret_val));
    } else {
        ESP_LOGI(TAG, "Partition size: total: %d, used: %d", total, used);
    }

    return ret_val;
}



void list_dir(const char *path) {
    DIR *dir = opendir(path);
    if (!dir) {
        ESP_LOGE("SPIFFS", "Failed to open directory: %s", path);
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        // 使用更大的缓冲区（如 512 字节）
        char full_path[512];
        int ret = snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);
        
        // 检查是否截断
        if (ret >= sizeof(full_path)) {
            ESP_LOGW("SPIFFS", "Path truncated: %s/%s", path, entry->d_name);
            continue;
        }

        if (entry->d_type == DT_DIR) {
            ESP_LOGI("SPIFFS", "DIR: %s", full_path);
            list_dir(full_path);  // 递归遍历子目录
        } else {
            ESP_LOGI("SPIFFS", "FILE: %s", full_path);
        }
    }
    closedir(dir);
}