# ESP32 MIDI Step Sequencer 🎵

This is a simple MIDI step sequencer built on the ESP32 microcontroller using the [ESP-IDF](https://docs.espressif.com/projects/esp-idf/en/latest/) framework.

The sequencer sends MIDI messages via UART, playing a fixed sequence of notes with configurable velocity and gate timing. It uses FreeRTOS tasks for timing and playback.

## Features

- MIDI `NOTE ON` / `NOTE OFF` message generation
- Fixed-length 8-step pattern
- Gate time and velocity per step
- Tempo-controlled playback loop using FreeRTOS
- Clean component-based project structure

## Hardware

- **ESP32 board**
- UART output connected to a standard MIDI DIN connector (with appropriate resistor & opto-isolator circuit on the receiving end)
- MIDI-compatible device (e.g. synthesizer, sound module) to test the output

## Getting Started

Make sure you have the ESP-IDF environment set up. Then clone this repo and flash it to your board:

```bash
idf.py build
idf.py -p /dev/ttyUSB0 flash monitor
```

Adjust UART pin and configuration in midi_init() as needed.

## Acknowledgments

This project is the result of a pair programming session with ChatGPT. 🤝
We explored C concepts, ESP-IDF tools, FreeRTOS, and MIDI protocols — all through interactive development and discussion.

🎶 Enjoy hacking MIDI with ESP32!