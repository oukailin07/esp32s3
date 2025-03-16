/* Servo Motor control example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/mcpwm_prelude.h"  // 使用新版 MCPWM 驱动
#include "freertos/queue.h"
#include "ds_pwm.h"

static QueueHandle_t beep_queue = NULL;

#define SERVO_PULSE_GPIO        (15)   // GPIO connects to the PWM signal line

// 定义 MCPWM 对象
static mcpwm_timer_handle_t timer = NULL;
static mcpwm_oper_handle_t operator = NULL;
static mcpwm_cmpr_handle_t comparator = NULL;
static mcpwm_gen_handle_t generator = NULL;

static void ds_beep_start() {
    // 设置占空比为 50%
    ESP_ERROR_CHECK(mcpwm_comparator_set_compare_value(comparator, 50));
}

static void ds_beep_stop() {
    // 设置 PWM 信号为低电平
    ESP_ERROR_CHECK(mcpwm_generator_set_force_level(generator, 0, true));
}

static void beep_task(void* arg) {
    uint32_t evt;
    for (;;) {
        if (xQueueReceive(beep_queue, &evt, portMAX_DELAY)) {
            if (evt == BEEP_SHORT_100MS) {
                ds_beep_start();
                vTaskDelay(pdMS_TO_TICKS(100));
                ds_beep_stop();
            } else if (evt == BEEP_SHORT_500MS) {
                ds_beep_start();
                vTaskDelay(pdMS_TO_TICKS(100));
                ds_beep_stop();
                vTaskDelay(pdMS_TO_TICKS(100));
                ds_beep_start();
                vTaskDelay(pdMS_TO_TICKS(100));
                ds_beep_stop();
            } else if (evt == BEEP_LONG) {
                ds_beep_start();
                vTaskDelay(pdMS_TO_TICKS(100));
                ds_beep_stop();
                vTaskDelay(pdMS_TO_TICKS(100));
                ds_beep_start();
                vTaskDelay(pdMS_TO_TICKS(100));
                ds_beep_stop();
                vTaskDelay(pdMS_TO_TICKS(100));
                ds_beep_start();
                vTaskDelay(pdMS_TO_TICKS(100));
                ds_beep_stop();
            }
        }
    }
}

void send_beep_event(BEEP_TYPE_E type) {
    uint32_t evt = type;
    xQueueSend(beep_queue, &evt, 0);
}

void send_beep_event_from_isr(BEEP_TYPE_E type) {
    uint32_t evt = type;
    xQueueSendFromISR(beep_queue, &evt, 0);
}

void ds_pwm_init(void) {
    // 配置 MCPWM 定时器
    mcpwm_timer_config_t timer_config = {
        .group_id = 0,  // MCPWM 组 ID
        .clk_src = MCPWM_TIMER_CLK_SRC_DEFAULT,
        .resolution_hz = 1000000,  // 1MHz 分辨率
        .period_ticks = 100,        // 10kHz 频率
        .count_mode = MCPWM_TIMER_COUNT_MODE_UP,
    };
    ESP_ERROR_CHECK(mcpwm_new_timer(&timer_config, &timer));

    // 配置 MCPWM 操作器
    mcpwm_operator_config_t operator_config = {
        .group_id = 0,  // MCPWM 组 ID
    };
    ESP_ERROR_CHECK(mcpwm_new_operator(&operator_config, &operator));

    // 连接定时器和操作器
    ESP_ERROR_CHECK(mcpwm_operator_connect_timer(operator, timer));

    // 配置比较器
    mcpwm_comparator_config_t comparator_config = {
        .flags.update_cmp_on_tez = true,
    };
    ESP_ERROR_CHECK(mcpwm_new_comparator(operator, &comparator_config, &comparator));

    // 配置生成器
    mcpwm_generator_config_t generator_config = {
        .gen_gpio_num = SERVO_PULSE_GPIO,
    };
    ESP_ERROR_CHECK(mcpwm_new_generator(operator, &generator_config, &generator));

    // 设置 PWM 信号行为
    ESP_ERROR_CHECK(mcpwm_generator_set_action_on_timer_event(generator,
        MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_EMPTY, MCPWM_GEN_ACTION_HIGH)));
    ESP_ERROR_CHECK(mcpwm_generator_set_action_on_compare_event(generator,
        MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, comparator, MCPWM_GEN_ACTION_LOW)));

    // 启动定时器
    ESP_ERROR_CHECK(mcpwm_timer_enable(timer));
    ESP_ERROR_CHECK(mcpwm_timer_start_stop(timer, MCPWM_TIMER_START_NO_STOP));

    // 初始化蜂鸣器状态
    ds_beep_stop();

    // 创建队列以处理事件
    beep_queue = xQueueCreate(10, sizeof(uint32_t));

    // 启动蜂鸣器任务
    xTaskCreatePinnedToCore(beep_task, "beep_task", 1024, NULL, 10, NULL, 1);
}