#ifndef MAIN_H
#define MAIN_H

#include <Arduino.h>
#include <DHT.h>
#include <LiquidCrystal_I2C.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncWebSocket.h>
#include "index_html.h"
#include <ESP32Servo.h>

// Pin Definitions
#define DHT_PIN 4
#define DHT_TYPE DHT22
#define SERVO_PIN 12

// Function declarations
void TaskSensor(void *pvParameters);
void TaskDisplay(void *pvParameters);
String createHtml();

// External declarations
extern DHT dht;
extern LiquidCrystal_I2C lcd;
extern AsyncWebServer server;
extern SemaphoreHandle_t xMutex;
extern Servo myservo;

// WiFi credentials (matching test.cpp exactly)
const char* ssid = "Wokwi-GUEST";
const char* password = "";
const int WIFI_CHANNEL = 6; // Speeds up the connection in Wokwi


#endif // MAIN_H