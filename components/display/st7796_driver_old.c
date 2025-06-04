// #include "./include/st7796_driver.h"
// #include "driver/gpio.h"
// #include "driver/ledc.h"
// #include "esp_log.h"
// #include <string.h> // for memset

// #define TAG "ST7796"

// // Example pin assignments (adjust to match your wiring!)
// #define PIN_NUM_MISO -1  // Not used
// #define PIN_NUM_MOSI 10
// #define PIN_NUM_CLK  8
// #define PIN_NUM_CS   5
// #define PIN_NUM_DC   4
// #define PIN_NUM_RST  3
// #define PIN_NUM_BCKL 20

// #define SPI_HOST_USED SPI2_HOST

// static spi_device_handle_t st7796_spi;

// // Basic send command helper
// static void st7796_send_cmd(uint8_t cmd) {
//     gpio_set_level(PIN_NUM_DC, 0); // Command mode
//     spi_transaction_t t = {
//         .length = 8,
//         .tx_buffer = &cmd,
//     };
//     spi_device_transmit(st7796_spi, &t);
// }

// // Basic send data helper
// static void st7796_send_data(const void *data, size_t len) {
//     if (len == 0) return;
//     gpio_set_level(PIN_NUM_DC, 1); // Data mode
//     spi_transaction_t t = {
//         .length = len * 8,
//         .tx_buffer = data,
//     };
//     spi_device_transmit(st7796_spi, &t);
// }

// static void st7796_reset(void) {
//     gpio_set_level(PIN_NUM_RST, 0);
//     vTaskDelay(pdMS_TO_TICKS(100));
//     gpio_set_level(PIN_NUM_RST, 1);
//     vTaskDelay(pdMS_TO_TICKS(100));
// }

// // Example ST7796 initialization sequence
// static void st7796_hw_init(void) {
//     st7796_send_cmd(0x11);  // Sleep Out
//     vTaskDelay(pdMS_TO_TICKS(120));

//     st7796_send_cmd(0x36);  // Memory Data Access Control
//     uint8_t data = 0x00;    // Adjust orientation as needed
//     st7796_send_data(&data, 1);

//     st7796_send_cmd(0x3A);  // Interface Pixel Format
//     data = 0x55;            // 16 bits/pixel
//     st7796_send_data(&data, 1);

//     st7796_send_cmd(0x29);  // Display ON
//     vTaskDelay(pdMS_TO_TICKS(20));
// }

// void st7796_init(void) {
//     ESP_LOGI(TAG, "Initializing ST7796 display...");

//     // GPIO setup
//     gpio_config_t io_conf = {
//         .pin_bit_mask = (1ULL << PIN_NUM_DC) | (1ULL << PIN_NUM_RST) | (1ULL << PIN_NUM_BCKL),
//         .mode = GPIO_MODE_OUTPUT,
//         .pull_up_en = 0,
//         .pull_down_en = 0,
//         .intr_type = GPIO_INTR_DISABLE,
//     };
//     gpio_config(&io_conf);

//     // SPI bus config
//     spi_bus_config_t buscfg = {
//         .miso_io_num = PIN_NUM_MISO,
//         .mosi_io_num = PIN_NUM_MOSI,
//         .sclk_io_num = PIN_NUM_CLK,
//         .quadwp_io_num = -1,
//         .quadhd_io_num = -1,
//         .max_transfer_sz = 320 * 480 * 2 + 8,
//     };
//     spi_bus_initialize(SPI_HOST_USED, &buscfg, SPI_DMA_CH_AUTO);

//     // SPI device config
//     spi_device_interface_config_t devcfg = {
//         .clock_speed_hz = 40 * 1000 * 1000, // 40MHz
//         .mode = 0,
//         .spics_io_num = PIN_NUM_CS,
//         .queue_size = 7,
//         .pre_cb = NULL,
//     };
//     spi_bus_add_device(SPI_HOST_USED, &devcfg, &st7796_spi);

//     // Hardware reset
//     st7796_reset();

//     // Init sequence
//     st7796_hw_init();

//     // Turn on backlight
//     gpio_set_level(PIN_NUM_BCKL, 1);
// }

