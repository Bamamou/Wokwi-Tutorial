# ESP32 WebServer with Counter and RGB LED Control

A web-enabled ESP32 project that features a 7-segment display counter and RGB LED control through a web interface.

## Features

- 4-digit 7-segment display counter
- RGB LED control via web interface
- Real-time counter control (start/stop)
- Async Web Server implementation
- FreeRTOS task management

## Hardware Requirements

- ESP32 development board
- TM1637 4-digit 7-segment display
- RGB LED
- Appropriate resistors for LED
- USB cable for power and programming

## Pin Configuration

- TM1637 Display:
  - CLK: GPIO22
  - DIO: GPIO21

- RGB LED:
  - Red: GPIO25
  - Green: GPIO26
  - Blue: GPIO27

## Software Dependencies

- Arduino IDE or PlatformIO
- ESP32 Arduino Core
- ESPAsyncWebServer library
- TM1637 library

## Usage

1. Connect to the ESP32's WiFi network
2. Access the web interface at the ESP32's IP address
3. Use the interface to:
   - Start/Stop the counter
   - Control RGB LED colors
   - Monitor counter values

## Project Structure

- `src/main.cpp`: Main application code
- `src/main.h`: Header file with pin definitions and configurations
- `index.html`: Web interface

## Implementation Details

- Uses FreeRTOS tasks for concurrent operations
- Implements mutex for counter protection
- Queue-based RGB LED control
- Async web server for responsive control

## Code Preview

### Counter Task Implementation
```cpp
void counterTask(void *parameter) {
  while (1) {
    xSemaphoreTake(counterMutex, portMAX_DELAY);
    if (counter == 10000) {
      counter = 0;
    }
    if (!isCounterRunning) {
      // Counter stays the same
    } else {
      counter++;
    }
    xSemaphoreGive(counterMutex);
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}
```

### RGB Control Implementation
```cpp
void handleRGB(AsyncWebServerRequest *request) {
    if(request->hasParam("r") && request->hasParam("g") && request->hasParam("b")) {
        struct rgb_values rgb;
        rgb.r = request->getParam("r")->value().toInt();
        rgb.g = request->getParam("g")->value().toInt();
        rgb.b = request->getParam("b")->value().toInt();
        
        xQueueSend(rgbQueue, &rgb, 0);
        request->send(200, "text/plain", "OK");
    }
}
```

### Display Task Implementation
```cpp
void displayTask(void *parameter) {
  while (1) {
    xSemaphoreTake(counterMutex, portMAX_DELAY);
    tm.display(0, (counter / 1000) % 10);
    tm.display(1, (counter / 100) % 10);
    tm.display(2, (counter / 10) % 10);
    tm.display(3, counter % 10);
    xSemaphoreGive(counterMutex);
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}
```

## License

This project is open source and available under the MIT License.
