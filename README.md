# MQTT Railway Signal Controller

A PlatformIO firmware project for an ESP32-based railway signal controller that drives multiple signal mast layouts over MQTT.

## Overview

This project listens for MQTT commands and updates railway signal lamps connected to PCA9685 boards. It is designed for a modular signal setup with support for standard signal states and distant-signal combinations.

## Features

- ESP32 firmware using Arduino/PlatformIO
- Wi-Fi + MQTT control
- PCA9685 PWM signal board support
- Flashing and steady LED states
- Signal status handling for main and distant signals
- Clean local configuration override for Wi‑Fi and MQTT credentials

## Hardware

- ESP32 development board
- One or more PCA9685 I2C PWM controllers
- Railway signal LEDs or LED modules on the PCA9685 outputs
- Wi‑Fi network and MQTT broker

## Repository layout

- `src/` – firmware entry points and signal logic
- `include/` – shared configuration and declarations
- `lib/` – project libraries (if added later)
- `test/` – test scaffolding
- `platformio.ini` – PlatformIO build configuration

## Setup

1. Copy the sample config file:
   ```bash
   copy include\config.local.h.example include\config.local.h
   ```
2. Edit `include/config.local.h` and fill in your local Wi‑Fi and MQTT values.
3. Build and upload:
   ```bash
   pio run -e esp32dev
   pio run -e esp32dev -t upload
   ```
4. Optionally monitor serial output:
   ```bash
   pio device monitor -e esp32dev
   ```

## MQTT topic structure

The firmware subscribes to topics under the base prefix defined in `config.h`:

```cpp
#define BASE_MQTT_TOPIC "trains/track/signalmast/"
```

Messages are expected to include the signal name and a display state such as `Sygnał S1`, `Sygnał S2`, and similar values used by the project.

## Security note

This repository intentionally does not include real credentials. Keep your personal Wi‑Fi and MQTT settings in `include/config.local.h`, which is ignored by Git.

## License

This project is licensed under the MIT License. See [LICENSE](LICENSE) for details.
