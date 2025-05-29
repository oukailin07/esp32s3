/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */



/****************************************************************************
*
* This demo showcases BLE GATT client. It can scan BLE devices and connect to one device.
* Run the gatt_server demo, the client demo will automatically connect to the gatt_server demo.
* Client demo will enable gatt_server's notify after connection. The two devices will then exchange
* data.
*
****************************************************************************/

#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include "nvs.h"
#include "nvs_flash.h"
#include "esp_timer.h"
#include "esp_bt.h"
#include "esp_gap_ble_api.h"
#include "esp_gattc_api.h"
#include "esp_gatt_defs.h"
#include "esp_bt_main.h"
#include "esp_gatt_common_api.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "mbedtls/aes.h"
#define GATTC_TAG "GATTC_DEMO"
#define REMOTE_SERVICE_UUID        0xFFF0
#define REMOTE_WRITE_CHAR_UUID 0xFFF2
#define REMOTE_NOTIFY_CHAR_UUID    0xFFF1
#define PROFILE_NUM      1
#define PROFILE_A_APP_ID 0
#define INVALID_HANDLE   0

static char remote_device_name[ESP_BLE_ADV_NAME_LEN_MAX] = "XH1";
static bool connect    = false;
static bool get_server = false;
static esp_gattc_char_elem_t *char_elem_result   = NULL;
static esp_gattc_descr_elem_t *descr_elem_result = NULL;



#define REMOTE_DEVICE_NAME "XH1"

static const char *TAG = "BLE_CLIENT";

static esp_gatt_if_t client_if;
static esp_bd_addr_t remote_device_mac = {0xe4, 0xc3, 0x30, 0xda, 0xca, 0xe3};

#define MAC_ADDR_LEN 6
#define USERID_LEN 16
#define AES_KEY_LEN 16

static uint16_t conn_id;
static uint16_t service_start_handle = 0;
static uint16_t service_end_handle = 0;
static uint16_t write_char_handle = 0;
static uint16_t notify_char_handle = 0;

static uint8_t user_property = 0; // 0 = admin
static const uint8_t userid[USERID_LEN] = "e4c330dacae3";

static void pad_pkcs7(uint8_t *data, size_t data_len, size_t block_size, size_t *out_len)
{
    size_t pad_len = block_size - (data_len % block_size);
    for (size_t i = 0; i < pad_len; i++) {
        data[data_len + i] = pad_len;
    }
    *out_len = data_len + pad_len;
}

static void aes_encrypt_ecb(uint8_t *key, uint8_t *input, size_t len, uint8_t *output)
{
    mbedtls_aes_context aes;
    mbedtls_aes_init(&aes);
    mbedtls_aes_setkey_enc(&aes, key, 128);
    for (int i = 0; i < len; i += 16) {
        mbedtls_aes_crypt_ecb(&aes, MBEDTLS_AES_ENCRYPT, input + i, output + i);
    }
    mbedtls_aes_free(&aes);
}


static void build_hello_packet(uint8_t *packet, size_t *packet_len)
{
    uint8_t plain[64] = {0};
    uint8_t aes_data[64] = {0};
    size_t len;

    // 1. 添加 4 字节时间戳（秒）
    uint32_t timestamp = (uint32_t)(esp_timer_get_time() / 1000000ULL);
    plain[0] = (timestamp >> 24) & 0xFF;
    plain[1] = (timestamp >> 16) & 0xFF;
    plain[2] = (timestamp >> 8) & 0xFF;
    plain[3] = (timestamp) & 0xFF;

    // 2. 添加 "AT+HELLO=" 字符串
    sprintf((char *)(plain + 4), "AT+HELLO=");
    size_t base_len = strlen("AT+HELLO="); // 不包含时间戳

    // 3. 添加 MAC、user_property、userid、'\r'
    memcpy(plain + 4 + base_len, remote_device_mac, MAC_ADDR_LEN);
    plain[4 + base_len + MAC_ADDR_LEN] = user_property;
    memcpy(plain + 4 + base_len + MAC_ADDR_LEN + 1, userid, USERID_LEN);
    plain[4 + base_len + MAC_ADDR_LEN + 1 + USERID_LEN] = 0x0D;

    size_t total_len = 4 + base_len + MAC_ADDR_LEN + 1 + USERID_LEN + 1;

    // 👇 打印未加密明文数据（包含时间戳）
    ESP_LOG_BUFFER_HEX(TAG, plain, total_len);

    // 4. PKCS7 填充
    pad_pkcs7(plain, total_len, 16, &len);

    // 5. 构造 AES key（基于 MAC）
    uint8_t aes_key[AES_KEY_LEN] = {
        0x32, 0x31, 0x36, 0x43, 0x41, 0x41, 0x30, 0x41, 0x46, 0x35,
        remote_device_mac[0], remote_device_mac[1], remote_device_mac[2],
        remote_device_mac[3], remote_device_mac[4], remote_device_mac[5]
    };

    // 6. 加密
    aes_encrypt_ecb(aes_key, plain, len, aes_data);

    // 👇 打印加密后的数据
    ESP_LOG_BUFFER_HEX(TAG, aes_data, len);

    // 7. 封包
    size_t offset = 0;
    packet[offset++] = 0x09;
    packet[offset++] = 0x5F;
    packet[offset++] = 0x00;
    packet[offset++] = 0x00;
    packet[offset++] = (len >> 8) & 0xFF;
    packet[offset++] = len & 0xFF;
    memcpy(&packet[offset], aes_data, len);
    offset += len;

    // 8. XOR 校验
    uint8_t xor = 0;
    for (int i = 0; i < offset; i++) xor ^= packet[i];
    packet[offset++] = xor;

    // 9. 包尾
    packet[offset++] = 0x0D;

    *packet_len = offset;

    // 👇 打印最终打包好的数据包
    ESP_LOG_BUFFER_HEX(TAG, packet, *packet_len);
}

