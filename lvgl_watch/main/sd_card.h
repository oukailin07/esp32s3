#ifndef SD_CARD_H
#define SD_CARD_H
#include <string.h>
#include "math.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_check.h"
#include "sdmmc_cmd.h"
#include "driver/sdmmc_host.h"
#include <dirent.h>
#include "esp_vfs_fat.h"
#define BSP_SD_CLK          (47)
#define BSP_SD_CMD          (48)
#define BSP_SD_D0           (21)

#define MOUNT_POINT              "/sdcard"
#define EXAMPLE_MAX_CHAR_SIZE    64
/******************************************************************************/
/***************************  I2C ↓ *******************************************/
#define BSP_I2C_SDA           (GPIO_NUM_1)   // SDA引脚
#define BSP_I2C_SCL           (GPIO_NUM_2)   // SCL引脚
int sdcard_filelist(const char (**file)[256]);
static esp_err_t s_example_read_file(const char *path);
void sdcard_init(void);
int sdcard_filelist_in_dir(const char (**out_filelist)[256], const char *dir_path);
#endif