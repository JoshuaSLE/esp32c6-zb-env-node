#include "presence.h"
#include "vcnl4010.h"

#include "esp_check.h"
#include "esp_log.h"
#include "esp_task.h"

static const char *TAG = "presence";

static QueueHandle_t gpio_evt_queue = NULL;
static vcnl4010_handle_t vcnl4010_handle = NULL;

static void IRAM_ATTR gpio_isr_handler(void *arg)
{
    uint32_t gpio_num = (uint32_t)arg;
    xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL);
}

static void gpio_task_example(void *arg)
{
    uint32_t io_num;
    uint8_t status = 0;

    while (true)
    {
        if (xQueueReceive(gpio_evt_queue, &io_num, portMAX_DELAY))
        {
            vcnl4010_clear_interrupt(vcnl4010_handle, &status);
            ESP_LOGI(TAG, "vcnl4010 interrupt status: %02X", status);
        }
    }
}

esp_err_t presence_init(const presence_config_t *config)
{
    gpio_config_t io_config = {
        .intr_type = GPIO_INTR_NEGEDGE,
        .mode = GPIO_MODE_DEF_INPUT,
        .pin_bit_mask = BIT64(config->gpio_num),
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en = GPIO_PULLUP_DISABLE,
    };
    ESP_RETURN_ON_ERROR(gpio_config(&io_config), TAG, "Failed to setup gpio pin");

    vcnl4010_handle = config->vcnl4010_handle;

    gpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));
    xTaskCreate(gpio_task_example, "presence", 2048, NULL, 10, NULL);

    gpio_install_isr_service(BIT64(config->gpio_num));
    gpio_isr_handler_add(config->gpio_num, gpio_isr_handler, (void *)config->gpio_num);

    return ESP_OK;
}

esp_err_t presence_deinit(void)
{

    return ESP_OK;
}