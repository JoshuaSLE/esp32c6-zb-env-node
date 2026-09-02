#include "bme280.h"

#include "esp_check.h"
#include "esp_task.h"

static const char *TAG = "bme280";

typedef struct bme280_comp
{
    uint16_t dig_T1;
    int16_t dig_T2;
    int16_t dig_T3;
    uint16_t dig_T4;

    uint16_t dig_P1;
    int16_t dig_P2;
    int16_t dig_P3;
    int16_t dig_P4;
    int16_t dig_P5;
    int16_t dig_P6;
    int16_t dig_P7;
    int16_t dig_P8;
    int16_t dig_P9;

    uint8_t dig_H1;
    int16_t dig_H2;
    uint8_t dig_H3;
    int16_t dig_H4;
    int16_t dig_H5;
    int8_t dig_H6;
} bme280_comp_t;

struct bme280_handle
{
    i2c_master_dev_handle_t i2c_device;
    bme280_comp_t comp;
};

int32_t t_fine;

static esp_err_t bme280_reg_read(bme280_handle_t handle, uint8_t reg_addr, uint8_t *data, size_t len)
{
    return i2c_master_transmit_receive(handle->i2c_device, &reg_addr, 1, data, len, CONFIG_APP_I2C_TIMEOUT_MS);
}

static esp_err_t bme280_reg_write(bme280_handle_t handle, uint8_t reg_addr, uint8_t data)
{
    uint8_t write_buf[2] = {reg_addr, data};
    return i2c_master_transmit(handle->i2c_device, write_buf, sizeof(write_buf), CONFIG_APP_I2C_TIMEOUT_MS);
}

static esp_err_t bme280_wait_until_ready(bme280_handle_t handle, uint32_t timeout_ms)
{
    uint8_t status = 0;
    uint32_t elapsed = 0;

    while (elapsed < timeout_ms)
    {
        ESP_RETURN_ON_ERROR(bme280_reg_read(handle, BME280_REG_STATUS, &status, 1), "bme280", "failed reading status");

        if ((status & BME280_STATUS_UPDATE) == 0)
        {
            return ESP_OK;
        }

        vTaskDelay(pdMS_TO_TICKS(1));
        elapsed += 1;
    }
    return ESP_ERR_TIMEOUT;
}

static int32_t bme280_comp_temp(bme280_handle_t handle, int32_t adc_T)
{
    int32_t var1;
    int32_t var2;
    int32_t temp;
    int32_t temp_min = -4000;
    int32_t temp_max = 8500;

    var1 = (int32_t)((adc_T / 8) - ((int32_t)handle->comp.dig_T1 * 2));
    var1 = (var1 * ((int32_t)handle->comp.dig_T2)) / 2048;
    var2 = (int32_t)((adc_T / 16) - ((int32_t)handle->comp.dig_T1));
    var2 = (((var2 * var2) / 4096) * ((int32_t)handle->comp.dig_T3)) / 16384;
    t_fine = var1 + var2;
    temp = (t_fine * 5 + 128) / 256;

    if (temp < temp_min)
        temp = temp_min;
    else if (temp > temp_max)
        temp = temp_max;

    return temp;
}

static uint32_t bme280_comp_hum(bme280_handle_t handle, int32_t adc_H)
{
    int32_t var1;
    int32_t var2;
    int32_t var3;
    int32_t var4;
    int32_t var5;
    uint32_t hum;
    uint32_t hum_max = 102400;

    var1 = t_fine - ((int32_t)76800);
    var2 = (int32_t)(adc_H * 16384);
    var3 = (int32_t)(((int32_t)handle->comp.dig_H4) * 1048576);
    var4 = ((int32_t)handle->comp.dig_H5) * var1;
    var5 = (((var2 - var3) - var4) + (int32_t)16384) / 32768;
    var2 = (var1 * ((int32_t)handle->comp.dig_H6)) / 1024;
    var3 = (var1 * ((int32_t)handle->comp.dig_H3)) / 2048;
    var4 = ((var2 * (var3 + (int32_t)32768)) / 1024) + (int32_t)2097152;
    var2 = ((var4 * ((int32_t)handle->comp.dig_H2)) + 8192) / 16384;
    var3 = var5 * var2;
    var4 = ((var3 / 32768) * (var3 / 32768)) / 128;
    var5 = var3 - ((var4 * ((int32_t)handle->comp.dig_H1)) / 16);
    var5 = (var5 < 0 ? 0 : var5);
    var5 = (var5 > 419430400 ? 419430400 : var5);
    hum = (uint32_t)(var5 / 4096);

    if (hum > hum_max)
        hum = hum_max;

    return hum;
}

