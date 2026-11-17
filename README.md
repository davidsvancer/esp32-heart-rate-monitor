# ESP32 Heart Rate Monitor

A heart rate monitoring project built with **ESP32 and ESP-IDF in C**. The application reads an analog pulse sensor, processes the signal to detect heartbeats, calculates the heart rate (BPM), and displays the results on an OLED screen.

The project focuses on embedded programming, hardware interfacing, signal processing, and real-time application development.

## Features

- Analog pulse sensor data acquisition using the ESP32 ADC
- Digital low-pass filtering to reduce signal noise
- Heartbeat detection using a dynamic threshold and hysteresis
- BPM calculation based on the time interval between detected heartbeats
- Median-based smoothing to stabilize the displayed heart rate
- Real-time visualization of the signal and BPM on an OLED display
- Button-based user interaction

## Hardware

The project uses the following components:

- ESP32 development board
- Analog pulse sensor
- SSD1306-compatible OLED display (128 × 64)
- Push button
- Breadboard and connecting wires

### Pin Configuration

| Component | ESP32 GPIO | Description |
|---|---:|---|
| Pulse sensor output | GPIO 34 | Analog input (ADC) |
| OLED SDA | GPIO 21 | I²C data |
| OLED SCL | GPIO 22 | I²C clock |
| Push button | GPIO 18 | User input |

The OLED display uses the I²C address `0x3C`.

## Signal Processing

The application processes the incoming analog signal before attempting to detect heartbeats.

The processing pipeline consists of:

1. **Signal acquisition** – The ESP32 ADC periodically samples the analog output of the pulse sensor.
2. **Low-pass filtering** – A simple digital filter reduces rapid fluctuations and high-frequency noise.
3. **Heartbeat detection** – A dynamic threshold with hysteresis is used to identify individual pulse peaks while reducing repeated detections.
4. **BPM calculation** – The time interval between consecutive detected heartbeats is converted into beats per minute.
5. **Median smoothing** – A circular buffer and median calculation help reduce sudden fluctuations in the displayed BPM value.

The application also maintains a rolling signal buffer, which is used to visualize the pulse waveform on the OLED display.

## Display

The OLED interface provides real-time feedback, including:

- Calculated heart rate in BPM
- Visualization of the incoming pulse signal
- Graph scaling based on the observed signal values

The display is controlled through I²C communication.

## Software and Technologies

- **Language:** C
- **Platform:** ESP32
- **Framework:** ESP-IDF
- **RTOS:** FreeRTOS (provided by ESP-IDF)
- **Peripherals:** ADC, GPIO, I²C
- **Display:** SSD1306-compatible OLED

## Building and Running

### Requirements

- ESP-IDF development framework
- ESP32 development board
- USB connection for flashing and serial monitoring

### Build

Open the project directory in an ESP-IDF environment and configure the target:

```bash
idf.py set-target esp32
```

Build the project:

```bash
idf.py build
```

### Flash

Connect the ESP32 board and flash the firmware:

```bash
idf.py -p PORT flash
```

Replace `PORT` with the serial port assigned to your ESP32.

### Monitor

To view serial output:

```bash
idf.py -p PORT monitor
```

You can also flash the firmware and start the serial monitor in one step:

```bash
idf.py -p PORT flash monitor
```

## Project Structure

The project is organized into separate source and configuration files. The exact structure may depend on the ESP-IDF project configuration.

Typical ESP-IDF project files include:

- `main/` – Application source code
- `CMakeLists.txt` – Build configuration
- `sdkconfig` – ESP-IDF project configuration

## Limitations

- Heart rate measurements depend on sensor placement, movement, and signal quality.
- Motion artifacts and electrical noise can affect heartbeat detection.

## Learning Outcomes

Through this project, I gained practical experience with:

- Embedded C programming on the ESP32 platform
- Working with ADC, GPIO, and I²C peripherals
- Analog sensor data acquisition
- Digital signal filtering and basic signal processing
- Heartbeat detection and BPM calculation
- Real-time data visualization on an OLED display
- Developing and debugging applications using ESP-IDF

---

**Note:** This project was developed as part of university coursework.