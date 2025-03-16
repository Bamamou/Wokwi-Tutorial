#pragma once

const char* HTML_TEMPLATE = R"(
<!DOCTYPE html>
<html>
<head>
    <title>ESP32 Sensor Monitor</title>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
        }
        body {
            background: linear-gradient(135deg, #1e3c72 0%, #2a5298 100%);
            min-height: 100vh;
            display: flex;
            justify-content: center;
            align-items: center;
            color: white;
        }
        .container {
            background: rgba(255, 255, 255, 0.1);
            backdrop-filter: blur(10px);
            padding: 2rem;
            border-radius: 20px;
            box-shadow: 0 8px 32px 0 rgba(31, 38, 135, 0.37);
            border: 1px solid rgba(255, 255, 255, 0.18);
            text-align: center;
        }
        h1 {
            font-size: 2.5rem;
            margin-bottom: 2rem;
            color: #ffffff;
            text-shadow: 2px 2px 4px rgba(0,0,0,0.3);
        }
        .sensor-card {
            background: rgba(255, 255, 255, 0.05);
            padding: 1.5rem;
            border-radius: 15px;
            margin: 1rem 0;
            transition: transform 0.3s ease;
        }
        .sensor-card:hover {
            transform: translateY(-5px);
        }
        .label {
            font-size: 1.2rem;
            opacity: 0.8;
            margin-bottom: 0.5rem;
        }
        .value {
            font-size: 2.5rem;
            font-weight: bold;
            color: #4dc9ff;
        }
        .unit {
            font-size: 1.2rem;
            opacity: 0.8;
        }
        .datetime {
            font-size: 1.2rem;
            margin-bottom: 1.5rem;
            color: #a0d8ef;
        }
        .weather-desc {
            margin-top: 1.5rem;
            padding: 1rem;
            background: rgba(255, 255, 255, 0.05);
            border-radius: 15px;
            font-size: 1.2rem;
            color: #4dc9ff;
        }
        @media (max-width: 480px) {
            .container {
                margin: 1rem;
                padding: 1rem;
            }
            h1 {
                font-size: 1.8rem;
            }
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>ESP32 Sensor Monitor</h1>
        <div class="datetime" id="datetime">--:--:--</div>
        <div class="sensor-card">
            <div class="label">Temperature</div>
            <div>
                <span id="temperature" class="value">--</span>
                <span class="unit">&#176;C</span>
            </div>
        </div>
        <div class="sensor-card">
            <div class="label">Humidity</div>
            <div>
                <span id="humidity" class="value">--</span>
                <span class="unit">%</span>
            </div>
        </div>
        <div class="sensor-card">
            <div class="label">Heat Index</div>
            <div>
                <span id="heatindex" class="value">--</span>
                <span class="unit">&#176;C</span>
            </div>
        </div>
        <div class="weather-desc" id="weather-desc">
            Analyzing weather conditions...
        </div>
    </div>
    <script>
        function updateDateTime() {
            const now = new Date();
            const options = { 
                weekday: 'long', 
                year: 'numeric', 
                month: 'long', 
                day: 'numeric',
                hour: '2-digit',
                minute: '2-digit',
                second: '2-digit'
            };
            document.getElementById('datetime').textContent = now.toLocaleDateString('en-US', options);
        }

        function getWeatherDescription(temp) {
            if (temp < 0) return "Freezing conditions \u2744";  // snowflake
            if (temp < 10) return "Very cold weather \u2603";   // snowman
            if (temp < 20) return "Cool temperature \u26c5";    // sun behind cloud
            if (temp < 25) return "Pleasant weather \u2600";    // sun
            if (temp < 30) return "Warm conditions \u2600\ufe0f"; // sun with variation selector
            if (temp < 35) return "Hot weather \ud83c\udf21";   // thermometer
            return "Very hot conditions \ud83d\udd25";          // fire
        }

        window.onload = function() {
            updateDateTime();
            setInterval(updateDateTime, 1000);

            const evtSource = new EventSource('/events');
            evtSource.addEventListener('sensor_data', function(e) {
                const data = JSON.parse(e.data);
                document.getElementById('temperature').textContent = data.temperature.toFixed(1);
                document.getElementById('humidity').textContent = data.humidity.toFixed(1);
                document.getElementById('heatindex').textContent = data.heatindex.toFixed(1);
                document.getElementById('weather-desc').textContent = 
                    getWeatherDescription(data.temperature);
            });
            evtSource.onerror = function(err) {
                console.error("EventSource failed:", err);
            };
        };
    </script>
</body>
</html>
)";