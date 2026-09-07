#ifndef CONFIG_H
#define CONFIG_H

#if __has_include("config.local.h")
#include "config.local.h"
#else
#define Wifi_SSID "YOUR_WIFI_SSID"
#define Wifi_Password "YOUR_WIFI_PASSWORD"
#define MQTT_BROKER "192.168.1.10"
#define MQTT_PORT 1883
#define MQTT_USER ""
#define MQTT_PASSWORD ""
#endif

#define USE_BUILTINLED_FOR_STATUS false
#define STATUS_LED_PIN 10

#define LAST_WILL_MQTT_TOPIC "trains/status/mqtt_signal_controller"
#define BASE_MQTT_TOPIC "trains/track/signalmast/"

#define DEVICE_NAME "MQTT_Signal_Controller"

#define number_of_PCA9685 3
#include <stdint.h>
const uint8_t adressesPCA9685[number_of_PCA9685] = {0x40};

#define SDA_PIN 21
#define SCL_PIN 22
#define I2C_FREQ 100000

#endif
