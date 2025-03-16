#include <stdio.h>
#include "driver/i2c_master.h"  // 使用新版 I2C 驱动
#include "esp_log.h"
#include "sdkconfig.h"
#include "ds_i2c.h"
#include "driver/i2c.h"
static i2c_master_bus_handle_t bus_handle = NULL;  // I2C 总线句柄
static i2c_master_dev_handle_t dev_handle = NULL;  // I2C 设备句柄

// 初始化 I2C 主设备
esp_err_t i2c_master_init(void) {
    i2c_master_bus_config_t bus_config = {
        .i2c_port = I2C_MASTER_NUM,          // I2C 端口号
        .sda_io_num = I2C_MASTER_SDA_IO,     // SDA GPIO 引脚
        .scl_io_num = I2C_MASTER_SCL_IO,     // SCL GPIO 引脚
        .clk_source = I2C_CLK_SRC_DEFAULT,   // 时钟源
        .glitch_ignore_cnt = 7,              // 毛刺滤波
        .flags.enable_internal_pullup = true, // 启用内部上拉
    };
    esp_err_t ret = i2c_new_master_bus(&bus_config, &bus_handle);
    if (ret != ESP_OK) {
        ESP_LOGE("I2C", "Failed to initialize I2C bus");
        return ret;
    }

    // 添加 I2C 设备
    i2c_device_config_t dev_config = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,  // 设备地址长度
        .device_address = ESP_SLAVE_ADDR,       // 设备地址
        .scl_speed_hz = I2C_MASTER_FREQ_HZ,     // SCL 时钟频率
    };
    ret = i2c_master_bus_add_device(bus_handle, &dev_config, &dev_handle);
    if (ret != ESP_OK) {
        ESP_LOGE("I2C", "Failed to add I2C device");
        return ret;
    }

    return ESP_OK;
}

// 设置读取地址
static esp_err_t i2c_master_set_addr(uint8_t u8Cmd) {
    esp_err_t ret = i2c_master_transmit(dev_handle, &u8Cmd, 1, -1);
    if (ret != ESP_OK) {
        ESP_LOGE("I2C", "Failed to set address: 0x%02X", u8Cmd);
    }
    return ret;
}

// 读取数据
esp_err_t i2c_master_read_slave(uint8_t u8Cmd, uint8_t *data_rd, size_t size) {
    if (size == 0) {
        return ESP_OK;
    }

    // 设置读取地址
    esp_err_t ret = i2c_master_set_addr(u8Cmd);
    if (ret != ESP_OK) {
        return ret;
    }

    // 读取数据
    ret = i2c_master_receive(dev_handle, data_rd, size, -1);
    if (ret != ESP_OK) {
        ESP_LOGE("I2C", "Failed to read data from slave");
    }
    return ret;
}

// 写入数据
esp_err_t i2c_master_write_slave(uint8_t u8Cmd, uint8_t *data_wr, size_t size) {
    uint8_t buffer[size + 1];
    buffer[0] = u8Cmd;  // 命令字节
    for (size_t i = 0; i < size; i++) {
        buffer[i + 1] = data_wr[i];  // 数据字节
    }

    // 写入数据
    esp_err_t ret = i2c_master_transmit(dev_handle, buffer, size + 1, -1);
    if (ret != ESP_OK) {
        ESP_LOGE("I2C", "Failed to write data to slave");
    }
    return ret;
}