static void build_sn_packet(uint8_t *packet, size_t *packet_len)
{
    uint8_t plain[64] = {0};
    uint8_t aes_data[64] = {0};
    size_t len;

    // 1. 时间戳（4 字节）
    uint32_t timestamp = (uint32_t)(esp_timer_get_time() / 1000000ULL);
    plain[0] = (timestamp >> 24) & 0xFF;
    plain[1] = (timestamp >> 16) & 0xFF;
    plain[2] = (timestamp >> 8) & 0xFF;
    plain[3] = (timestamp) & 0xFF;

    // 2. 添加 "AT+SN=PF00A02505290001\r"
    const char *sn_cmd = "AT+SN=PF00A02505290001\r";
    size_t sn_len = strlen(sn_cmd);
    memcpy(plain + 4, sn_cmd, sn_len);

    size_t total_len = 4 + sn_len; // 总长度：时间戳 + 指令

    // 👇 打印明文
    ESP_LOG_BUFFER_HEX(TAG, plain, total_len);

    // 3. PKCS7 填充
    pad_pkcs7(plain, total_len, 16, &len);

    // 4. 构造 AES-128 ECB 密钥（基于 remote_device_mac）
    uint8_t aes_key[16] = {
        0x32, 0x31, 0x36, 0x43, 0x41, 0x41, 0x30, 0x41, 0x46, 0x35,
        remote_device_mac[0], remote_device_mac[1], remote_device_mac[2],
        remote_device_mac[3], remote_device_mac[4], remote_device_mac[5]
    };

    // 5. 加密
    aes_encrypt_ecb(aes_key, plain, len, aes_data);

    // 👇 打印加密后的数据
    ESP_LOG_BUFFER_HEX(TAG, aes_data, len);

    // 6. 封包
    size_t offset = 0;
    packet[offset++] = 0x09;  // 帧头
    packet[offset++] = 0x5F;  // 地址
    packet[offset++] = 0x00;
    packet[offset++] = 0x00;
    packet[offset++] = (len >> 8) & 0xFF;
    packet[offset++] = len & 0xFF;
    memcpy(&packet[offset], aes_data, len);
    offset += len;

    // 7. XOR 校验
    uint8_t xor = 0;
    for (int i = 0; i < offset; i++) xor ^= packet[i];
    packet[offset++] = xor;

    // 8. 包尾
    packet[offset++] = 0x0D;

    *packet_len = offset;

    // 👇 打印最终数据包
    ESP_LOG_BUFFER_HEX(TAG, packet, *packet_len);
}

// static void build_hello_packet(uint8_t *packet, size_t *packet_len)
// {
//     uint8_t plain[64] = {0};
//     size_t total_len;

//     // 添加 4 字节时间戳（以秒为单位）
//     uint32_t timestamp = (uint32_t)(esp_timer_get_time() / 1000000ULL);
//     plain[0] = (timestamp >> 24) & 0xFF;
//     plain[1] = (timestamp >> 16) & 0xFF;
//     plain[2] = (timestamp >> 8) & 0xFF;
//     plain[3] = (timestamp) & 0xFF;

//     // 构造明文 "AT+HELLO=" 字符串
//     sprintf((char *)(plain + 4), "AT+HELLO=");
//     size_t base_len = strlen("AT+HELLO=");  // 不包含时间戳

//     // 添加 MAC + user_property + userid + '\r'
//     memcpy(plain + 4 + base_len, remote_device_mac, MAC_ADDR_LEN);
//     plain[4 + base_len + MAC_ADDR_LEN] = user_property;
//     memcpy(plain + 4 + base_len + MAC_ADDR_LEN + 1, userid, USERID_LEN);
//     plain[4 + base_len + MAC_ADDR_LEN + 1 + USERID_LEN] = 0x0D;

//     total_len = 4 + base_len + MAC_ADDR_LEN + 1 + USERID_LEN + 1;


//     size_t offset = 0;
//     packet[offset++] = 0x09; // frame head
//     packet[offset++] = 0x5F; // addr
//     packet[offset++] = 0x00; // signaling
//     packet[offset++] = 0x00; // reserved
//     packet[offset++] = (total_len >> 8) & 0xFF;
//     packet[offset++] = total_len & 0xFF;

//     memcpy(&packet[offset], plain, total_len);  // 拷贝明文+时间戳
//     offset += total_len;

//     // XOR 校验
//     uint8_t xor = 0;
//     for (int i = 0; i < offset; i++) xor ^= packet[i];
//     packet[offset++] = xor;

