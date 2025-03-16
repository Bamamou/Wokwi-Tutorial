/**
 * @file main.cpp
 * @brief Implementation of ESP32 LED control system with web interface
 * @details Uses FreeRTOS tasks to handle LED control and web server operations
 */

#include "main.h"

// Global variables initialization
TaskHandle_t ledTaskHandle = NULL;

volatile bool ledState = false;
volatile unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 200;
AsyncWebServer server(80);  // Web server on port 80

/**
 * @brief Processes template variables for the web page
 * @param var Template variable to process
 * @return String containing the processed value
 */
String processor(const String& var) {
    if(var == "STATE") {
        return ledState ? "ON" : "OFF";
    }
    return String();
}

/**
 * @brief ISR for button press
 * @details Called when button is pressed (FALLING edge)
 *          Notifies the LED control task
 */
void IRAM_ATTR buttonISR() {
   // BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    unsigned long currentTime = millis();
    if (currentTime - lastDebounceTime > debounceDelay) {
        ledState = !ledState;
        digitalWrite(LED_PIN, ledState);
        lastDebounceTime = currentTime;
        xTaskNotifyGive(ledTaskHandle);  // Notify Task1 to update LCD
    }

}

/**
 * @brief FreeRTOS task to handle LED control
 * @details Waits for notifications from ISR and toggles LED state
 */
void ledControlTask(void *parameter) {
   
   
    while(1) {
        
        if (ulTaskNotifyTake(pdTRUE, 0)) {
            digitalWrite(LED_PIN, digitalRead(LED_PIN));
        }
        vTaskDelay(250 / portTICK_PERIOD_MS);
    }
}

/**
 * @brief FreeRTOS task to handle web server
 * @details Sets up routes and handles HTTP requests
 */
void webServerTask(void *parameter) {
    // Enable CORS for web browser access
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
    
    // Define web server routes
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send_P(200, "text/html", index_html, processor);
    });

    // Route for getting LED status
    server.on("/status", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(200, "text/plain", ledState ? "ON" : "OFF");
    });

    server.begin();
    Serial.println("HTTP server started");

    while(1) {
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

/**
 * @brief Setup function
 * @details Initializes hardware, WiFi connection, and FreeRTOS tasks
 */
void setup() {
    pinMode(LED_PIN, OUTPUT);
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    
    Serial.begin(115200);
    Serial.println("Starting ESP32...");
    
    // Configure WiFi channel and mode
    WiFi.mode(WIFI_STA);
    WiFi.channel(6);
    
    // Connect to Wi-Fi
    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(1000);
    }
    
    Serial.println("\nWiFi Connected!");
    Serial.println("Open your browser and navigate to: http://" + WiFi.localIP().toString());
    Serial.println("HTTP server started (http://localhost:8080)");  // Direct localhost link
    
    // Create LED control task
    xTaskCreate(
        ledControlTask,
        "LED Control",
        2048,
        NULL,
        1,
        &ledTaskHandle
    );
    
    // Create web server task
    xTaskCreate(
        webServerTask,
        "Web Server",
        4096,
        NULL,
        1,
        NULL
    );
    
    attachInterrupt(BUTTON_PIN, buttonISR, FALLING);
}

/**
 * @brief Main loop
 * @details Empty as functionality is handled by FreeRTOS tasks
 */
void loop() {
    // Main loop remains empty as FreeRTOS handles the tasks
    vTaskDelete(NULL);
}