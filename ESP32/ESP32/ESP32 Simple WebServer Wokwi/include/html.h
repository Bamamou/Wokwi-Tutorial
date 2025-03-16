/**
 * @file html.h
 * @brief HTML template for the web interface
 * @details Contains the webpage design with CSS styling and JavaScript
 *          for real-time LED status updates
 */

#ifndef HTML_H
#define HTML_H

/**
 * @brief HTML template with embedded CSS and JavaScript
 * @details Uses server-side processing for initial state
 *          and AJAX for real-time updates
 */
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>
<head>
  <title>ESP32 LED Control</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    html {
      font-family: 'Segoe UI', Arial, sans-serif;
      height: 100%;
      margin: 0;
    }
    body {
      min-height: 100%;
      margin: 0;
      background: linear-gradient(135deg, #1e4d92, #2c3e50);
      color: white;
      display: flex;
      flex-direction: column;
      align-items: center;
      justify-content: center;
    }
    .container {
      background: rgba(255, 255, 255, 0.1);
      backdrop-filter: blur(10px);
      border-radius: 15px;
      padding: 2rem;
      box-shadow: 0 8px 32px rgba(0, 0, 0, 0.1);
      transition: transform 0.3s ease;
    }
    .container:hover {
      transform: translateY(-5px);
    }
    h2 {
      font-size: 2.5rem;
      margin-bottom: 2rem;
      text-shadow: 2px 2px 4px rgba(0, 0, 0, 0.2);
    }
    .led-status {
      font-size: 1.5rem;
      margin: 20px;
      display: flex;
      align-items: center;
      justify-content: center;
      gap: 15px;
    }
    .led-indicator {
      width: 20px;
      height: 20px;
      border-radius: 50%;
      display: inline-block;
      transition: all 0.3s ease;
      animation: pulse 2s infinite;
    }
    @keyframes pulse {
      0% { transform: scale(1); }
      50% { transform: scale(1.1); }
      100% { transform: scale(1); }
    }
  </style>
</head>
<body>
  <div class="container">
    <h2>ESP32 LED Control</h2>
    <div class="led-status">
      Status: <span id="ledState">%STATE%</span>
      <div class="led-indicator" id="ledIndicator"></div>
    </div>
  </div>
  <script>
    function updateLED(status) {
      const indicator = document.getElementById('ledIndicator');
      const state = document.getElementById('ledState');
      indicator.style.backgroundColor = status === 'ON' ? '#22c55e' : '#666';
      indicator.style.boxShadow = status === 'ON' ? '0 0 15px #22c55e' : 'none';
      state.textContent = status;
    }

    setInterval(function() {
      fetch('/status')
        .then(response => response.text())
        .then(data => {
          updateLED(data);
        });
    }, 1000);

    // Initial LED state
    updateLED('%STATE%');
  </script>
</body>
</html>
)rawliteral";

#endif