#include "i2c_bus.h"

#include "esp_check.h"
#include "driver/i2c_master.h"

static const char *TAG = "i2c_bus";

esp_err_t i2c_bus_init(i2c_master_bus_handle_t *bus_handle)
{
    if (bus_handle == NULL)
    {
        return ESP_ERR_INVALID_ARG;
    }

    i2c_master_bus_config_t i2c_mst_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = -1,
        .scl_io_num = CONFIG_APP_I2C_SCL_GPIO,
        .sda_io_num = CONFIG_APP_I2C_SDA_GPIO,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = false,
    };

    ESP_RETURN_ON_ERROR(i2c_new_master_bus(&i2c_mst_config, bus_handle),
                        TAG, "failed to create I2C bus");

    return ESP_OK;
}

esp_err_t i2c_bus_deinit(i2c_master_bus_handle_t *bus_handle)
{
    if (bus_handle == NULL || *bus_handle == NULL)
    {
        return ESP_OK;
    }

    esp_err_t err = i2c_del_master_bus(*bus_handle);
    if (err == ESP_OK)
    {
        *bus_handle = NULL;
    }

    return err;
}

esp_err_t i2c_bus_add_device(i2c_master_bus_handle_t bus_handle, bool ten_bit_addr, uint16_t dev_addr, uint16_t scl_speed_hz, i2c_master_dev_handle_t *dev_handle)
{
    i2c_device_config_t dev_config = {
        .dev_addr_length = ten_bit_addr ? I2C_ADDR_BIT_LEN_10 : I2C_ADDR_BIT_LEN_7,
        .device_address = dev_addr,
        .scl_speed_hz = scl_speed_hz,
    };

    ESP_RETURN_ON_ERROR(i2c_master_bus_add_device(bus_handle, &dev_config, dev_handle), TAG, "failed to add device to i2c bus");

    return ESP_OK;
}

esp_err_t i2c_bus_remove_device(i2c_master_dev_handle_t *dev_handle)
{
    if (dev_handle == NULL || *dev_handle == NULL)
    {
        return ESP_OK;
    }

    esp_err_t err = i2c_master_bus_rm_device(*dev_handle);
    if (err == ESP_OK)
    {
        *dev_handle = NULL;
    }

    return err;
}

esp_err_t i2c_bus_scan(i2c_master_bus_handle_t *bus_handle)
{
    if (bus_handle == NULL || *bus_handle == NULL)
    {
        return ESP_ERR_INVALID_ARG;
    }

    ESP_LOGI(TAG, "Scanning I2C bus...");
    uint8_t devices_found = 0;

    for (uint16_t addr = 0x08; addr <= 0x77; addr++)
    {
        if (i2c_master_probe(*bus_handle, addr, CONFIG_APP_I2C_TIMEOUT_MS) == ESP_OK)
        {
            ESP_LOGI(TAG, "Found device at address: 0x%02X", addr);
            devices_found++;
        }
    }

    if (devices_found == 0)
    {
        ESP_LOGW(TAG, "No I2C devices found");
    }
    else
    {
        ESP_LOGI(TAG, "Scan complete: found %d device(s)", devices_found);
    }

    return ESP_OK;
}