#include "lvgl.h"
#include "touch_driver.h"
#include "driver/i2c_master.h"
#include "esp_lcd_io_spi.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_interface.h"
#include "esp_lcd_panel_dev.h"
#include "esp_lcd_touch.h"
#include "esp_lcd_touch_ft5x06.h"
#include "esp_err.h"
#include "esp_log.h"

#define TOUCH_INT_GPIO -1 // No interrupt pin used
#define TOUCH_RST_GPIO GPIO_NUM_21
#define TOUCH_MAX_POINTS 1

static const char *TAG = "TOUCH";


//FT6336U Touch Driver
// Will need to share the I2C bus with the air quality sensor, share i2c_master_bus_handle_t

void lvgl_touchpad_read_cb(lv_indev_t *indev, lv_indev_data_t *data)
{
    esp_lcd_touch_handle_t tp = (esp_lcd_touch_handle_t)lv_indev_get_user_data(indev);

    uint16_t touch_x[TOUCH_MAX_POINTS];
    uint16_t touch_y[TOUCH_MAX_POINTS];
    uint16_t strength[TOUCH_MAX_POINTS];
    uint8_t touch_cnt = 0;

    tp->read_data(tp);
    if (tp->get_xy(tp, touch_x, touch_y, strength, &touch_cnt, TOUCH_MAX_POINTS) && touch_cnt > 0) {
        data->state = LV_INDEV_STATE_PRESSED;
        data->point.x = touch_x[0];      // <-- ADD THIS!
        data->point.y = touch_y[0];      // <-- ADD THIS!
        ESP_LOGI(TAG, "Touch detected at (%u, %u %u)", touch_x[0], touch_y[0], touch_cnt);
    } else {
        data->state = LV_INDEV_STATE_RELEASED;
    }
}

esp_err_t touch_i2c_init(i2c_master_bus_handle_t bus_handle)
{
    touch_hw_init(bus_handle);
    return ESP_OK;
}

esp_err_t touch_hw_init(i2c_master_bus_handle_t tp_bus_handle)
{
    // 2️⃣ Create panel IO handle
    esp_lcd_panel_io_handle_t io_handle;
    esp_lcd_panel_io_i2c_config_t io_config = {
        .scl_speed_hz = 400000, // I2C clock speed
        .dev_addr = 0x38,
        .lcd_cmd_bits = 8,
        .lcd_param_bits = 8,
        .dc_bit_offset = 0,
        .control_phase_bytes = 1,
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_i2c(tp_bus_handle,&io_config, &io_handle));
    esp_lcd_touch_handle_t touch_handle = NULL;

    esp_lcd_touch_config_t tp_cfg = {
        .x_max = 320,
        .y_max = 480,
        .rst_gpio_num = TOUCH_RST_GPIO,
        .int_gpio_num = TOUCH_INT_GPIO,
        .flags = {
            .swap_xy = 0,
            .mirror_x = 0,
            .mirror_y = 1
        },
    };

    ESP_ERROR_CHECK(esp_lcd_touch_new_i2c_ft5x06(io_handle, &tp_cfg, &touch_handle));

// Correct LVGL 9 input device creation
    lv_indev_t *indev = lv_indev_create();
    lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(indev, lvgl_touchpad_read_cb);
    lv_indev_set_user_data(indev, touch_handle);

    esp_lcd_touch_set_swap_xy(touch_handle, tp_cfg.flags.swap_xy);

    return ESP_OK;
}