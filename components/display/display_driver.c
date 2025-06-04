
#include "lvgl.h"
#include "./../components/esp_lvgl_port/include/esp_lvgl_port.h"
#include "./../components/esp_lcd_st7796/priv_include/esp_lcd_st7796_interface.h"
#include "esp_lcd_io_spi.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_interface.h"
#include "esp_lcd_panel_dev.h"

#include <inttypes.h>


#include "esp_log.h"
#include "esp_check.h"
#include "driver/gpio.h"
#include <sys/cdefs.h>
#include <stdlib.h>

#include "esp_task_wdt.h"
#include "esp_timer.h"
#include "esp_err.h"

#define DISP_WIDTH 320
#define DISP_HEIGHT 480

#define PIN_NUM_MISO 9  // Not used
#define PIN_NUM_MOSI 10
#define PIN_NUM_CLK  8
#define PIN_NUM_CS   5
#define PIN_NUM_DC   4
#define PIN_NUM_RST  3
#define PIN_NUM_BCKL 20
static lv_disp_t *disp_handle = NULL;

void lv_example_get_started_1(void)
{
    /*Change the active screen's background color*/
    lv_obj_set_style_bg_color(lv_screen_active(), lv_color_hex(0x003a57), LV_PART_MAIN);

    /*Create a white label, set its text and align it to the center*/
    lv_obj_t * label = lv_label_create(lv_screen_active());
    lv_label_set_text(label, "Hello world");
    lv_obj_set_style_text_color(lv_screen_active(), lv_color_hex(0xffffff), LV_PART_MAIN);
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
}

esp_err_t display_spi_init(void){
    // Initialize the SPI bus
    ESP_LOGI("LVGL", "Initializing SPI bus...");
    spi_bus_config_t bus_config = {
        .mosi_io_num = PIN_NUM_MOSI,
        .miso_io_num = PIN_NUM_MISO,
        .sclk_io_num = PIN_NUM_CLK,
        .max_transfer_sz = DISP_WIDTH * DISP_HEIGHT * sizeof(uint16_t) + 8,
    };
    ESP_ERROR_CHECK(spi_bus_initialize(SPI2_HOST, &bus_config, SPI_DMA_CH_AUTO));
    ESP_LOGI("LVGL", "SPI bus initialized");
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
    ESP_LOGI("LVGL", "Panel IO created");
    return ESP_OK;
}

esp_err_t display_panel_driver_init(esp_lcd_panel_io_handle_t io_handle, esp_lcd_panel_handle_t *panel_handle){
    esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = PIN_NUM_RST,
        .rgb_endian = LCD_RGB_ENDIAN_BGR,
        .bits_per_pixel = 16,
    };
    // Create the LCD panel driver
    ESP_ERROR_CHECK(esp_lcd_new_panel_st7796_general(io_handle, &panel_config, panel_handle));
    ESP_LOGI("LVGL", "Panel driver created");

    ESP_LOGI("LVGL", "Resetting panel...");
    esp_lcd_panel_reset(*panel_handle);
    vTaskDelay(pdMS_TO_TICKS(100));
    ESP_ERROR_CHECK(esp_lcd_panel_init(*panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(*panel_handle, true));
    ESP_LOGI("LVGL", "Panel initialized and display turned on");
    return ESP_OK;

}

esp_err_t display_backlight_init(void){
    // Backlight
    gpio_config_t bk_gpio_config = {
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = 1ULL << PIN_NUM_BCKL,
        .pull_up_en = 0,
        .pull_down_en = 0,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&bk_gpio_config);
    gpio_set_level(PIN_NUM_BCKL, 1);
    ESP_LOGI("LVGL", "Backlight turned on");
    return ESP_OK;
}

esp_err_t display_lvgl_init(esp_lcd_panel_io_handle_t io_handle, esp_lcd_panel_handle_t panel_handle){
    // Initialize LVGL port
    lvgl_port_cfg_t lvgl_cfg = {
        .task_priority = 5,
        .task_stack = 8192,
        .task_affinity = -1,
        .task_max_sleep_ms = 10,
        .task_stack_caps = MALLOC_CAP_8BIT,
        .timer_period_ms = 10,
    };
    ESP_ERROR_CHECK(lvgl_port_init(&lvgl_cfg));
    ESP_LOGI("LVGL", "LVGL port initialized");

    // Create display configuration
    const lvgl_port_display_cfg_t disp_cfg = {
        .io_handle = io_handle,
        .panel_handle = panel_handle,
        .buffer_size = DISP_WIDTH * 40,
        .double_buffer = true,
        .hres = DISP_WIDTH,
        .vres = DISP_HEIGHT,
        .monochrome = false,
        .color_format = LV_COLOR_FORMAT_RGB565,
        .rotation = {
            .swap_xy = false,
            .mirror_x = true,
            .mirror_y = false,
        },
        .flags = {
            .buff_dma = true,
            .swap_bytes = false,
        }
    };
    disp_handle = lvgl_port_add_disp(&disp_cfg);
    ESP_LOGI("LVGL", "Display handle created");
    return ESP_OK;

}

esp_err_t display_hw_init(void){
    esp_lcd_panel_io_handle_t io_handle = NULL;
    esp_lcd_panel_handle_t panel_handle = NULL;

    ESP_ERROR_CHECK(display_spi_init());
    ESP_ERROR_CHECK(display_panel_init(&io_handle));
    ESP_ERROR_CHECK(display_panel_driver_init(io_handle, &panel_handle));
    // ESP_ERROR_CHECK(display_backlight_init());
    ESP_ERROR_CHECK(display_lvgl_init(io_handle, panel_handle));
    return ESP_OK;
}