static uint32_t bme280_comp_press(bme280_handle_t handle, int32_t adc_P)
{
    int32_t var1, var2;
    uint32_t p;

    var1 = (((int32_t)t_fine) >> 1) - (int32_t)64000;
    var2 = (((var1 >> 2) * (var1 >> 2)) >> 11) * ((int32_t)handle->comp.dig_P6);
    var2 = var2 + ((var1 * ((int32_t)handle->comp.dig_P5)) << 1);
    var2 = (var2 >> 2) + (((int32_t)handle->comp.dig_P4) << 16);
    var1 = (((handle->comp.dig_P3 * (((var1 >> 2) * (var1 >> 2)) >> 13)) >> 3) + ((((int32_t)handle->comp.dig_P2) * var1) >> 1)) >> 18;
    var1 = ((((32768 + var1)) * ((int32_t)handle->comp.dig_P1)) >> 15);
    if (var1 == 0)
        return 0;

    p = (((uint32_t)(((int32_t)1048576) - adc_P) - (var2 >> 12))) * 3125;

    if (p < 0x80000000)
        p = (p << 1) / ((uint32_t)var1);
    else
        p = (p / (uint32_t)var1) * 2;

    var1 = (((int32_t)handle->comp.dig_P9) * ((int32_t)(((p >> 3) * (p >> 3)) >> 13))) >> 12;
    var2 = (((int32_t)(p >> 2)) * ((int32_t)handle->comp.dig_P8)) >> 13;

    p = (uint32_t)((int32_t)p + ((var1 + var2 + handle->comp.dig_P7) >> 4));
    return p;
}

esp_err_t bme280_init(const bme280_config_t *config, bme280_handle_t *ret_handle)
{
    if (config == NULL || ret_handle == NULL)
    {
        return ESP_ERR_INVALID_ARG;
    }

    bme280_handle_t dev = calloc(1, sizeof(struct bme280_handle));
    if (dev == NULL)
    {
        return ESP_ERR_NO_MEM;
    }

    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = config->i2c_address,
        .scl_speed_hz = 100000,
    };

    esp_err_t err = i2c_master_bus_add_device(config->bus_handle, &dev_cfg, &dev->i2c_device);
    if (err != ESP_OK)
    {
        free(dev);
        return err;
    }

    uint8_t chip_id = 0;
    err = bme280_reg_read(dev, BME280_REG_ID, &chip_id, 1);
    if (err != ESP_OK || chip_id != BME280_ID_VAL)
    {
        i2c_master_bus_rm_device(dev->i2c_device);
        free(dev);
        return (err != ESP_OK) ? err : ESP_ERR_NOT_FOUND;
    }

    bme280_reg_write(dev, BME280_REG_REST, BME280_SOFT_RESET_VAL);
    vTaskDelay(pdMS_TO_TICKS(10));

    err = bme280_wait_until_ready(dev, CONFIG_APP_I2C_TIMEOUT_MS);
    if (err != ESP_OK)
    {
        i2c_master_bus_rm_device(dev->i2c_device);
        free(dev);
        return err;
    }

    err = bme280_reg_write(dev, BME280_REG_CTRL_HUM, config->hum_over_sample & 0x07);

    if (err == ESP_OK)
    {
        uint8_t config_reg = ((config->standby & 0x07) << 5) | ((config->filter & 0x07) << 2);
        err = bme280_reg_write(dev, BME280_REG_CONFIG, config_reg);
    }

    if (err == ESP_OK)
    {
        uint8_t ctrl_meas = ((config->temp_over_sample & 0x07) << 5) |
                            ((config->pres_over_sample & 0x07) << 2) |
                            (config->mode & 0x03);
        err = bme280_reg_write(dev, BME280_REG_CTRL_MEAS, ctrl_meas);
    }

    if (err != ESP_OK)
    {
        i2c_master_bus_rm_device(dev->i2c_device);
        free(dev);
        return err;
    }

    *ret_handle = (bme280_handle_t)dev;
    return ESP_OK;
}

esp_err_t bme280_deinit(bme280_handle_t *handle)
{
    if (handle == NULL)
    {
        return ESP_OK;
    }

    ESP_RETURN_ON_ERROR(i2c_master_bus_rm_device((*handle)->i2c_device), TAG, "failed to remove bme280");

    *handle = NULL;

    return ESP_OK;
}