//     packet[offset++] = 0x0D;

//     *packet_len = offset;

//     // 👇 打印最终数据包
//     ESP_LOG_BUFFER_HEX(TAG, packet, *packet_len);
// }

#include "mbedtls/aes.h"
#include "esp_timer.h"
#include "esp_log.h"

#define TAG "WIFI_PACKET"

// 构造 Wi-Fi 加密包
void build_wifi_packet(uint8_t *packet, size_t *packet_len, const char *ssid, const char *password)
{
    uint8_t plain[128] = {0};
    uint8_t encrypted[128] = {0};
    size_t offset = 0;
    size_t plain_len = 0;
    size_t padded_len = 0;

    // 5. 构造 AES key（基于 MAC）
    uint8_t aes_key[AES_KEY_LEN] = {
        0x32, 0x31, 0x36, 0x43, 0x41, 0x41, 0x30, 0x41, 0x46, 0x35,
        remote_device_mac[0], remote_device_mac[1], remote_device_mac[2],
        remote_device_mac[3], remote_device_mac[4], remote_device_mac[5]
    };

    // === [2] 构造明文 ===
    uint32_t timestamp = (uint32_t)(esp_timer_get_time() / 1000000ULL);  // 秒级时间戳
    plain[0] = (timestamp >> 24) & 0xFF;
    plain[1] = (timestamp >> 16) & 0xFF;
    plain[2] = (timestamp >> 8) & 0xFF;
    plain[3] = (timestamp >> 0) & 0xFF;

    int cmd_len = snprintf((char *)(plain + 4), sizeof(plain) - 4,
                           "AT+CWJAP=\"%s\",\"%s\"", ssid, password);
    plain_len = 4 + cmd_len;

    // === [3] PKCS#7 Padding ===
    uint8_t pad_len = 16 - (plain_len % 16);
    padded_len = plain_len + pad_len;
    for (int i = 0; i < pad_len; ++i) {
        plain[plain_len + i] = pad_len;
    }

    // === [4] AES-128-ECB 加密 ===
    mbedtls_aes_context aes;
    mbedtls_aes_init(&aes);
    mbedtls_aes_setkey_enc(&aes, aes_key, 128);  // 设置 128bit 密钥

    for (int i = 0; i < padded_len; i += 16) {
        mbedtls_aes_crypt_ecb(&aes, MBEDTLS_AES_ENCRYPT, plain + i, encrypted + i);
    }

    mbedtls_aes_free(&aes);

    // === [5] 构造 BLE 协议包 ===
    packet[offset++] = 0x09;                 // 包头
    packet[offset++] = 0x5F;                 // 地址
    packet[offset++] = 0x00;                 // signaling
    packet[offset++] = 0x00;                 // reserved
    packet[offset++] = (padded_len >> 8) & 0xFF;
    packet[offset++] = padded_len & 0xFF;

    memcpy(&packet[offset], encrypted, padded_len);
    offset += padded_len;

    // === [6] XOR 校验 ===
    uint8_t xor = 0;
    for (int i = 0; i < offset; i++) {
        xor ^= packet[i];
    }
    packet[offset++] = xor;

    // === [7] 添加结束符 ===
    packet[offset++] = 0x0D;

    *packet_len = offset;

    ESP_LOGI(TAG, "Build Encrypted WiFi Packet:");
    ESP_LOG_BUFFER_HEX(TAG, packet, *packet_len);
}


/* Declare static functions */
static void esp_gap_cb(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param);
static void esp_gattc_cb(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param);
static void gattc_profile_event_handler(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param);


static esp_bt_uuid_t remote_filter_service_uuid = {
    .len = ESP_UUID_LEN_16,
    .uuid = {.uuid16 = REMOTE_SERVICE_UUID,},
};

esp_bt_uuid_t remote_filter_char_uuid_write = {
    .len = ESP_UUID_LEN_16,
    .uuid = {.uuid16 = REMOTE_WRITE_CHAR_UUID,},
};

static esp_bt_uuid_t remote_filter_char_uuid = {
    .len = ESP_UUID_LEN_16,
    .uuid = {.uuid16 = REMOTE_NOTIFY_CHAR_UUID,},
};

static esp_bt_uuid_t notify_descr_uuid = {
    .len = ESP_UUID_LEN_16,
    .uuid = {.uuid16 = ESP_GATT_UUID_CHAR_CLIENT_CONFIG,},
};

static esp_ble_scan_params_t ble_scan_params = {
    .scan_type              = BLE_SCAN_TYPE_ACTIVE,
    .own_addr_type          = BLE_ADDR_TYPE_PUBLIC,
    .scan_filter_policy     = BLE_SCAN_FILTER_ALLOW_ALL,
    .scan_interval          = 0x50,
    .scan_window            = 0x30,
    .scan_duplicate         = BLE_SCAN_DUPLICATE_DISABLE
};

