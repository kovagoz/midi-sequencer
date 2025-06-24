# 🎵 Metronome Component for ESP-IDF

The `metronome` component provides a precise, timer-driven pulse generator built on top of ESP-IDF's GPTimer API. It is designed for timing-critical applications such as MIDI sequencers, audio step processing, or real-time event triggering.

## ✨ Features

- Uses GPTimer hardware for accurate timing (1 µs resolution).
- Lightweight and real-time safe (ISR-based).
- Sends task notifications to a target FreeRTOS task on each tick.
- Supports dynamically changing tempo (step duration).
- Compatible with ESP-IDF v5 and later.

## 🧩 Integration

### CMakeLists.txt

Add the `metronome` component to your project's `CMakeLists.txt`:

```cmake
set(EXTRA_COMPONENT_DIRS components/metronome)
```

If you're using `idf_component_register` in a `components` directory structure, no special handling is needed — just use `REQUIRES metronome` or `PRIV_REQUIRES metronome` where needed.

### Include in Your Code

```c
#include "metronome.h"
```

## 🚀 Usage Example

```c
static TaskHandle_t player_task_handle = NULL;

void player_task(void *arg)
{
    while (1) {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        // Do something on each tick
    }
}

void app_main(void)
{
    xTaskCreatePinnedToCore(player_task, "player", 2048, NULL, 10, &player_task_handle, 1);

    metronome_init(&player_task_handle);
    metronome_start(125); // 125ms step duration

    // Change tempo later:
    // metronome_set_period(100); // 100ms
}
```

## 📚 API Reference

### `void metronome_init(TaskHandle_t *task_handle);`

Initializes the metronome timer and binds it to the task that will receive step notifications.

### `void metronome_start(uint32_t step_duration_ms);`

Starts the metronome with the specified step duration (in milliseconds).

### `void metronome_set_period(uint32_t step_duration_ms);`

Updates the timer period. Safe to call while the metronome is running.

### `void metronome_stop(void);`

Stops the timer but does not deinitialize it.

## ⚠️ Notes

- Do not forget to set the task handle pointer to `NULL` if the task is deleted.
- Avoid using blocking calls or logging from the ISR callback.
- `metronome_init()` must be called before `metronome_start()`.

## 🛠 Tested On

- ESP32 DevKitC
- ESP-IDF v5.0+

## 📄 License

MIT License (or your project's license).
