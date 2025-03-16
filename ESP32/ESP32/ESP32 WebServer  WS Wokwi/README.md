# ESP32 WebSocket WebServer Project

## Overview
This project implements a WebSocket server on an ESP32 microcontroller that provides real-time communication between the device and web clients. It features LED control and sensor data monitoring capabilities.

## Features
- WebSocket server implementation
- Real-time LED control
- Dynamic web interface
- Status monitoring and display
- Automatic client reconnection

## Requirements
- ESP32 development board
- Arduino IDE or PlatformIO
- Required libraries:
  - ESPAsyncWebServer
  - AsyncTCP
  - WiFi

## Setup Instructions
1. Connect your ESP32 to your development environment
2. Install required libraries
3. Update WiFi credentials in the code
4. Upload the code to your ESP32
5. Access the web interface through your browser

## Project Structure
- `main.cpp` - Main application code
- `main.h` - Header definitions and configurations
- `index_html.h` - Web interface HTML content

## Code Preview
```cpp
// Main configuration
const char* ssid = "Your_SSID";
const char* password = "Your_Password";

// WebSocket server setup
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

// LED control function example
void toggleLED(bool state) {
  digitalWrite(LED_BUILTIN, state);
}

// WebSocket event handler
void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, 
               AwsEventType type, void *arg, uint8_t *data, size_t len) {
  // Handle WebSocket events
}
```

## Usage
1. Power up the ESP32
2. Connect to your WiFi network
3. Access the web interface using ESP32's IP address
4. Use the interface to control LEDs and monitor status

## Note
This project is designed for educational and development purposes. Ensure proper safety measures when implementing in real-world applications.