struct gattc_profile_inst {
    esp_gattc_cb_t gattc_cb;
    uint16_t gattc_if;
    uint16_t app_id;
    uint16_t conn_id;
    uint16_t service_start_handle;
    uint16_t service_end_handle;
    uint16_t char_handle;
    uint16_t read_handle;
    esp_bd_addr_t remote_bda;
    uint16_t notify_char_handle; // ✅ 添加这个字段
};
int text =0 ;
/* One gatt-based profile one app_id and one gattc_if, this array will store the gattc_if returned by ESP_GATTS_REG_EVT */
static struct gattc_profile_inst gl_profile_tab[PROFILE_NUM] = {
    [PROFILE_A_APP_ID] = {
        .gattc_cb = gattc_profile_event_handler,
        .gattc_if = ESP_GATT_IF_NONE,       /* Not get the gatt_if, so initial is ESP_GATT_IF_NONE */
    },
};
static void gattc_profile_event_handler(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param)
{
    esp_ble_gattc_cb_param_t *p_data = (esp_ble_gattc_cb_param_t *)param;

    switch (event) {
    case ESP_GATTC_REG_EVT:
        ESP_LOGI(GATTC_TAG, "GATT client register, status %d, app_id %d, gattc_if %d", param->reg.status, param->reg.app_id, gattc_if);
        esp_err_t scan_ret = esp_ble_gap_set_scan_params(&ble_scan_params);
        if (scan_ret){
            ESP_LOGE(GATTC_TAG, "set scan params error, error code = %x", scan_ret);
        }
        break;
    case ESP_GATTC_CONNECT_EVT:{
        ESP_LOGI(GATTC_TAG, "Connected, conn_id %d, remote "ESP_BD_ADDR_STR"", p_data->connect.conn_id,
                 ESP_BD_ADDR_HEX(p_data->connect.remote_bda));
        gl_profile_tab[PROFILE_A_APP_ID].conn_id = p_data->connect.conn_id;
        memcpy(gl_profile_tab[PROFILE_A_APP_ID].remote_bda, p_data->connect.remote_bda, sizeof(esp_bd_addr_t));
        esp_err_t mtu_ret = esp_ble_gattc_send_mtu_req (gattc_if, p_data->connect.conn_id);
        if (mtu_ret){
            ESP_LOGE(GATTC_TAG, "Config MTU error, error code = %x", mtu_ret);
        }
        break;
    }
    case ESP_GATTC_OPEN_EVT:
        if (param->open.status != ESP_GATT_OK){
            ESP_LOGE(GATTC_TAG, "Open failed, status %d", p_data->open.status);
            break;
        }
        ESP_LOGI(GATTC_TAG, "Open successfully, MTU %u", p_data->open.mtu);
        break;
    case ESP_GATTC_DIS_SRVC_CMPL_EVT:
        if (param->dis_srvc_cmpl.status != ESP_GATT_OK){
            ESP_LOGE(GATTC_TAG, "Service discover failed, status %d", param->dis_srvc_cmpl.status);
            break;
        }
        ESP_LOGI(GATTC_TAG, "Service discover complete, conn_id %d", param->dis_srvc_cmpl.conn_id);
        esp_ble_gattc_search_service(gattc_if, param->dis_srvc_cmpl.conn_id, &remote_filter_service_uuid);
        break;
    case ESP_GATTC_CFG_MTU_EVT:
        ESP_LOGI(GATTC_TAG, "MTU exchange, status %d, MTU %d", param->cfg_mtu.status, param->cfg_mtu.mtu);
        break;
    case ESP_GATTC_SEARCH_RES_EVT: {
        ESP_LOGI(GATTC_TAG, "Service search result, conn_id = %x, is primary service %d", p_data->search_res.conn_id, p_data->search_res.is_primary);
        ESP_LOGI(GATTC_TAG, "start handle %d, end handle %d, current handle value %d", p_data->search_res.start_handle, p_data->search_res.end_handle, p_data->search_res.srvc_id.inst_id);
        if (p_data->search_res.srvc_id.uuid.len == ESP_UUID_LEN_16 && p_data->search_res.srvc_id.uuid.uuid.uuid16 == REMOTE_SERVICE_UUID) {
            ESP_LOGI(GATTC_TAG, "Service found");
            get_server = true;
            gl_profile_tab[PROFILE_A_APP_ID].service_start_handle = p_data->search_res.start_handle;
            gl_profile_tab[PROFILE_A_APP_ID].service_end_handle = p_data->search_res.end_handle;
            ESP_LOGI(GATTC_TAG, "UUID16: %x", p_data->search_res.srvc_id.uuid.uuid.uuid16);
        }
        break;
    }
case ESP_GATTC_SEARCH_CMPL_EVT: {
    if (p_data->search_cmpl.status != ESP_GATT_OK) {
        ESP_LOGE(GATTC_TAG, "Service search failed, status %x", p_data->search_cmpl.status);
        break;
    }

    uint16_t count = 0;
    esp_gatt_status_t status = esp_ble_gattc_get_attr_count(
        gattc_if,
        p_data->search_cmpl.conn_id,
        ESP_GATT_DB_CHARACTERISTIC,
        gl_profile_tab[PROFILE_A_APP_ID].service_start_handle,
        gl_profile_tab[PROFILE_A_APP_ID].service_end_handle,
        INVALID_HANDLE,
        &count);
    if (status != ESP_GATT_OK || count == 0) {
        ESP_LOGE(GATTC_TAG, "Failed to get attribute count or no characteristic found");
        break;
    }

    esp_gattc_char_elem_t *char_elem_result = malloc(sizeof(esp_gattc_char_elem_t) * count);
    if (!char_elem_result) {
        ESP_LOGE(GATTC_TAG, "Malloc char_elem_result failed");
        break;
    }

    status = esp_ble_gattc_get_all_char(
        gattc_if,
        p_data->search_cmpl.conn_id,
        gl_profile_tab[PROFILE_A_APP_ID].service_start_handle,
        gl_profile_tab[PROFILE_A_APP_ID].service_end_handle,
        char_elem_result,
        &count,
        0);

    if (status != ESP_GATT_OK) {
        ESP_LOGE(GATTC_TAG, "--esp_ble_gattc_get_all_char failed");
        free(char_elem_result);
        break;
    }

    // 遍历所有特征，找 notify
    for (int i = 0; i < count; i++) {
        ESP_LOGI(GATTC_TAG, "Char[%d] UUID: 0x%04x, Handle: 0x%04x, Prop: 0x%02x",
            i,
            char_elem_result[i].uuid.uuid.uuid16,
            char_elem_result[i].char_handle,
            char_elem_result[i].properties);

        if (char_elem_result[i].properties & ESP_GATT_CHAR_PROP_BIT_NOTIFY) {
            gl_profile_tab[PROFILE_A_APP_ID].notify_char_handle = char_elem_result[i].char_handle;
            esp_ble_gattc_register_for_notify(
                gattc_if,
                gl_profile_tab[PROFILE_A_APP_ID].remote_bda,
                char_elem_result[i].char_handle);
            ESP_LOGE(TAG, "Found notify char: handle=0x%x, properties=0x%x", 
             char_elem_result[i].char_handle,
             char_elem_result[i].properties);
        }

        if (char_elem_result[i].properties & ESP_GATT_CHAR_PROP_BIT_READ) {
        ESP_LOGE(TAG, "Found read char: handle=0x%x, properties=0x%x", 
             char_elem_result[i].char_handle,
             char_elem_result[i].properties);
            // 这个char_handle就是可读特征的句柄
            gl_profile_tab[PROFILE_A_APP_ID].read_handle = char_elem_result[i].char_handle;
        }
    }
    free(char_elem_result);

    // ---- 查找写特征 ----
    esp_gattc_char_elem_t *write_char = malloc(sizeof(esp_gattc_char_elem_t));
    count = 1;
    status = esp_ble_gattc_get_char_by_uuid(
        gattc_if,
        p_data->search_cmpl.conn_id,
        gl_profile_tab[PROFILE_A_APP_ID].service_start_handle,
        gl_profile_tab[PROFILE_A_APP_ID].service_end_handle,
        remote_filter_char_uuid_write,
        write_char,
        &count);

    if (status == ESP_GATT_OK && count > 0) {
        write_char_handle = write_char[0].char_handle;
        gl_profile_tab[PROFILE_A_APP_ID].char_handle = write_char_handle;
        ESP_LOGI(GATTC_TAG, "Found write char handle: 0x%04X", write_char_handle);
    } else {
        ESP_LOGE(GATTC_TAG, "Write characteristic not found");
    }
    free(write_char);

    break;
}
    case ESP_GATTC_REG_FOR_NOTIFY_EVT: {
        if (p_data->reg_for_notify.status != ESP_GATT_OK){
            ESP_LOGE(GATTC_TAG, "Notification register failed, status %d", p_data->reg_for_notify.status);
        }else{
            ESP_LOGI(GATTC_TAG, "Notification register successfully");
            uint16_t count = 0;
            uint16_t notify_en = 1;
            esp_gatt_status_t ret_status = esp_ble_gattc_get_attr_count( gattc_if,
                                                                         gl_profile_tab[PROFILE_A_APP_ID].conn_id,
                                                                         ESP_GATT_DB_DESCRIPTOR,
                                                                         gl_profile_tab[PROFILE_A_APP_ID].service_start_handle,
                                                                         gl_profile_tab[PROFILE_A_APP_ID].service_end_handle,
                                                                         gl_profile_tab[PROFILE_A_APP_ID].notify_char_handle,
                                                                         &count);
            if (ret_status != ESP_GATT_OK){
                ESP_LOGE(GATTC_TAG, "esp_ble_gattc_get_attr_count error");
                break;
            }
            if (count > 0){
                descr_elem_result = malloc(sizeof(esp_gattc_descr_elem_t) * count);
                if (!descr_elem_result){
                    ESP_LOGE(GATTC_TAG, "malloc error, gattc no mem");
                    break;
                }else{
                    ret_status = esp_ble_gattc_get_descr_by_char_handle( gattc_if,
                                                                         gl_profile_tab[PROFILE_A_APP_ID].conn_id,
                                                                         gl_profile_tab[PROFILE_A_APP_ID].notify_char_handle,
                                                                         notify_descr_uuid,
                                                                         descr_elem_result,
                                                                         &count);
                    if (ret_status != ESP_GATT_OK){
                        ESP_LOGE(GATTC_TAG, "esp_ble_gattc_get_descr_by_char_handle error");
                        free(descr_elem_result);
                        descr_elem_result = NULL;
                        break;
                    }
                    
                    /* Every char has only one descriptor in our 'ESP_GATTS_DEMO' demo, so we used first 'descr_elem_result' */
                    if (count > 0 && descr_elem_result[0].uuid.len == ESP_UUID_LEN_16 && descr_elem_result[0].uuid.uuid.uuid16 == ESP_GATT_UUID_CHAR_CLIENT_CONFIG){
                        ESP_LOGE(TAG,"通知:");
                        ret_status = esp_ble_gattc_write_char_descr( gattc_if,
                                                                     gl_profile_tab[PROFILE_A_APP_ID].conn_id,
                                                                     descr_elem_result[0].handle,
                                                                     sizeof(notify_en),
                                                                     (uint8_t *)&notify_en,
                                                                     ESP_GATT_WRITE_TYPE_RSP,
                                                                     ESP_GATT_AUTH_REQ_NONE);
                         if (ret_status != ESP_GATT_OK){
                            ESP_LOGE(GATTC_TAG, "esp_ble_gattc_get_descr_by_char_handle error---------");
                            free(descr_elem_result);
                            descr_elem_result = NULL;
                        }
                    }
                    

                // 构造并发送握手包
                uint8_t packet[128] = {0};
                size_t packet_len = 0;
                build_hello_packet(packet, &packet_len);

                esp_err_t write_ret = esp_ble_gattc_write_char(
                    gattc_if,
                    gl_profile_tab[PROFILE_A_APP_ID].conn_id,
                    gl_profile_tab[PROFILE_A_APP_ID].char_handle, // 即 write_char_handle
                    packet_len,
                    packet,
                    ESP_GATT_WRITE_TYPE_RSP,
                    ESP_GATT_AUTH_REQ_NONE);

                    if (ret_status != ESP_GATT_OK){
                        ESP_LOGE(GATTC_TAG, "esp_ble_gattc_write_char_descr error");
                    }

                    /* free descr_elem_result */
                    free(descr_elem_result);
                }
            }
            else{
                ESP_LOGE(GATTC_TAG, "decsr not found");
            }

        }
        break;
    }
case ESP_GATTC_NOTIFY_EVT:
    if (p_data->notify.is_notify){
        ESP_LOGI(GATTC_TAG, "Notification received");
    }else{
        ESP_LOGI(GATTC_TAG, "Indication received");
    }

    ESP_LOG_BUFFER_HEX(GATTC_TAG, p_data->notify.value, p_data->notify.value_len);
    ESP_LOGI(GATTC_TAG, "获取到的通知值: %.*s", p_data->notify.value_len, p_data->notify.value);
    if(text == 0)
    {
        uint8_t packet[128] = {0};
        size_t packet_len = 0;
        build_wifi_packet(packet, &packet_len,"adol-3466","12345678");

        esp_err_t write_ret = esp_ble_gattc_write_char(
            gattc_if,
            p_data->notify.conn_id,  // ← 注意这里应该用 notify.conn_id 而不是 search_cmpl.conn_id
            write_char_handle,
            packet_len,
            packet,
            ESP_GATT_WRITE_TYPE_RSP,
            ESP_GATT_AUTH_REQ_NONE);

        if (write_ret != ESP_OK) {
            ESP_LOGE(GATTC_TAG, "Failed to write characteristic: %s", esp_err_to_name(write_ret));
        }  
        text = 1;  
    }
    
    if(text == 1)
    {
        
        uint8_t packet[128] = {0};
        size_t packet_len = 0;
        build_sn_packet(packet, &packet_len);

        esp_err_t write_ret = esp_ble_gattc_write_char(
            gattc_if,
            p_data->notify.conn_id,  // ← 注意这里应该用 notify.conn_id 而不是 search_cmpl.conn_id
            write_char_handle,
            packet_len,
            packet,
            ESP_GATT_WRITE_TYPE_RSP,
            ESP_GATT_AUTH_REQ_NONE);

        if (write_ret != ESP_OK) {
            ESP_LOGE(GATTC_TAG, "Failed to write characteristic: %s", esp_err_to_name(write_ret));
        }  
        text = 2;  
    }
    
    // if (p_data->notify.value_len >= 3 &&
    //     strncmp((char *)p_data->notify.value, "+OK", 3) == 0) {

    //     printf("Received +OK response from server!\n");

    //     uint8_t packet[128] = {0};
    //     size_t packet_len = 0;
    //     build_wifi_packet(packet, &packet_len,"adol-3466","123456789");

    //     esp_err_t write_ret = esp_ble_gattc_write_char(
    //         gattc_if,
    //         p_data->notify.conn_id,  // ← 注意这里应该用 notify.conn_id 而不是 search_cmpl.conn_id
    //         write_char_handle,
    //         packet_len,
    //         packet,
    //         ESP_GATT_WRITE_TYPE_RSP,
    //         ESP_GATT_AUTH_REQ_NONE);

    //     if (write_ret != ESP_OK) {
    //         ESP_LOGE(GATTC_TAG, "Failed to write characteristic: %s", esp_err_to_name(write_ret));
    //     }
    // }
    break;
    case ESP_GATTC_WRITE_DESCR_EVT:
        if (p_data->write.status != ESP_GATT_OK){
            ESP_LOGE(GATTC_TAG, "Descriptor write failed, status %x", p_data->write.status);
            break;
        }
        ESP_LOGI(GATTC_TAG, "Descriptor write successfully");

              
        break;
case ESP_GATTC_READ_CHAR_EVT:
    if (param->read.status == ESP_GATT_OK) {
        ESP_LOGI(GATTC_TAG, "Read value: ");

    } else {
        ESP_LOGE(GATTC_TAG, "Read failed, status = %d", param->read.status);
    }
    break;
    case ESP_GATTC_SRVC_CHG_EVT: {
        esp_bd_addr_t bda;
        memcpy(bda, p_data->srvc_chg.remote_bda, sizeof(esp_bd_addr_t));
        ESP_LOGI(GATTC_TAG, "Service change from "ESP_BD_ADDR_STR"", ESP_BD_ADDR_HEX(bda));
        break;
    }
    case ESP_GATTC_WRITE_CHAR_EVT:
        if (p_data->write.status != ESP_GATT_OK){
            ESP_LOGE(GATTC_TAG, "Characteristic write failed, status %x)", p_data->write.status);
            break;
        }
        ESP_LOGI(GATTC_TAG, "Characteristic write successfully");
        //AT+CWJAP="test","12345678"
        break;
    case ESP_GATTC_DISCONNECT_EVT:
        connect = false;
        get_server = false;
        ESP_LOGI(GATTC_TAG, "Disconnected, remote "ESP_BD_ADDR_STR", reason 0x%02x",
                 ESP_BD_ADDR_HEX(p_data->disconnect.remote_bda), p_data->disconnect.reason);
        break;
    default:
        break;
    }
}

