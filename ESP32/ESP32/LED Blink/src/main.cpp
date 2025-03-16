#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

// LED pin definitions
const int led1Pin = 5;    // First LED on GPIO5
const int led2Pin = 4;    // Second LED on GPIO4
const int led3Pin = 12;   // Third LED on GPIO12
const int buttonPin = 15; // Button on GPIO15

// Create LCD object
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Task handles
TaskHandle_t LED1TaskHandle = NULL;
TaskHandle_t LED2TaskHandle = NULL;

// Variable to track LED3 state
volatile bool led3State = false;
volatile unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 200;

// Interrupt Service Routine
void IRAM_ATTR buttonISR() {
    unsigned long currentTime = millis();
    if (currentTime - lastDebounceTime > debounceDelay) {
        led3State = !led3State;
        lastDebounceTime = currentTime;
    }
}

// Task for first LED (1 second interval)
void LED1Task(void *parameter) {
    while(1) {
        digitalWrite(led1Pin, HIGH);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        digitalWrite(led1Pin, LOW);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

// Task for second LED (250ms interval)
void LED2Task(void *parameter) {
    while(1) {
        digitalWrite(led2Pin, HIGH);
        vTaskDelay(250 / portTICK_PERIOD_MS);
        digitalWrite(led2Pin, LOW);
        vTaskDelay(250 / portTICK_PERIOD_MS);
    }
}

// Task for third LED (interrupt controlled)
void LED3Task(void *parameter) {
    while(1) {
        digitalWrite(led3Pin, led3State);
        lcd.setCursor(0, 0);
        lcd.print("LED3 Status:    ");
        lcd.setCursor(0, 1);
        lcd.print(led3State ? "ON " : "OFF");
        vTaskDelay(50 / portTICK_PERIOD_MS);
    }
}

void setup() {
    // Initialize LCD
    lcd.init();
    lcd.backlight();
    lcd.clear();
    
    // Configure pins
    pinMode(led1Pin, OUTPUT);
    pinMode(led2Pin, OUTPUT);
    pinMode(led3Pin, OUTPUT);
    pinMode(buttonPin, INPUT_PULLUP);

    // Attach interrupt
    attachInterrupt(buttonPin, buttonISR, FALLING);

    // Create tasks
    xTaskCreate(
        LED1Task,
        "LED1Task",
        2000,
        NULL,
        2,
        &LED1TaskHandle
    );

    xTaskCreate(
        LED2Task,
        "LED2Task",
        2000,
        NULL,
        2,
        &LED2TaskHandle
    );

    xTaskCreate(
        LED3Task,
        "LED3Task",
        3000,
        NULL,
        1,
        NULL
    );
}

void loop() {
    vTaskDelay(portMAX_DELAY);
}