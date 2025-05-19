import mido
import serial
import threading

MIDI_PORT_NAME = 'Minilab3:Minilab3 Minilab3 MIDI 20:0'
ESP32_PORT     = '/dev/ttyUSB0'
BAUD_RATE      = 115200

try:
    esp32 = serial.Serial(ESP32_PORT, BAUD_RATE)
except serial.SerialException:
    print(f"Could not open serial port: {ESP32_PORT}")
    exit(1)

def read_from_esp32():
    while True:
        line = esp32.readline()
        if line:
            print(f"< ESP32: {line.decode(errors='replace').strip()}")

threading.Thread(target=read_from_esp32, daemon=True).start()

try:
    midi_in = mido.open_input(MIDI_PORT_NAME)
except IOError:
    print(f"Could not open MIDI port: {MIDI_PORT_NAME}")
    exit(1)

try:
    for msg in midi_in:
        data = msg.bytes()
        esp32.write(bytearray(data))
        # print(f"> Sent: {msg}")
except KeyboardInterrupt:
    print("\nStopped by user.")
finally:
    midi_in.close()
    esp32.close()
