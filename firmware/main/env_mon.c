#include "i2c_bus.h"
#include "display.h"

#include "esp_check.h"
#include "esp_log.h"
#include "esp_task.h"

static const char *TAG = "env_mon";

static i2c_master_bus_handle_t i2c_bus_handle = NULL;
static esp_lcd_panel_handle_t panel_handle = NULL;

void app_main(void)
{
    ESP_LOGI(TAG, "App main starting...");

    ESP_ERROR_CHECK(i2c_bus_init(&i2c_bus_handle));
    ESP_ERROR_CHECK(display_init(i2c_bus_handle, &panel_handle));

    float temp_c = 21.5f;
    float humidity_pct = 45.0f;
    float pressure_hpa = 1013.0f;

    while (1)
    {
        ESP_LOGI(TAG, "Showing test values: %.1fC %.0f%% %.0fhPa",
                 temp_c, humidity_pct, pressure_hpa);
        ESP_ERROR_CHECK(display_off_on(panel_handle, true));
        ESP_ERROR_CHECK(display_show_readings(panel_handle, temp_c, humidity_pct, pressure_hpa));

        vTaskDelay(pdMS_TO_TICKS(3000));

        temp_c += 1.0f;
        humidity_pct += 2.0f;
        pressure_hpa += 1.0f;

        ESP_LOGI(TAG, "Turning display off for 2s...");
        ESP_ERROR_CHECK(display_off_on(panel_handle, false));
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}