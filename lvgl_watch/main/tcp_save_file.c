#include "tcp_save_file.h"
#include "esp_crc.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <inttypes.h>
#define TCP_PORT 1234
#define BUFFER_SIZE 1024
#define QUEUE_SIZE 10
#define MAX_FILEPATH_LEN 256
#define MAX_TEMP_PATH_LEN 512

static const char *TAG = "tcp_file_receiver";

typedef struct {
    uint8_t data[BUFFER_SIZE];
    size_t length;
    bool is_last_chunk;
} file_chunk_t;

static QueueHandle_t file_queue = NULL;
static char current_filepath[MAX_FILEPATH_LEN] = {0};
static uint32_t expected_crc = 0;
static uint32_t calculated_crc = 0;

#define BASE_PATH "/sdcard/"
#define BASE_PATH_LEN (sizeof(BASE_PATH) - 1)  // 不包含末尾 \0

// 替换非法字符，保留 /
static void sanitize_filename(char *filename) {
    for (size_t i = 0; filename[i]; i++) {
        unsigned char c = (unsigned char)filename[i];
        if (c == '/') continue;
        if (!(isalnum(c) || c == '.' || c == '_' || c == '-')) {
            filename[i] = '_';
        }
    }
}

// 创建多级目录
static void create_parent_dir(const char *path) {
    char *dir = strdup(path);
    if (!dir) return;
    char *p = dir;

    // 跳过 "/sdcard/"
    if (strncmp(dir, BASE_PATH, BASE_PATH_LEN) == 0) {
        p = dir + BASE_PATH_LEN;
    }

    for (char *s = p; *s; s++) {
        if (*s == '/') {
            *s = '\0';
            mkdir(dir, 0777);  // 如果已存在会自动跳过
            *s = '/';
        }
    }

    free(dir);
}

static void process_file_path(const uint8_t *buffer) {
    char sanitized[MAX_FILEPATH_LEN];

    // 计算最多可接受的路径长度
    size_t max_filename_len = MAX_FILEPATH_LEN - BASE_PATH_LEN - 1;

    // 限制 buffer 长度，防止溢出
    strncpy(sanitized, (const char *)buffer, max_filename_len);
    sanitized[max_filename_len] = '\0';

    sanitize_filename(sanitized);

    // 用 snprintf 安全拼接完整路径，写入全局 current_filepath
    int written = snprintf(current_filepath, MAX_FILEPATH_LEN, BASE_PATH "%s", sanitized);
    if (written >= MAX_FILEPATH_LEN) {
        ESP_LOGE(TAG, "File path too long, truncated: %s", current_filepath);
        return;
    }

    create_parent_dir(current_filepath);
    ESP_LOGI(TAG, "Final path: %s", current_filepath);
}


// 文件写入任务
static void file_writer_task(void *arg) {
    FILE *file = NULL;
    file_chunk_t chunk;
    while (1) {
        if (xQueueReceive(file_queue, &chunk, portMAX_DELAY)) {
            if (chunk.is_last_chunk) {
                if (file) {
                    fclose(file);
                    ESP_LOGI(TAG, "Finished file.");
                    if (expected_crc && calculated_crc != expected_crc) {
                        ESP_LOGE(TAG, "CRC mismatch! expected: ");

                    }
                    file = NULL;
                }
                continue;
            }

            if (!file) {
                file = fopen(current_filepath, "wb");  // 覆盖写
                if (!file) {
                    ESP_LOGE(TAG, "Failed to open file: %s", strerror(errno));
                    continue;
                }
                calculated_crc = 0;
                ESP_LOGI(TAG, "Start writing: %s", current_filepath);
            }

            fwrite(chunk.data, 1, chunk.length, file);
            calculated_crc = esp_crc32_le(calculated_crc, chunk.data, chunk.length);
        }
    }
}

// TCP 接收任务
static void tcp_receive_task(void *arg) {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_addr = {0}, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    uint8_t buffer[BUFFER_SIZE];
    int client_fd;
    size_t filename_len = 0;
    bool filename_received = false;

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(TCP_PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    listen(server_fd, 1);

    while (1) {
        ESP_LOGI(TAG, "Waiting for connection...");
        client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &addr_len);
        ESP_LOGI(TAG, "Client connected");

        filename_received = false;
        expected_crc = 0;

        while (1) {
            ssize_t received = recv(client_fd, buffer, BUFFER_SIZE, 0);
            if (received <= 0) break;

            if (!filename_received) {
                char *null_pos = memchr(buffer, '\0', received);
                if (null_pos) {
                    filename_len = null_pos - (char *)buffer;
                    process_file_path(buffer);
                    filename_received = true;

                    size_t remain = received - filename_len - 1;
                    if (remain > 0) {
                        file_chunk_t chunk = {
                            .length = remain,
                            .is_last_chunk = false
                        };
                        memcpy(chunk.data, buffer + filename_len + 1, remain);
                        xQueueSend(file_queue, &chunk, portMAX_DELAY);
                    }
                    continue;
                } else {
                    ESP_LOGE(TAG, "Filename not null-terminated");
                    break;
                }
            }

            if (memcmp(buffer, "CRC:", 4) == 0) {
                expected_crc = strtoul((char *)buffer + 4, NULL, 16);
                ESP_LOGI(TAG, "Received CRC");
                break;
            }

            file_chunk_t chunk = {
                .length = received,
                .is_last_chunk = false
            };
            memcpy(chunk.data, buffer, received);
            xQueueSend(file_queue, &chunk, portMAX_DELAY);
        }

        file_chunk_t final = {.is_last_chunk = true};
        xQueueSend(file_queue, &final, portMAX_DELAY);
        close(client_fd);
        ESP_LOGI(TAG, "Client disconnected");
    }
}

// 启动 TCP 文件接收服务
void tcp_file_start(void) {
    file_queue = xQueueCreate(QUEUE_SIZE, sizeof(file_chunk_t));
    xTaskCreatePinnedToCore(tcp_receive_task, "tcp_receive_task", 8192, NULL, 5, NULL, 1);
    xTaskCreatePinnedToCore(file_writer_task, "file_writer_task", 8192, NULL, 5, NULL, 0);
}
