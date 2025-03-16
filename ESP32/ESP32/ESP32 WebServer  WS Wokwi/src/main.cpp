/*
 * ESP32 Temperature and Humidity Monitor
 * 
 * This project reads temperature and humidity from a DHT sensor
 * and displays the values on an I2C LCD display using FreeRTOS tasks.
 * 
 * Hardware Connections:
 * - DHT Sensor: GPIO4
 * - Servo Motor: GPIO12
 * - LCD I2C: 
 *   - SDA: GPIO21
 *   - SCL: GPIO22
 * 
 * Author: Your Name
 * Date: Current Date
 */

#include "main.h"


// Global objects initialization
DHT dht(DHT_PIN, DHT_TYPE);
LiquidCrystal_I2C lcd(0x27, 16, 4);
AsyncWebServer server(80);  // Changed to match test.cpp approach
AsyncWebSocket ws("/ws"); // WebSocket server instance
Servo myservo;

SemaphoreHandle_t xMutex = NULL;




// Task handles
TaskHandle_t TaskSensor_Handle = NULL;
TaskHandle_t TaskDisplay_Handle = NULL;
TaskHandle_t TaskWebSocket_Handle = NULL;

void TaskDisplay(void *pvParameters) {
    static float lastTemp = -999;  // Initialize with invalid value
    static float lastHum = -999;   // Initialize with invalid value
    static int lastServoPos = -1;
    
    // Initial display setup
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Temp: ");
    lcd.setCursor(0, 1);
    lcd.print("Humidity: ");
    lcd.setCursor(0, 2);
    lcd.print("Servo: ");
    
    while(1) {
        if(xSemaphoreTake(xMutex, portMAX_DELAY)) {
            // Only update if values changed
            if (abs(lastTemp - temperature) >= 0.1) {  // Update if change >= 0.1
                lcd.setCursor(10, 0);
                lcd.print("     ");
                lcd.setCursor(10, 0);
                lcd.print(temperature, 1);
                lcd.write(0xDF);  // Proper degree symbol for LCD
                lcd.print("C");
                lastTemp = temperature;
            }
            
            if (abs(lastHum - humidity) >= 0.1) {
                lcd.setCursor(10, 1);
                lcd.print("     ");
                lcd.setCursor(10, 1);
                lcd.print(humidity, 1);
                lcd.print("%");
                lastHum = humidity;
            }
            
            // Update servo position
            int currentServoPos = myservo.read();
            if (lastServoPos != currentServoPos) {
                lcd.setCursor(10, 2);
                lcd.print("     ");
                lcd.setCursor(10, 2);
                lcd.print(currentServoPos);
                lcd.write(0xDF);  // Proper degree symbol for LCD
                lastServoPos = currentServoPos;
            }
            
            xSemaphoreGive(xMutex);
        }
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void TaskWebSocket(void *pvParameters) {
    String lastJson;
    
    while(1) {
        if(xSemaphoreTake(xMutex, portMAX_DELAY)) {
            // Ensure valid readings before sending
            if (!isnan(temperature) && !isnan(humidity)) {
                // Format JSON with fixed decimal places
                char json[128];
                snprintf(json, sizeof(json), 
                    "{\"temperature\":%.1f,\"humidity\":%.1f,\"timestamp\":%lu}",
                    temperature, humidity, millis());
                
                // Only send if data changed
                String currentJson = String(json);
                if (lastJson != currentJson) {
                 //   webSocket.broadcastTXT(currentJson);
                    lastJson = currentJson;
                    Serial.println("WS Sent: " + currentJson); // Debug output
                }
            }
            xSemaphoreGive(xMutex);
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

// Add WebSocket event handler
void onWebSocketEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, 
                     AwsEventType type, void *arg, uint8_t *payload, size_t length) {
    switch(type) {
        case WS_EVT_DISCONNECT:
            Serial.printf("Client #%u disconnected\n", client->id());
            break;
        case WS_EVT_CONNECT:
            {
                Serial.printf("Client #%u connected from %s\n", client->id(), 
                            client->remoteIP().toString().c_str());
                // Send current values
                char json[128];
                snprintf(json, sizeof(json), 
                    "{\"temperature\":%.1f,\"humidity\":%.1f,\"timestamp\":%lu}",
                    temperature, humidity, millis());
                client->text(json);
            }
            break;
        case WS_EVT_DATA:
            {
                Serial.printf("Received data from client #%u\n", client->id());
                String message = String((char*)payload);
                if(message == "getData") {
                    char json[128];
                    snprintf(json, sizeof(json), 
                        "{\"temperature\":%.1f,\"humidity\":%.1f,\"timestamp\":%lu}",
                        temperature, humidity, millis());
                    client->text(json);
                }
            }
            break;
    }
}

void TaskSensor(void *pvParameters) {
    // Initialize sensor error counter
    uint8_t errorCount = 0;
    
    while(1) {
        if(xSemaphoreTake(xMutex, portMAX_DELAY)) {
            float newTemp = dht.readTemperature();
            float newHum = dht.readHumidity();
            
            if (!isnan(newTemp) && !isnan(newHum)) {
                temperature = newTemp;
                humidity = newHum;
                errorCount = 0;  // Reset error counter on successful read
                Serial.printf("Temp: %.1f°C, Humidity: %.1f%%\n", temperature, humidity);
            } else {
                errorCount++;
                if (errorCount > 5) {
                    Serial.println("DHT sensor error!");
                }
            }
            xSemaphoreGive(xMutex);
        }
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

void setup() {
    Serial.begin(115200);
    
    // Initialize sensors and LCD
    dht.begin();
    lcd.init();
    lcd.backlight();
    
    // Create mutex
    xMutex = xSemaphoreCreateMutex();
    if (xMutex == NULL) {
        Serial.println("Mutex creation failed!");
        while (1) { delay(1000); }
    }
    
    // WiFi connection (matching test.cpp format)
    Serial.print("Connecting to WiFi... ");
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password, WIFI_CHANNEL);
    if (WiFi.waitForConnectResult() != WL_CONNECTED) {
        Serial.printf("WiFi Failed!\n");
        return;
    }
    Serial.println(" Connected!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
    Serial.println("HTTP server started (http://localhost:8180)");  // Direct localhost link
    Serial.println("WebSocket server started (ws://localhost:8181)");  // WebSocket localhost link

    // Update server setup
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(200, "text/html", createHtml());
        Serial.println("Web page requested");
    });
    
    // Initialize servo
    myservo.attach(SERVO_PIN);
    myservo.write(0);  // Initial position
    
    // Add servo endpoint for AJAX updates (0-180 degrees)  ===================================
    server.on("/servo", HTTP_GET, [](AsyncWebServerRequest *request) {
        String message;
        // Receive value from AJAX request
        if (request->hasParam("value")) {
            int value = request->getParam("value")->value().toInt();
            value = constrain(value, 0, 180);
            // Set servo position and return message
            myservo.write(value);
            message = "Servo set to " + String(value);
        } else {
            message = "No value sent";
        }
        request->send(200, "text/plain", message);
    });

    // Add DHT sensors endpoint for AJAX updates      ==========================================
    server.on("/sensors", HTTP_GET, [](AsyncWebServerRequest *request){
        String json = "{\"temperature\":" + String(temperature, 1) + 
                     ",\"humidity\":" + String(humidity, 1) + "}";
        request->send(200, "application/json", json);
    });

    // WebSocket setup
    ws.onEvent(onWebSocketEvent);
    server.addHandler(&ws);

    server.begin();
    Serial.println("HTTP server started");

    // Create tasks with error checking
    BaseType_t xReturned;
    
    xReturned = xTaskCreatePinnedToCore(
        TaskSensor,
        "Sensor",
        3000,
        NULL,
        2,
        &TaskSensor_Handle,
        0
    );
    if (xReturned != pdPASS) {
        Serial.println("Sensor task creation failed!");
    }
    
    xReturned = xTaskCreatePinnedToCore(
        TaskDisplay,
        "DisplayTask",
        3000,
        NULL,
        1,
        &TaskDisplay_Handle,
        1
    );
    if (xReturned != pdPASS) {
        Serial.println("Display task creation failed!");
    }

    xReturned = xTaskCreatePinnedToCore(
        TaskWebSocket,
        "WebSocketTask",
        4096,
        NULL,
        1,
        &TaskWebSocket_Handle,
        1
    );
    if (xReturned != pdPASS) {
        Serial.println("WebSocket task creation failed!");
    }
}

void loop() {
    // No need for explicit WebSocket handling in loop for AsyncWebSocket
    vTaskDelay(pdMS_TO_TICKS(10));
}
