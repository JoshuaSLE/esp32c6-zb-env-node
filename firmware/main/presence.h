#pragma once

#include "vcnl4010.h"

#include "esp_err.h"
#include "driver/gpio.h"

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct presence_config
    {
        gpio_num_t gpio_num;
        vcnl4010_handle_t vcnl4010_handle;
    } presence_config_t;

    esp_err_t presence_init(const presence_config_t *config);

    esp_err_t presence_deinit(void);

#ifdef __cplusplus
}
#endif