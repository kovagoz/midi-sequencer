#pragma once

#include <stddef.h>
#include "esp_err.h"
#include "nvs.h"

#define STORAGE_NOT_FOUND ESP_ERR_NVS_NOT_FOUND

esp_err_t storage_init();
esp_err_t storage_save(const char *namespace, const char *key, const void *value, size_t length);
esp_err_t storage_load(const char *namespace, const char *key, void *value, size_t *length);