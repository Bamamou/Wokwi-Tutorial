# ESP32 LED Control Web Server

A modern web interface for controlling and monitoring an LED using ESP32, featuring real-time updates and an attractive UI.

## 🌟 Features

- Beautiful gradient interface with glass-morphism design
- Real-time LED status monitoring with visual indicators
- Responsive web interface that works on all devices
- Automatic status updates every second
- Animated LED status indicator
- Clean and modern typography

## 📝 Code Structure

### Main Components

1. **main.cpp**
   - Core application logic
   - WiFi and WebServer setup
   - LED control implementation
   - Request handling

2. **main.h**
   - Configuration constants
   - Function declarations
   - Pin definitions

3. **html.h**
   - Web interface template
   - CSS styling with modern design
   - JavaScript for real-time updates
   - LED status animation

## 🛠️ Technical Details

### Web Interface Features
```cpp
// LED status update function
function updateLED(status) {
  // Updates LED indicator color and shadow
  // ON  -> Green with glow effect
  // OFF -> Gray without glow
}
```

### Communication Protocol
- HTTP GET `/status` - Returns current LED state
- Polling interval: 1000ms (1 second)
- Response format: Plain text ("ON"/"OFF")

## 🔧 Hardware Setup

1. **Required Components**
   - ESP32 Development Board
   - LED (Any color)
   - 220Ω Resistor
   - Breadboard
   - Jumper wires

2. **Connections**
   - LED positive (longer leg) → Resistor → ESP32 GPIO pin
   - LED negative (shorter leg) → GND

## ⚡ Quick Start

1. **Environment Setup**
   ```bash
   # Install required libraries
   - ESP32 Board Package
   - WebServer Library
   ```

2. **Configuration**
   - Update WiFi credentials in main.cpp
   - Verify GPIO pin assignments
   - Upload code to ESP32

3. **Access Interface**
   - Connect to ESP32's IP address
   - Default: http://[ESP32-IP-ADDRESS]

## 🔍 Debugging

Common status codes:
- 200: Successful response
- 404: Endpoint not found
- 500: Server error

## 📚 Dependencies

- ESP32 Arduino Core
- WebServer Library
- WiFi Library

## 🤝 Contributing

1. Fork the repository
2. Create feature branch
3. Commit changes
4. Push to branch
5. Open pull request

## 📜 License

MIT License - Feel free to use in your projects

## 🔗 Related Documentation

- [ESP32 Official Documentation](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/)
- [WebServer Library Reference](https://github.com/espressif/arduino-esp32/tree/master/libraries/WebServer)

## 📫 Support

Create an issue in the repository for:
- Bug reports
- Feature requests
- General questions
