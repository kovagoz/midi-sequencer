#pragma once

#include "esp_err.h"
#include <stddef.h>

esp_err_t storage_init();
esp_err_t storage_save(const char *namespace, const char *key, const void *value, size_t length);
esp_err_t storage_load(const char *namespace, const char *key, void *value, size_t *length);