static void esp_gap_cb(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param)
{
    uint8_t *adv_name = NULL;
    uint8_t adv_name_len = 0;
    switch (event) {
    case ESP_GAP_BLE_SCAN_PARAM_SET_COMPLETE_EVT: {
        //the unit of the duration is second
        uint32_t duration = 30;
        esp_ble_gap_start_scanning(duration);
        break;
    }
    case ESP_GAP_BLE_SCAN_START_COMPLETE_EVT:
        //scan start complete event to indicate scan start successfully or failed
        if (param->scan_start_cmpl.status != ESP_BT_STATUS_SUCCESS) {
            ESP_LOGE(GATTC_TAG, "Scanning start failed, status %x", param->scan_start_cmpl.status);
            break;
        }
        ESP_LOGI(GATTC_TAG, "Scanning start successfully");

        break;
    case ESP_GAP_BLE_SCAN_RESULT_EVT: {
        esp_ble_gap_cb_param_t *scan_result = (esp_ble_gap_cb_param_t *)param;
        switch (scan_result->scan_rst.search_evt) {
        case ESP_GAP_SEARCH_INQ_RES_EVT:
            adv_name = esp_ble_resolve_adv_data_by_type(scan_result->scan_rst.ble_adv,
                                                        scan_result->scan_rst.adv_data_len + scan_result->scan_rst.scan_rsp_len,
                                                        ESP_BLE_AD_TYPE_NAME_CMPL,
                                                        &adv_name_len);
            ESP_LOGI(GATTC_TAG, "Scan result, device "ESP_BD_ADDR_STR", name len %u", ESP_BD_ADDR_HEX(scan_result->scan_rst.bda), adv_name_len);
            ESP_LOG_BUFFER_CHAR(GATTC_TAG, adv_name, adv_name_len);

#if CONFIG_EXAMPLE_DUMP_ADV_DATA_AND_SCAN_RESP
            if (scan_result->scan_rst.adv_data_len > 0) {
                ESP_LOGI(GATTC_TAG, "adv data:");
                ESP_LOG_BUFFER_HEX(GATTC_TAG, &scan_result->scan_rst.ble_adv[0], scan_result->scan_rst.adv_data_len);
            }
            if (scan_result->scan_rst.scan_rsp_len > 0) {
                ESP_LOGI(GATTC_TAG, "scan resp:");
                ESP_LOG_BUFFER_HEX(GATTC_TAG, &scan_result->scan_rst.ble_adv[scan_result->scan_rst.adv_data_len], scan_result->scan_rst.scan_rsp_len);
            }
#endif

            if (adv_name != NULL) {
                if (strlen(remote_device_name) == adv_name_len && strncmp((char *)adv_name, remote_device_name, adv_name_len) == 0) {
                    // Note: If there are multiple devices with the same device name, the device may connect to an unintended one.
                    // It is recommended to change the default device name to ensure it is unique.
                    ESP_LOGI(GATTC_TAG, "Device found %s", remote_device_name);
                    if (connect == false) {
                        connect = true;
                        ESP_LOGI(GATTC_TAG, "Connect to the remote device");
                        esp_ble_gap_stop_scanning();
                        esp_ble_gatt_creat_conn_params_t creat_conn_params = {0};
                        memcpy(&creat_conn_params.remote_bda, scan_result->scan_rst.bda, ESP_BD_ADDR_LEN);
                        creat_conn_params.remote_addr_type = scan_result->scan_rst.ble_addr_type;
                        creat_conn_params.own_addr_type = BLE_ADDR_TYPE_PUBLIC;
                        creat_conn_params.is_direct = true;
                        creat_conn_params.is_aux = false;
                        creat_conn_params.phy_mask = 0x0;
                        esp_ble_gattc_enh_open(gl_profile_tab[PROFILE_A_APP_ID].gattc_if,
                                            &creat_conn_params);
                    }
                }
            }
            break;
        case ESP_GAP_SEARCH_INQ_CMPL_EVT:
            break;
        default:
            break;
        }
        break;
    }

    case ESP_GAP_BLE_SCAN_STOP_COMPLETE_EVT:
        if (param->scan_stop_cmpl.status != ESP_BT_STATUS_SUCCESS){
            ESP_LOGE(GATTC_TAG, "Scanning stop failed, status %x", param->scan_stop_cmpl.status);
            break;
        }
        ESP_LOGI(GATTC_TAG, "Scanning stop successfully");
        break;

    case ESP_GAP_BLE_ADV_STOP_COMPLETE_EVT:
        if (param->adv_stop_cmpl.status != ESP_BT_STATUS_SUCCESS){
            ESP_LOGE(GATTC_TAG, "Advertising stop failed, status %x", param->adv_stop_cmpl.status);
            break;
        }
        ESP_LOGI(GATTC_TAG, "Advertising stop successfully");
        break;
    case ESP_GAP_BLE_UPDATE_CONN_PARAMS_EVT:
         ESP_LOGI(GATTC_TAG, "Connection params update, status %d, conn_int %d, latency %d, timeout %d",
                  param->update_conn_params.status,
                  param->update_conn_params.conn_int,
                  param->update_conn_params.latency,
                  param->update_conn_params.timeout);
        break;
    case ESP_GAP_BLE_SET_PKT_LENGTH_COMPLETE_EVT:
        ESP_LOGI(GATTC_TAG, "Packet length update, status %d, rx %d, tx %d",
                  param->pkt_data_length_cmpl.status,
                  param->pkt_data_length_cmpl.params.rx_len,
                  param->pkt_data_length_cmpl.params.tx_len);
        break;
    default:
        break;
    }
}

