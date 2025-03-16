#ifndef HTML_H
#define HTML_H
#include <Arduino.h>

const char MAIN_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<head>
    <title>7-Segment Control</title>
    <style>
        body {
            font-family: 'Arial', sans-serif;
            margin: 0;
            padding: 0;
            min-height: 100vh;
            background: linear-gradient(135deg, #1e3c72 0%, #2a5298 100%);
            color: white;
            display: flex;
            justify-content: center;
            align-items: center;
        }
        .container {
            background: rgba(255, 255, 255, 0.1);
            backdrop-filter: blur(10px);
            border-radius: 20px;
            padding: 2rem;
            box-shadow: 0 8px 32px 0 rgba(31, 38, 135, 0.37);
            border: 1px solid rgba(255, 255, 255, 0.18);
            text-align: center;
        }
        h1 {
            margin-bottom: 2rem;
            font-size: 2.5rem;
            text-shadow: 2px 2px 4px rgba(0,0,0,0.3);
        }
        .status-container {
            margin: 2rem 0;
            font-size: 1.2rem;
        }
        .ip-address {
            font-size: 0.9rem;
            opacity: 0.8;
            margin-bottom: 1rem;
        }
        .switch {
            position: relative;
            display: inline-block;
            width: 80px;
            height: 40px;
            margin: 1rem 0;
        }
        .switch input {
            opacity: 0;
            width: 0;
            height: 0;
        }
        .slider {
            position: absolute;
            cursor: pointer;
            top: 0;
            left: 0;
            right: 0;
            bottom: 0;
            background-color: rgba(255,255,255,0.2);
            transition: .4s;
            border-radius: 40px;
        }
        .slider:before {
            position: absolute;
            content: "";
            height: 32px;
            width: 32px;
            left: 4px;
            bottom: 4px;
            background-color: white;
            transition: .4s;
            border-radius: 50%;
            box-shadow: 0 2px 4px rgba(0,0,0,0.2);
        }
        input:checked + .slider {
            background-color: #4CAF50;
        }
        input:checked + .slider:before {
            transform: translateX(40px);
        }
        #status {
            font-weight: bold;
            color: #4CAF50;
        }
        .rgb-container {
            margin: 2rem 0;
            padding: 1rem;
            background: rgba(0,0,0,0.2);
            border-radius: 10px;
        }
        .rgb-slider {
            width: 100%;
            margin: 1rem 0;
        }
        .slider-label {
            display: flex;
            justify-content: space-between;
            align-items: center;
            margin-bottom: 0.5rem;
        }
        input[type="range"] {
            -webkit-appearance: none;
            width: 100%;
            height: 10px;
            border-radius: 5px;
            background: rgba(255,255,255,0.1);
            outline: none;
        }
        input[type="range"]::-webkit-slider-thumb {
            -webkit-appearance: none;
            width: 20px;
            height: 20px;
            border-radius: 50%;
            cursor: pointer;
        }
        #redSlider::-webkit-slider-thumb {
            background: #ff4444;
        }
        #greenSlider::-webkit-slider-thumb {
            background: #4CAF50;
        }
        #blueSlider::-webkit-slider-thumb {
            background: #2196F3;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>7-Segment Display</h1>
        <div class="ip-address">
            Server IP: <span id="ipAddress"></span>
        </div>
        <label class="switch">
            <input type="checkbox" id="toggleSwitch" onchange="toggleCounter(this)">
            <span class="slider"></span>
        </label>
        <div class="status-container">
            Status: <span id="status">Stopped</span>
        </div>
        <div class="rgb-container">
            <h2>RGB LED Control</h2>
            <div class="slider-label">
                <span>Red</span>
                <span id="redValue">0</span>
            </div>
            <input type="range" min="0" max="255" value="0" class="rgb-slider" id="redSlider" onchange="updateRGB()" oninput="updateValue(this, 'redValue')">
            
            <div class="slider-label">
                <span>Green</span>
                <span id="greenValue">0</span>
            </div>
            <input type="range" min="0" max="255" value="0" class="rgb-slider" id="greenSlider" onchange="updateRGB()" oninput="updateValue(this, 'greenValue')">
            
            <div class="slider-label">
                <span>Blue</span>
                <span id="blueValue">0</span>
            </div>
            <input type="range" min="0" max="255" value="0" class="rgb-slider" id="blueSlider" onchange="updateRGB()" oninput="updateValue(this, 'blueValue')">
        </div>
    </div>
    <script>
        document.getElementById('ipAddress').innerText = window.location.host;
        function toggleCounter(element) {
            fetch('/toggle?state=' + (element.checked ? '1' : '0'))
            .then(response => response.text())
            .then(data => {
                document.getElementById('status').innerText = 
                    element.checked ? 'Running' : 'Stopped';
                document.getElementById('status').style.color = 
                    element.checked ? '#4CAF50' : '#ff4444';
            });
        }
        
        let rgbTimeout;
        
        function updateValue(slider, valueId) {
            document.getElementById(valueId).innerText = slider.value;
        }

        function updateRGB() {
            clearTimeout(rgbTimeout);
            rgbTimeout = setTimeout(() => {
                const r = document.getElementById('redSlider').value;
                const g = document.getElementById('greenSlider').value;
                const b = document.getElementById('blueSlider').value;
                
                document.getElementById('redValue').innerText = r;
                document.getElementById('greenValue').innerText = g;
                document.getElementById('blueValue').innerText = b;
                
                fetch(`/rgb?r=${r}&g=${g}&b=${b}`);
            }, 50); // 50ms delay for debounce
        }
    </script>
</body>
</html>
)=====";

#endif
