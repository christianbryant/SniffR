
#include "lvgl.h"
#include "./../components/esp_lvgl_port/include/esp_lvgl_port.h"
#include "./../components/esp_lcd_st7796/priv_include/esp_lcd_st7796_interface.h"
#include "esp_lcd_io_spi.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_interface.h"
#include "esp_lcd_panel_dev.h"

#include "touch_driver.h"
#include "nvs_driver.h"

#include <inttypes.h>


#include "esp_log.h"
#include "esp_check.h"
#include "driver/gpio.h"
#include <sys/cdefs.h>
#include <stdlib.h>

#include "esp_task_wdt.h"
#include "esp_timer.h"
#include "esp_err.h"
#include "driver/ledc.h"

#define DISP_WIDTH 320
#define DISP_HEIGHT 480

#define PIN_NUM_MISO 9  // Not used
#define PIN_NUM_MOSI 10
#define PIN_NUM_CLK  8
#define PIN_NUM_CS   5
#define PIN_NUM_DC   4
#define PIN_NUM_RST  3
static lv_disp_t *disp_handle = NULL;

//brightness control defines
#define PIN_NUM_BCKL 20
#define LEDC_TIMER     LEDC_TIMER_0
#define LEDC_MODE      LEDC_LOW_SPEED_MODE
#define LEDC_CHANNEL   LEDC_CHANNEL_0
#define LEDC_DUTY_RES  LEDC_TIMER_13_BIT
#define LEDC_FREQUENCY 5000

static const char* TAG = "display_driver";

esp_err_t display_spi_init(void){
    int32_t debug;
    load_user_setting("debug", &debug, 0);
    // Initialize the SPI bus
    spi_bus_config_t bus_config = {
        .mosi_io_num = PIN_NUM_MOSI,
        .miso_io_num = PIN_NUM_MISO,
        .sclk_io_num = PIN_NUM_CLK,
        .max_transfer_sz = 4092,
    };
    ESP_ERROR_CHECK(spi_bus_initialize(SPI2_HOST, &bus_config, SPI_DMA_CH_AUTO));
    if(debug == 1){
        ESP_LOGI(TAG, "SPI bus initialized");
    }
    return ESP_OK;

}

esp_err_t display_panel_init(esp_lcd_panel_io_handle_t *io_handle){
    // Initialize the LCD panel
    esp_lcd_panel_io_spi_config_t io_config = {
        .cs_gpio_num = PIN_NUM_CS,
        .dc_gpio_num = PIN_NUM_DC,
        .spi_mode = 0,
        .pclk_hz = 40 * 1000 * 1000,
        .trans_queue_depth = 10,
        .on_color_trans_done = NULL,
        .user_ctx = NULL,
        .lcd_cmd_bits = 8,
        .lcd_param_bits = 8,
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)SPI2_HOST, &io_config, io_handle));
    int32_t debug;
    load_user_setting("debug", &debug, 0);
    if(debug == 1){
        ESP_LOGI(TAG, "Panel IO created");
    }
    return ESP_OK;
}

esp_err_t display_panel_driver_init(esp_lcd_panel_io_handle_t io_handle, esp_lcd_panel_handle_t *panel_handle){
    esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = PIN_NUM_RST,
        .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB,
        .data_endian = LCD_RGB_DATA_ENDIAN_BIG, // Use big endian for RGB data
        .bits_per_pixel = 24,
    };
    // Create the LCD panel driver
    int32_t debug;
    load_user_setting("debug", &debug, 0);
    ESP_ERROR_CHECK(esp_lcd_new_panel_st7796_general(io_handle, &panel_config, panel_handle));
    if(debug == 1){
        ESP_LOGI(TAG, "Panel driver created");
        ESP_LOGI(TAG, "Resetting panel...");
    }
    esp_lcd_panel_reset(*panel_handle);
    vTaskDelay(pdMS_TO_TICKS(100));
    ESP_ERROR_CHECK(esp_lcd_panel_init(*panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(*panel_handle, true));
    if(debug == 1){
        ESP_LOGI(TAG, "Panel initialized and display turned on");
    }
    return ESP_OK;

}

