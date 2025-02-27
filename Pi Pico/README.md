# Wokwi LED Blink Project

This project blinks an LED connected to digital pin D2 of an ESP32 every 500 milliseconds.

## Requirements

- PlatformIO
- VS Code
- Wokwi Simulator

## Setup

1. Clone this repository.
2. Open the project in VS Code.
3. Ensure PlatformIO is installed.

## Compiling and Uploading

1. Connect your ESP32 to your computer.
2. Click the PlatformIO: Build button in the VS Code status bar.
3. Click the PlatformIO: Upload button to upload the code to your ESP32.

## Simulation

1. Open `diagram.json` in the Wokwi Simulator.
2. Click the "Start Simulation" button.
3. Verify that the LED blinks every 500 milliseconds.

## Files

- `platformio.ini`: Project configuration file.
- `diagram.json`: Wokwi hardware definition.
- `src/main.cpp`: Arduino code for blinking the LED.
- `wokwi.toml`: Wokwi simulation configuration.
