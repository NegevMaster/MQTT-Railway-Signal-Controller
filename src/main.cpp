#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "config.h"
#include "structures.h"
#include "signals.h"
#include <string>
#include <cmath>

using namespace std;

WiFiClient espClient;
PubSubClient client(espClient);

Adafruit_PWMServoDriver pwmBoards[number_of_PCA9685];

void setup() {
  Serial.begin(115200);
  initializePCA9685Boards();
  pinMode(STATUS_LED_PIN, OUTPUT);
  delay(1000);

  WiFi.mode(WIFI_STA);
  WiFi.begin(Wifi_SSID, Wifi_Password);
  Serial.println("\nConnecting");

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(100);
  }

  Serial.println("\nConnected to the WiFi network");
  Serial.print("Local Railway Controller IP:");
  Serial.println(WiFi.localIP());

  client.setServer(MQTT_BROKER, MQTT_PORT);
  client.connect(DEVICE_NAME, "", "", LAST_WILL_MQTT_TOPIC, 0, false, "OFFLINE");
  client.publish(LAST_WILL_MQTT_TOPIC, "ONLINE", true);
  client.setCallback(MessageReceiving);
  delay(1000);
  subscribingToSignals();
}

void loop() {
  updateFlashingLEDs();
  digitalWrite(STATUS_LED_PIN, true);

  if (!client.connected()) {
    digitalWrite(STATUS_LED_PIN, false);
    reconnect();
    client.publish(LAST_WILL_MQTT_TOPIC, "ONLINE");
    subscribingToSignals();
  }

  if (!client.loop()) {
    client.connect(DEVICE_NAME, "", "", LAST_WILL_MQTT_TOPIC, 0, false, "OFFLINE");
  }
}
