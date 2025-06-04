#include "./include/st7796_driver.h"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "esp_log.h"
#include <string.h> // for memset
#include "lvgl.h"

#define TAG "ST7796"

// Example pin assignments (adjust to match your wiring!)
#define PIN_NUM_MISO -1  // Not used
#define PIN_NUM_MOSI 10
#define PIN_NUM_CLK  8
#define PIN_NUM_CS   5
#define PIN_NUM_DC   4
#define PIN_NUM_RST  3
#define PIN_NUM_BCKL 20

#define SPI_HOST_USED SPI2_HOST

static spi_device_handle_t st7796_spi;

// Basic send command helper
static void st7796_send_cmd(uint8_t cmd) {
    gpio_set_level(PIN_NUM_DC, 0); // Command mode
    spi_transaction_t t = {
        .length = 8,
        .tx_buffer = &cmd,
    };
    spi_device_transmit(st7796_spi, &t);
}

// Basic send data helper
static void st7796_send_data(const void *data, size_t len) {
    if (len == 0) return;
    gpio_set_level(PIN_NUM_DC, 1); // Data mode
    spi_transaction_t t = {
        .length = len * 8,
        .tx_buffer = data,
    };
    spi_device_transmit(st7796_spi, &t);
}

static void st7796_reset(void) {
    gpio_set_level(PIN_NUM_RST, 0);
    vTaskDelay(pdMS_TO_TICKS(100));
    gpio_set_level(PIN_NUM_RST, 1);
    vTaskDelay(pdMS_TO_TICKS(100));
}

// Example ST7796 initialization sequence
static void st7796_hw_init(void) {
    st7796_send_cmd(0x11);  // Sleep Out
    vTaskDelay(pdMS_TO_TICKS(120));

    st7796_send_cmd(0x36);  // Memory Data Access Control
    uint8_t data = 0x00;    // Adjust orientation as needed
    st7796_send_data(&data, 1);

    st7796_send_cmd(0x3A);  // Interface Pixel Format
    data = 0x55;            // 16 bits/pixel
    st7796_send_data(&data, 1);

    st7796_send_cmd(0x29);  // Display ON
    vTaskDelay(pdMS_TO_TICKS(20));
}

void st7796_init(void) {
    ESP_LOGI(TAG, "Initializing ST7796 display...");

    // GPIO setup
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << PIN_NUM_DC) | (1ULL << PIN_NUM_RST) | (1ULL << PIN_NUM_BCKL),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = 0,
        .pull_down_en = 0,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&io_conf);

    // SPI bus config
    spi_bus_config_t buscfg = {
        .miso_io_num = PIN_NUM_MISO,
        .mosi_io_num = PIN_NUM_MOSI,
        .sclk_io_num = PIN_NUM_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 320 * 480 * 2 + 8,
    };
    spi_bus_initialize(SPI_HOST_USED, &buscfg, SPI_DMA_CH_AUTO);

    // SPI device config
    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = 26 * 1000 * 1000, // 40MHz
        .mode = 0,
        .spics_io_num = PIN_NUM_CS,
        .queue_size = 7,
        .pre_cb = NULL,
    };
    spi_bus_add_device(SPI_HOST_USED, &devcfg, &st7796_spi);

    // Hardware reset
    st7796_reset();

    // Init sequence
    st7796_hw_init();

    // Turn on backlight
    gpio_set_level(PIN_NUM_BCKL, 1);
}

void my_lcd_send_cmd(lv_display_t *disp, const uint8_t *cmd, size_t cmd_size, const uint8_t *param, size_t param_size)
{
    // Send the command bytes first (usually one byte)
    gpio_set_level(PIN_NUM_DC, 0);  // Command mode
    spi_transaction_t t = {
        .length = cmd_size * 8,
        .tx_buffer = cmd,
    };
    esp_err_t ret = spi_device_transmit(st7796_spi, &t);
    if (ret != ESP_OK) return ;

    // If there are parameters, send them next
    if (param && param_size > 0) {
        gpio_set_level(PIN_NUM_DC, 1);  // Data mode
        t.length = param_size * 8;
        t.tx_buffer = param;
        ret = spi_device_transmit(st7796_spi, &t);
        if (ret != ESP_OK) return;
    }

    return;
}


void my_lcd_send_color(lv_display_t *disp, const uint8_t *cmd, size_t cmd_size, uint8_t *param, size_t param_size)
{
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));  // Always zero it out!

    // Set address window (optional, as per previous guidance)
    uint8_t col_addr[] = {0x00, 0x00, 0x01, 0x3F};  // 0-319
    uint8_t row_addr[] = {0x00, 0x00, 0x01, 0xDF};  // 0-479

    st7796_send_cmd(0x2A);
    st7796_send_data(col_addr, 4);

    st7796_send_cmd(0x2B);
    st7796_send_data(row_addr, 4);

    // Send Memory Write command
    gpio_set_level(PIN_NUM_DC, 0);
    t.length = cmd_size * 8;
    t.tx_buffer = cmd;
    ESP_ERROR_CHECK(spi_device_transmit(st7796_spi, &t));

    // Send pixel data
    gpio_set_level(PIN_NUM_DC, 1);
    t.length = param_size * 8;
    t.tx_buffer = param;
    ESP_ERROR_CHECK(spi_device_transmit(st7796_spi, &t));

    // Notify LVGL that flush is done
    lv_display_flush_ready(disp);
}
