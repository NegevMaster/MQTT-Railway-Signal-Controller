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

#include <stdint.h>

#ifndef NUM_PCA9685
#define NUM_PCA9685 3
#endif

#ifndef DEVICE_SDA_PIN
#define DEVICE_SDA_PIN 21
#endif

#ifndef DEVICE_SCL_PIN
#define DEVICE_SCL_PIN 22
#endif

#define number_of_PCA9685 NUM_PCA9685
const uint8_t adressesPCA9685[number_of_PCA9685] = {0x40, 0x41, 0x42};

#define SDA_PIN DEVICE_SDA_PIN
#define SCL_PIN DEVICE_SCL_PIN
#define I2C_FREQ 100000

#endif