esp_err_t display_backlight_init()
{
    int32_t debug;
    load_user_setting("debug", &debug, 0);
    // Timer configuration
    ledc_timer_config_t ledc_timer = {
        .speed_mode       = LEDC_MODE,
        .timer_num        = LEDC_TIMER,
        .duty_resolution  = LEDC_DUTY_RES,
        .freq_hz          = LEDC_FREQUENCY,
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

    // Channel configuration
    ledc_channel_config_t ledc_channel = {
        .gpio_num       = PIN_NUM_BCKL,
        .speed_mode     = LEDC_MODE,
        .channel        = LEDC_CHANNEL,
        .timer_sel      = LEDC_TIMER,
        .duty           = 0,
        .hpoint         = 0,
        .flags.output_invert = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));
    if(debug == 1){
        ESP_LOGI(TAG, "Display Brightness control initiated");
    }


    return ESP_OK;
}

esp_err_t display_set_brightness(int percent)
{
    if (percent > 100){
        percent = 100;
    } else if (percent <= 0){
        percent = 5;
    }

    uint32_t max_duty = (1 << LEDC_DUTY_RES) - 1;
    uint32_t duty = (percent * max_duty) / 100;

    return ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, duty) ||
           ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);
}

esp_err_t display_lvgl_init(esp_lcd_panel_io_handle_t io_handle, esp_lcd_panel_handle_t panel_handle){
    // Initialize LVGL port
    lvgl_port_cfg_t lvgl_cfg = {
        .task_priority = 5,
        .task_stack = 8192,
        .task_affinity = -1,
        .task_max_sleep_ms = 10,
        .task_stack_caps = MALLOC_CAP_DEFAULT,
        .timer_period_ms = 10,
    };
    ESP_ERROR_CHECK(lvgl_port_init(&lvgl_cfg));
    int32_t debug;
    load_user_setting("debug", &debug, 0);
    if(debug == 1){
        ESP_LOGI(TAG, "LVGL port initialized");
    }

    // Create display configuration
    const lvgl_port_display_cfg_t disp_cfg = {
        .io_handle = io_handle,
        .panel_handle = panel_handle,
        .buffer_size = (DISP_WIDTH * DISP_HEIGHT) / 10,
        .double_buffer = true,
        .hres = DISP_WIDTH,
        .vres = DISP_HEIGHT,
        .monochrome = false,
        .color_format = LV_COLOR_FORMAT_RGB888,
        .rotation = {
            .swap_xy = false,
            .mirror_x = true,
            .mirror_y = false,
        },
        .flags = {
            .buff_dma = false,
            .swap_bytes = false,
        }
    };
    disp_handle = lvgl_port_add_disp(&disp_cfg);
    if(debug == 1){
        ESP_LOGI(TAG, "Display handle created");
    }
    return ESP_OK;

}

esp_err_t display_hw_init(void){
    esp_lcd_panel_io_handle_t io_handle = NULL;
    esp_lcd_panel_handle_t panel_handle = NULL;

    ESP_ERROR_CHECK(display_spi_init());
    ESP_ERROR_CHECK(display_panel_init(&io_handle));
    ESP_ERROR_CHECK(display_panel_driver_init(io_handle, &panel_handle));
    panel_handle->invert_color(panel_handle, true);
    ESP_ERROR_CHECK(display_backlight_init());
    ESP_ERROR_CHECK(display_lvgl_init(io_handle, panel_handle));
    int32_t brightness;
    load_user_setting("brightness", &brightness, 100);
    ESP_ERROR_CHECK(display_set_brightness((int)brightness));
    int32_t debug;
    load_user_setting("debug", &debug, 0);
    if(debug == 1){
        ESP_LOGI(TAG, "Display hardware initialized");

    }
    return ESP_OK;
}

