#include "esp_err.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "storage.h"

esp_err_t storage_init()
{
    return nvs_flash_init();
}

esp_err_t storage_save(const char *namespace, const char *key, const void *value, size_t length)
{
    if (!namespace || !key || !value) {
        return ESP_ERR_INVALID_ARG;
    }

    nvs_handle_t handle;
    esp_err_t err = nvs_open(namespace, NVS_READWRITE, &handle);

    if (err != ESP_OK) {
        return err;
    }

    err = nvs_set_blob(handle, key, value, length);

    if (err == ESP_OK) {
        err = nvs_commit(handle);
    }

    nvs_close(handle);

    return err;
}

esp_err_t storage_load(const char *namespace, const char *key, void *value, size_t *length)
{
    if (!namespace || !key || !value || !length) {
        return ESP_ERR_INVALID_ARG;
    }

    nvs_handle_t handle;
    esp_err_t err = nvs_open(namespace, NVS_READONLY, &handle);

    if (err != ESP_OK) {
        return err;
    }

    // First get actual size
    size_t required_size = 0;
    err = nvs_get_blob(handle, key, NULL, &required_size);

    if (err != ESP_OK) {
        nvs_close(handle);
        return err;
    }

    // If the given size is not enough to store the data
    if (*length < required_size) {
        *length = required_size;
        nvs_close(handle);
        return ESP_ERR_NVS_VALUE_TOO_LONG;
    }

    err = nvs_get_blob(handle, key, value, &required_size);

    if (err == ESP_OK) {
        *length = required_size;
    }

    nvs_close(handle);

    return err;
}