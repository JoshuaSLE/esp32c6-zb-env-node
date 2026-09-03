#include "bme280.h"
#include "display.h"
#include "i2c_bus.h"

#include "esp_check.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"

static const char *TAG = "env_mon";

static i2c_master_bus_handle_t i2c_bus_handle = NULL;
static esp_lcd_panel_handle_t panel_handle = NULL;
static bme280_handle_t bme280_handle = NULL;

void app_main(void)
{
    ESP_LOGI(TAG, "App main starting...");

    ESP_ERROR_CHECK(i2c_bus_init(&i2c_bus_handle));
    ESP_ERROR_CHECK(display_init(i2c_bus_handle, &panel_handle));

    bme280_config_t bme280_config = {
        .bus_handle = i2c_bus_handle,
        .i2c_address = CONFIG_APP_BME280_I2C_ADDR,
        .scl_speed_hz = CONFIG_APP_I2C_FREQ_HZ,
        .mode = BME280_MODE_FORCED,
        .temp_over_sample = BME280_OVER_SAMPLE_1X,
        .hum_over_sample = BME280_OVER_SAMPLE_1X,
        .pres_over_sample = BME280_OVER_SAMPLE_1X,
        .filter = BME280_FILTER_OFF,
    };
    ESP_ERROR_CHECK(bme280_init(&bme280_config, &bme280_handle));

    bme280_data_t reading = {0};
    while (1)
    {
        ESP_ERROR_CHECK(bme280_trigger_measurement(bme280_handle));
        ESP_ERROR_CHECK(bme280_read_data(bme280_handle, &reading));

        ESP_LOGI(TAG, "Read: %.1fC %.0f%%RH %.0fhPa",
                 reading.temp, reading.hum, reading.press);

        ESP_ERROR_CHECK(display_off_on(panel_handle, true));
        ESP_ERROR_CHECK(display_show_readings(panel_handle, reading.temp, reading.hum, reading.press));

        vTaskDelay(pdMS_TO_TICKS(10000));

        ESP_ERROR_CHECK(display_off_on(panel_handle, false));
        vTaskDelay(pdMS_TO_TICKS(50000));
    }

    ESP_ERROR_CHECK(display_deinit(&panel_handle));
    ESP_ERROR_CHECK(bme280_deinit(&bme280_handle));
    ESP_ERROR_CHECK(i2c_bus_deinit(&i2c_bus_handle));

    ESP_LOGI(TAG, "App main ending...");
}