// void st7796_set_backlight(uint8_t brightness_percent) {
//     if (brightness_percent > 100) brightness_percent = 100;
//     uint32_t duty = (1023 * brightness_percent) / 100;
//     ledc_timer_config_t ledc_timer = {
//         .speed_mode       = LEDC_LOW_SPEED_MODE,  // or LEDC_HIGH_SPEED_MODE
//         .duty_resolution  = LEDC_TIMER_13_BIT,    // 13-bit resolution: 0-8191
//         .timer_num        = LEDC_TIMER_0,         // Timer 0 (can be 0-3)
//         .freq_hz          = 5000,                 // PWM frequency (e.g. 5 kHz)
//         .clk_cfg          = LEDC_AUTO_CLK         // Auto-select the clock source
//     };
//     ledc_timer_config(&ledc_timer);
//     ledc_channel_config_t ledc_channel = {
//         .gpio_num       = PIN_NUM_BCKL,                        // Pin number (example: GPIO18)
//         .speed_mode     = LEDC_LOW_SPEED_MODE,       // Match speed mode from timer
//         .channel        = LEDC_CHANNEL_0,            // Channel 0 (can be 0-7)
//         .intr_type      = LEDC_INTR_DISABLE,         // Disable interrupts
//         .timer_sel      = LEDC_TIMER_0,              // Timer to use (match the timer)
//         .duty           = 0,                         // Initial duty (0%)
//         .hpoint         = 0                          // Use 0 unless advanced use case
//     };
//     ledc_channel_config(&ledc_channel);
//     ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, duty);
//     ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
//     ESP_LOGI(TAG, "Backlight set to %d%% (duty=%d)", (int)brightness_percent, (int)duty);
// }

// void st7796_flush(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_map) {
//     uint8_t data[4];

//     // Column Address Set
//     st7796_send_cmd(0x2A);
//     data[0] = (area->x1 >> 8) & 0xFF;
//     data[1] = area->x1 & 0xFF;
//     data[2] = (area->x2 >> 8) & 0xFF;
//     data[3] = area->x2 & 0xFF;
//     st7796_send_data(data, 4);

//     // Page Address Set
//     st7796_send_cmd(0x2B);
//     data[0] = (area->y1 >> 8) & 0xFF;
//     data[1] = area->y1 & 0xFF;
//     data[2] = (area->y2 >> 8) & 0xFF;
//     data[3] = area->y2 & 0xFF;
//     st7796_send_data(data, 4);

//     // Memory Write
//     st7796_send_cmd(0x2C);

//     // Prepare transaction
//     size_t size = (area->x2 - area->x1 + 1) * (area->y2 - area->y1 + 1) * 2;
//     spi_transaction_t t = {
//         .length = size * 8,
//         .tx_buffer = color_map,
//     };
//     gpio_set_level(PIN_NUM_DC, 1); // Data mode
//     spi_device_transmit(st7796_spi, &t);

//     // Inform LVGL that flushing is done
//     lv_disp_flush_ready(drv);
// }

// void st7796_set_address_window(const lv_area_t *area) {
//     // Column Address Set
//     uint8_t data[4];
//     st7796_send_cmd(0x2A);
//     data[0] = (area->x1 >> 8) & 0xFF;
//     data[1] = area->x1 & 0xFF;
//     data[2] = (area->x2 >> 8) & 0xFF;
//     data[3] = area->x2 & 0xFF;
//     st7796_send_data(data, 4);

//     // Page Address Set
//     st7796_send_cmd(0x2B);
//     data[0] = (area->y1 >> 8) & 0xFF;
//     data[1] = area->y1 & 0xFF;
//     data[2] = (area->y2 >> 8) & 0xFF;
//     data[3] = area->y2 & 0xFF;
//     st7796_send_data(data, 4);

//     // Memory Write
//     st7796_send_cmd(0x2C);
// }

// void fill_screen_test() {
//     lv_area_t area = {0, 0, 319, 479};
//     st7796_set_address_window(&area);

//     static uint8_t buf[320 * 2];  // One line of pixels: 320 pixels * 2 bytes each
//     for (int i = 0; i < 320; i++) {
//         buf[2*i] = 0xFF;      // low byte
//         buf[2*i + 1] = 0x00; // high byte (red)
//     }

//     for (int y = 0; y < 480; y++) {
//         st7796_send_data(buf, sizeof(buf));
//     }
// }
