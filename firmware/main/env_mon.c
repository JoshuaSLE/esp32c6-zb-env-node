#include "bme280.h"
#include "display.h"
#include "i2c_bus.h"
#include "vcnl4010.h"
#include "presence.h"

#include "esp_check.h"
#include "esp_log.h"
#include "esp_task.h"

#include "driver/i2c_master.h"

static const char *TAG = "env_mon";

static i2c_master_bus_handle_t i2c_bus_handle = NULL;
static esp_lcd_panel_handle_t panel_handle = NULL;
static bme280_handle_t bme280_handle = NULL;
static vcnl4010_handle_t vcnl4010_handle = NULL;

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

    vcnl4010_config_t vcnl4010_config = {
        .bus_handle = i2c_bus_handle,
        .i2c_address = CONFIG_APP_VCNL4010_I2C_ADDR,
        .scl_speed_hz = CONFIG_APP_I2C_FREQ_HZ,
        .self_timed = true,
        .prox_enabled = true,
        .als_enabled = false,
        .led_current = VCNL4010_LED_CURRENT_100mA,
        .prox_rate = VCNL4010_PROX_RATE_15_625,
        .interrupt = {
            .count = VCNL4010_INT_COUNT_1,
            .enable_threshold = true,
            .enable_als_ready = false,
            .enable_prox_ready = false,
            .low_threshold = 0,
            .high_threshold = CONFIG_APP_VCNL4010_PROX_THRESHOLD,
        },
    };
    ESP_ERROR_CHECK(vcnl4010_init(&vcnl4010_config, &vcnl4010_handle));

    presence_config_t presence_config = {
        .gpio_num = CONFIG_APP_VCNL4010_INT_GPIO,
        .vcnl4010_handle = vcnl4010_handle,
    };
    ESP_ERROR_CHECK(presence_init(&presence_config));

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
}