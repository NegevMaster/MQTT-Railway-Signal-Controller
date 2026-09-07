#pragma once

#include <Arduino.h>
#include <PubSubClient.h>
#include <WiFi.h>
#include <Adafruit_PWMServoDriver.h>
#include "config.h"
#include "structures.h"

extern WiFiClient espClient;
extern PubSubClient client;
extern Adafruit_PWMServoDriver pwmBoards[];

void initializePCA9685Boards();
void startFlashingLED(struct led ledPin, int onDurationMs, int offDurationMs, int brightness);
void updateFlashingLEDs();
void turnOffLED(struct led ledPin);
void setSolidLED(struct led ledPin, int brightness);
void settingSignals(std::string signalToDisplay, char* topic);
void MessageReceiving(char* topic, uint8_t* message, unsigned int length);
void reconnect();
void subscribingToSignals();
