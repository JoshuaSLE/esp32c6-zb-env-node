#include "esp_log.h"
#include "esp_task.h"

static const char *TAG = "env_mon";

void app_main(void)
{
    ESP_LOGI(TAG, "App main starting...");

    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    ESP_LOGI(TAG, "App main ending...");
}
