#include "tpic6b595.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_err.h"
#include "esp_log.h"

static const char *TAG = "tpic6b595";

static void pulse_gpio(gpio_num_t gpio) {
    gpio_set_level(gpio, 1);
    esp_rom_delay_us(1);
    gpio_set_level(gpio, 0);
    esp_rom_delay_us(1);
}

esp_err_t tpic6b595_init(const tpic6b595_t *dev) {
    if (!dev) {
        return ESP_ERR_INVALID_ARG;
    }

    gpio_config_t io_conf = {
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = (1ULL << dev->srck) | (1ULL << dev->rck) | (1ULL << dev->ser),
        .intr_type = GPIO_INTR_DISABLE
    };

    esp_err_t err = gpio_config(&io_conf);

    if (err != ESP_OK) {
        ESP_LOGE(TAG, "GPIO config failed");
        return err;
    }

    gpio_set_level(dev->srck, 0);
    gpio_set_level(dev->rck, 0);
    gpio_set_level(dev->ser, 0);

    return ESP_OK;
}

void tpic6b595_write(const tpic6b595_t *dev, uint8_t data) {
    if (!dev) {
        return;
    }

    for (int i = 7; i >= 0; i--) {
        gpio_set_level(dev->ser, (data >> i) & 0x01);
        pulse_gpio(dev->srck);
    }

    pulse_gpio(dev->rck);
}