static void esp_gattc_cb(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param)
{
    /* If event is register event, store the gattc_if for each profile */
    if (event == ESP_GATTC_REG_EVT) {
        if (param->reg.status == ESP_GATT_OK) {
            gl_profile_tab[param->reg.app_id].gattc_if = gattc_if;
        } else {
            ESP_LOGI(GATTC_TAG, "reg app failed, app_id %04x, status %d",
                    param->reg.app_id,
                    param->reg.status);
            return;
        }
    }

    /* If the gattc_if equal to profile A, call profile A cb handler,
     * so here call each profile's callback */
    do {
        int idx;
        for (idx = 0; idx < PROFILE_NUM; idx++) {
            if (gattc_if == ESP_GATT_IF_NONE || /* ESP_GATT_IF_NONE, not specify a certain gatt_if, need to call every profile cb function */
                    gattc_if == gl_profile_tab[idx].gattc_if) {
                if (gl_profile_tab[idx].gattc_cb) {
                    gl_profile_tab[idx].gattc_cb(event, gattc_if, param);
                }
            }
        }
    } while (0);
}

void app_main(void)
{
    // Initialize NVS.
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK( ret );

    #if CONFIG_EXAMPLE_CI_PIPELINE_ID
    memcpy(remote_device_name, esp_bluedroid_get_example_name(), sizeof(remote_device_name));
    #endif

    ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT));

    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    ret = esp_bt_controller_init(&bt_cfg);
    if (ret) {
        ESP_LOGE(GATTC_TAG, "%s initialize controller failed: %s", __func__, esp_err_to_name(ret));
        return;
    }

    ret = esp_bt_controller_enable(ESP_BT_MODE_BLE);
    if (ret) {
        ESP_LOGE(GATTC_TAG, "%s enable controller failed: %s", __func__, esp_err_to_name(ret));
        return;
    }

    ret = esp_bluedroid_init();
    if (ret) {
        ESP_LOGE(GATTC_TAG, "%s init bluetooth failed: %s", __func__, esp_err_to_name(ret));
        return;
    }

    ret = esp_bluedroid_enable();
    if (ret) {
        ESP_LOGE(GATTC_TAG, "%s enable bluetooth failed: %s", __func__, esp_err_to_name(ret));
        return;
    }

    //register the  callback function to the gap module
    ret = esp_ble_gap_register_callback(esp_gap_cb);
    if (ret){
        ESP_LOGE(GATTC_TAG, "%s gap register failed, error code = %x", __func__, ret);
        return;
    }

    //register the callback function to the gattc module
    ret = esp_ble_gattc_register_callback(esp_gattc_cb);
    if(ret){
        ESP_LOGE(GATTC_TAG, "%s gattc register failed, error code = %x", __func__, ret);
        return;
    }

    ret = esp_ble_gattc_app_register(PROFILE_A_APP_ID);
    if (ret){
        ESP_LOGE(GATTC_TAG, "%s gattc app register failed, error code = %x", __func__, ret);
    }
    esp_err_t local_mtu_ret = esp_ble_gatt_set_local_mtu(500);
    if (local_mtu_ret){
        ESP_LOGE(GATTC_TAG, "set local  MTU failed, error code = %x", local_mtu_ret);
    }

}
