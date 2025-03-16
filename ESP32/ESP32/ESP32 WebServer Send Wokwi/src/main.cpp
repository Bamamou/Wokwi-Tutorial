#include "main.h"

// Global objects
AsyncWebServer server(80);  // Changed to match test.cpp approach
DHT dht(DHT_PIN, DHT22);
LiquidCrystal_I2C lcd(LCD_ADDR, 16, 2);
AsyncEventSource events("/events");

// Task handles
TaskHandle_t wifiTaskHandle = NULL;
TaskHandle_t serverTaskHandle = NULL;
TaskHandle_t sensorTaskHandle = NULL;

void WiFiTask(void *parameter) {
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
        vTaskDelay(pdMS_TO_TICKS(500));
        Serial.print(".");
    }
    Serial.println("\nWiFi connected");
    Serial.println("Server started at http://localhost:8280");
    Serial.println(WiFi.localIP());
    
    xTaskNotifyGive(serverTaskHandle);
    vTaskDelete(NULL);
}

void ServerTask(void *parameter) {
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(200, "text/html", getHTML());
    });

    events.onConnect([](AsyncEventSourceClient *client) {
        if(client->lastId()) {
            Serial.printf("Client reconnected! Last message ID: %u\n", client->lastId());
        }
    });
    server.addHandler(&events);
    server.begin();
    
    for(;;) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

String getHTML() {
    return HTML_TEMPLATE;
}

void initLCDLabels() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Temp: ");
    lcd.setCursor(0, 1);
    lcd.print("Hum:  ");
    lcd.setCursor(0, 2);
    lcd.print("HI:  ");
}

void SensorTask(void *parameter) {
    initLCDLabels();  // Initialize LCD labels once
    String tempOld, humOld, hiOld;  // Store previous values
    
    for(;;) {
        float temp = dht.readTemperature();
        float hum = dht.readHumidity();
        
        if (!isnan(temp) && !isnan(hum)) {
            float heatindex = dht.computeHeatIndex(temp, hum, false);
            
            // Update LCD only if values changed
            String tempNew = String(temp, 1) + "C ";
            String humNew = String(hum, 1) + "% ";
            String hiNew = String(heatindex, 1) + "% ";
            
            if (tempNew != tempOld) {
                lcd.setCursor(6, 0);  // Position after "Temp: "
                lcd.print(tempNew);
                tempOld = tempNew;
            }
            
            if (humNew != humOld) {
                lcd.setCursor(6, 1);  // Position after "Hum:  "
                lcd.print(humNew);
                humOld = humNew;
            }
              
            if (hiNew != hiOld) {
                lcd.setCursor(6, 2);  // Position after "Hum:  "
                lcd.print(hiNew);
                hiOld = hiNew;
            }


            // Send event to all connected clients
            String eventData = "{\"temperature\":" + String(temp, 1) + 
                             ",\"humidity\":" + String(hum, 1) + 
                             ",\"heatindex\":" + String(heatindex, 1) + "}";
            events.send(eventData.c_str(), "sensor_data", millis());
        }
        
        vTaskDelay(pdMS_TO_TICKS(UPDATE_INTERVAL));
    }
}

void setup() {
    Serial.begin(115200);
    
    // Initialize hardware
    Wire.begin(I2C_SDA, I2C_SCL);
    lcd.init();
    lcd.backlight();
    dht.begin();
    
    // Create tasks
    xTaskCreate(WiFiTask, "WiFiTask", STACK_SIZE, NULL, 1, &wifiTaskHandle);
    xTaskCreate(ServerTask, "ServerTask", STACK_SIZE, NULL, 1, &serverTaskHandle);
    xTaskCreate(SensorTask, "SensorTask", STACK_SIZE, NULL, 1, &sensorTaskHandle);
}

void loop() {
    vTaskDelete(NULL); // Delete the Arduino loop task
}