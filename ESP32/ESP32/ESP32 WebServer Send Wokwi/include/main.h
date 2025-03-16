#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "web.h"

// Constants
#define WIFI_SSID "Wokwi-GUEST"
#define WIFI_PASSWORD ""
#define DHT_PIN 5
#define I2C_SDA 21
#define I2C_SCL 22
#define LCD_ADDR 0x27
#define STACK_SIZE 4096
#define UPDATE_INTERVAL 1000

extern AsyncEventSource events;

// Function declarations
void WiFiTask(void *parameter);
void ServerTask(void *parameter);
void SensorTask(void *parameter);
String getHTML();
