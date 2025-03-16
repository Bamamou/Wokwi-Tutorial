#ifndef INDEX_HTML_H
#define INDEX_HTML_H

#include <Arduino.h>
float temperature = 0;
float humidity = 0;

String createHtml() {
    String response = R"(
      <!DOCTYPE html><html>
        <head>
          <title>ESP32 Temperature Monitor</title>
          <meta name="viewport" content="width=device-width, initial-scale=1">
          <style>
            html { 
              font-family: 'Arial', sans-serif;
              background: linear-gradient(135deg, #1e4d92, #2196F3);
              height: 100%;
              margin: 0;
              color: #fff;
            }
            body { 
              display: flex;
              flex-direction: column;
              align-items: center;
              justify-content: center;
              min-height: 100vh;
              margin: 0;
              padding: 20px;
            }
            h1 { 
              text-shadow: 2px 2px 4px rgba(0,0,0,0.2);
              font-size: 2.5em;
              margin-bottom: 1.2em;
              color: #ffffff;
            }
            .container {
              background: rgba(255, 255, 255, 0.1);
              backdrop-filter: blur(10px);
              border-radius: 20px;
              padding: 30px;
              box-shadow: 0 8px 32px 0 rgba(31, 38, 135, 0.37);
              border: 1px solid rgba(255, 255, 255, 0.18);
              width: 80%;
              max-width: 600px;
            }
            .readings { 
              display: flex;
              justify-content: space-around;
              flex-wrap: wrap;
              gap: 20px;
            }
            .reading-card {
              background: rgba(255, 255, 255, 0.1);
              padding: 20px;
              border-radius: 15px;
              min-width: 200px;
              text-align: center;
              backdrop-filter: blur(5px);
              transition: transform 0.3s ease;
            }
            .reading-card:hover {
              transform: translateY(-5px);
            }
            .reading-label {
              font-size: 1.2em;
              opacity: 0.9;
              margin-bottom: 10px;
            }
            .reading-value {
              font-size: 2.5em;
              font-weight: bold;
              color: #4CAF50;
              text-shadow: 0 0 10px rgba(76, 175, 80, 0.3);
            }
            .temp-value { color: #FF9800; }
            .humid-value { color: #4CAF50; }
            .servo-control {
                margin-top: 30px;
                text-align: center;
            }
            .knob {
                width: 200px;
                height: 200px;
                border-radius: 50%;
                border: 4px solid #fff;
                margin: 20px auto;
                position: relative;
                cursor: pointer;
                background: rgba(255, 255, 255, 0.1);
                backdrop-filter: blur(5px);
                transform: rotate(0deg);
                user-select: none;
            }
            .knob-dot {
                position: absolute;
                width: 10px;
                height: 10px;
                background: #fff;
                border-radius: 50%;
                top: 10px;
                left: 50%;
                transform: translateX(-50%);
            }
            #servoValue {
                font-size: 1.5em;
                color: #fff;
                margin-top: 10px;
            }
          </style>
          <script>
            let knob;
            let servoValue;
            let currentAngle = 0;
            
            function updateServo(angle) {
                fetch('/servo?value=' + angle)
                    .then(response => response.text())
                    .then(data => console.log(data));
            }

            function updateSensorValues() {
                fetch('/sensors')
                    .then(response => response.json())
                    .then(data => {
                        document.querySelector('.temp-value').innerHTML = data.temperature + '&#176;C';
                        document.querySelector('.humid-value').textContent = data.humidity + '%';
                    });
            }
            
            function rotateKnob(angle) {
                knob.style.transform = `rotate(${angle}deg)`;
                servoValue.innerHTML = Math.round(angle) + '&#176;';
                updateServo(Math.round(angle));
            }
            
            window.onload = function() {
                knob = document.querySelector('.knob');
                servoValue = document.getElementById('servoValue');
                
                function handleMove(e) {
                    if (!isDragging) return;
                    
                    e.preventDefault();
                    const rect = knob.getBoundingClientRect();
                    const centerX = rect.left + rect.width / 2;
                    const centerY = rect.top + rect.height / 2;
                    
                    const clientX = e.type.includes('touch') ? e.touches[0].clientX : e.clientX;
                    const clientY = e.type.includes('touch') ? e.touches[0].clientY : e.clientY;
                    
                    const angle = Math.atan2(clientY - centerY, clientX - centerX) * 180 / Math.PI + 90;
                    currentAngle = Math.min(180, Math.max(0, angle < 0 ? angle + 360 : angle));
                    rotateKnob(currentAngle);
                }
                
                let isDragging = false;
                
                knob.addEventListener('mousedown', (e) => {
                    isDragging = true;
                    handleMove(e);
                });
                
                knob.addEventListener('touchstart', (e) => {
                    isDragging = true;
                    handleMove(e);
                });
                
                document.addEventListener('mousemove', handleMove);
                document.addEventListener('touchmove', handleMove);
                
                document.addEventListener('mouseup', () => isDragging = false);
                document.addEventListener('touchend', () => isDragging = false);

                // Update sensor values every 2 seconds without page refresh
                setInterval(updateSensorValues, 2000);
            }
          </script>
        </head>
        <body>
          <div class="container">
            <h1>ESP32 Temperature Monitor</h1>
            <div class="readings">
              <div class="reading-card">
                <div class="reading-label">Temperature</div>
                <div class="reading-value temp-value">TEMP_VALUE&#176;C</div>
              </div>
              <div class="reading-card">
                <div class="reading-label">Humidity</div>
                <div class="reading-value humid-value">HUM_VALUE%</div>
              </div>
            </div>
            <div class="servo-control">
                <h2>Servo Control</h2>
                <div class="knob">
                    <div class="knob-dot"></div>
                </div>
                <div id="servoValue">0&#176;</div>
            </div>
          </div>
        </body>
      </html>
    )";
    response.replace("TEMP_VALUE", String(temperature, 1));
    response.replace("HUM_VALUE", String(humidity, 1));
    return response;
}

#endif // INDEX_HTML_H