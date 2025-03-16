/* HTTP File Server Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdio.h>
#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "esp_err.h"
#include "esp_log.h"
#include "esp_spiffs.h"

/* This example demonstrates how to create file server
 * using esp_http_server. This file has only startup code.
 * Look in file_server.c for the implementation */

static const char *TAG="spiffs";

/* Function to initialize SPIFFS */
esp_err_t init_spiffs(void)
{
    ESP_LOGI(TAG, "Initializing SPIFFS");

    esp_vfs_spiffs_conf_t conf = {
      .base_path = "/spiffs",   //可以认为挂着点，后续使用C库函数fopen("/spiffs/...")
      .partition_label = NULL,  //指定spiffs分区，如果为NULL，则默认为分区表中第一个spiffs类型的分区
      .max_files = 5,           //最大可同时打开的文件数
      .format_if_mount_failed = true
    };

    //初始化和挂载spiffs分区
    esp_err_t ret = esp_vfs_spiffs_register(&conf);

    //失败处理
    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to mount or format filesystem");
        } else if (ret == ESP_ERR_NOT_FOUND) {
            ESP_LOGE(TAG, "Failed to find SPIFFS partition");
        } else {
            ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
        }
        return ESP_OK;
    }

    //执行SPIFFS文件系统检查
    ESP_LOGI(TAG, "Performing SPIFFS_check().");
    ret = esp_spiffs_check(conf.partition_label);//操作spiffs文件系统器件断电，可能会导致 SPIFFS 损坏，可通过esp_spiffs_check恢复
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "SPIFFS_check() failed (%s)", esp_err_to_name(ret));
        return ESP_OK;
    } else {
        ESP_LOGI(TAG, "SPIFFS_check() successful");
    }

    //获取SPIFFS可用区域大小
    size_t total = 0, used = 0;
    ret = esp_spiffs_info(conf.partition_label, &total, &used);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get SPIFFS partition information (%s). Formatting...", esp_err_to_name(ret));
        esp_spiffs_format(conf.partition_label);
        return ESP_OK;
    } else {
        ESP_LOGI(TAG, "Partition size: total: %d, used: %d", total, used);
    }

    //可用空间异常，执行SPIFFS检查
    if (used > total) {
        ESP_LOGW(TAG, "Number of used bytes cannot be larger than total. Performing SPIFFS_check().");
        ret = esp_spiffs_check(conf.partition_label);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "SPIFFS_check() failed (%s)", esp_err_to_name(ret));
            return ESP_OK;
        } else {
            ESP_LOGI(TAG, "SPIFFS_check() successful");
        }
    }
    return ESP_OK;
}

