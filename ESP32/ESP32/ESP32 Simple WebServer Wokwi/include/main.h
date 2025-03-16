/**
 * @file main.h
 * @brief Main header file for ESP32 LED control web server
 * @details Contains all necessary includes, pin definitions, and function declarations
 *          for the LED control system with web interface
 */

#ifndef MAIN_H
#define MAIN_H

// Required libraries for ESP32, FreeRTOS and Web Server functionality
#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "html.h"

// Hardware configuration
#define LED_PIN 5        // GPIO pin connected to LED
#define BUTTON_PIN 4     // GPIO pin connected to push button

// WiFi configuration for Wokwi simulation
const char* ssid = "Wokwi-GUEST";
const char* password = "";

// External variable declarations
extern TaskHandle_t ledTaskHandle;    // Handle for LED control task
extern volatile bool ledState;        // Current state of LED

// Function declarations
void IRAM_ATTR buttonISR();          // Button interrupt service routine
void ledControlTask(void *parameter); // FreeRTOS task for LED control
void webServerTask(void *parameter);  // FreeRTOS task for web server
String processor(const String& var);  // Template processor for web page

#endif


