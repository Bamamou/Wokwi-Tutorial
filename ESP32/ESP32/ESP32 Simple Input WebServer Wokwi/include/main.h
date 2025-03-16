#ifndef MAIN_H
#define MAIN_H

#include <Arduino.h>
#include <TM1637.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "html.h"

#define CLK 2
#define DIO 4 

#define RED_PIN 27
#define GREEN_PIN 26
#define BLUE_PIN 25

struct rgb_values {
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

extern QueueHandle_t rgbQueue;

extern TM1637 tm;
extern unsigned int counter;
extern SemaphoreHandle_t counterMutex;
extern AsyncWebServer server;
extern bool isCounterRunning;

// function prototypes
void displayTask(void *parameter);
void counterTask(void *parameter);
void handleRGB(AsyncWebServerRequest *request);
void rgbTask(void *parameter);

#endif
