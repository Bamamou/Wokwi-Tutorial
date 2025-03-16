#include "main.h"

TM1637 tm(CLK, DIO);
unsigned int counter = 0;
SemaphoreHandle_t counterMutex;
AsyncWebServer server(80);
bool isCounterRunning = false;
QueueHandle_t rgbQueue;

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

void rgbTask(void *parameter) {
    struct rgb_values rgb;
    while(1) {
        if(xQueueReceive(rgbQueue, &rgb, portMAX_DELAY)) {
            ledcWrite(0, rgb.r);
            ledcWrite(1, rgb.g);
            ledcWrite(2, rgb.b);
        }
        vTaskDelay(pdMS_TO_TICKS(20)); // Small delay for stability
    }
}

void handleRGB(AsyncWebServerRequest *request) {
    if(request->hasParam("r") && request->hasParam("g") && request->hasParam("b")) {
        struct rgb_values rgb;
        rgb.r = request->getParam("r")->value().toInt();
        rgb.g = request->getParam("g")->value().toInt();
        rgb.b = request->getParam("b")->value().toInt();
        
        // Print RGB values to Serial
        Serial.printf("RGB Values - R: %d, G: %d, B: %d\n", rgb.r, rgb.g, rgb.b);
        
        xQueueSend(rgbQueue, &rgb, 0);
        request->send(200, "text/plain", "OK");
    } else {
        request->send(400, "text/plain", "Missing RGB parameters");
    }
}

void setup() {
  Serial.begin(115200);
  tm.init();
  tm.set(BRIGHT_TYPICAL);
  
  // Create semaphore for counter protection
  counterMutex = xSemaphoreCreateMutex();
  
  // WiFi Setup with Wokwi credentials
  WiFi.begin("Wokwi-GUEST", "", 6);
  Serial.print("Connecting to WiFi");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }
  
  Serial.println("\nConnected to WiFi");
  Serial.print("Local IP Address: http://");
  Serial.println(WiFi.localIP());
  Serial.println("HTTP server started (http://localhost:8180)");  // Direct localhost link
  
  // Async Web Server Routes
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", MAIN_page);
  });

  server.on("/toggle", HTTP_GET, [](AsyncWebServerRequest *request){
    if(request->hasParam("state")) {
      isCounterRunning = (request->getParam("state")->value() == "1");
      request->send(200, "text/plain", "OK");
    }
  });

  // Create RGB queue before LED setup
  rgbQueue = xQueueCreate(5, sizeof(struct rgb_values));
  
  // Setup RGB LED PWM
  ledcSetup(0, 5000, 8);
  ledcSetup(1, 5000, 8);
  ledcSetup(2, 5000, 8);
  
  ledcAttachPin(RED_PIN, 0);
  ledcAttachPin(GREEN_PIN, 1);
  ledcAttachPin(BLUE_PIN, 2);
  
  // Add RGB control route
  server.on("/rgb", HTTP_GET, handleRGB);

  server.begin();

  // Create tasks
  xTaskCreate(
    displayTask,    // Task function
    "DisplayTask",  // Name
    4000,          // Stack size
    NULL,          // Parameters
    1,             // Priority
    NULL           // Task handle
  );
  
  xTaskCreate(
    counterTask,    // Task function
    "CounterTask",  // Name
    4000,          // Stack size
    NULL,          // Parameters
    1,             // Priority
    NULL           // Task handle
  );

  xTaskCreate(
    rgbTask,       // Task function
    "RGBTask",     // Name
    4000,          // Stack size
    NULL,          // Parameters
    1,             // Priority
    NULL          // Task handle
  );
}

void loop() {
  // Empty loop as we're using FreeRTOS tasks
  vTaskDelete(NULL);
}
