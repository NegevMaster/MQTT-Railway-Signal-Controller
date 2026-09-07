#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "config.h"
#include "structures.h"
#include "signals.h"

using namespace std;

#define MAX_FLASHING_LEDS 10
flashingLED flashingLEDs[MAX_FLASHING_LEDS];
int flashingLEDCount = 0;

distantSignals myDistantSignals[] = {
  { "Gt_ToA", "Sygnał Os1", {0, 0}, {1, 0} },
  { "Gt_ToF", "Sygnał Os1", {2, 0}, {3, 0} },
};

signals mySignals[] = {
    {
        "Gt_A",
        "Gt_A",
        "Sygnał S1",
        Sm,
        true,
        "Gt_ToA",
        {1, 2},
        {2, 2},
        {3, 2},
        {4, 2},
        {5, 2}
    },
    {
        "Gt_B",
        "Gt_B",
        "Sygnał S1",
        Sm,
        false,
        "",
        {1, 1},
        {-1, -1},
        {2, 1},
        {3, 1},
        {4, 1}
    },
    {
        "Gt_C",
        "Gt_C",
        "Sygnał S1",
        Sm,
        false,
        "",
        {8, 0},
        {-1, -1},
        {9, 0},
        {-1, -1},
        {10, 0}
    },
    {
        "Gt_D",
        "Gt_D",
        "Sygnał S1",
        Sm,
        false,
        "",
        {5, 1},
        {-1, -1},
        {6, 1},
        {7, 1},
        {8, 1}
    },
    {
        "Gt_E",
        "Gt_E",
        "Sygnał S1",
        Sm,
        false,
        "",
        {11, 1},
        {-1, -1},
        {12, 1},
        {-1, -1},
        {13, 1}
    },
    {
        "Gt_F",
        "Gt_F",
        "Sygnał S1",
        Sm,
        true,
        "Gt_ToF",
        {6, 2},
        {7, 2},
        {8, 2},
        {9, 2},
        {10, 2}
    }
  
};

void initializePCA9685Boards() {
  for (int i = 0; i < number_of_PCA9685; i++) {
    pwmBoards[i] = Adafruit_PWMServoDriver(adressesPCA9685[i]);
    pwmBoards[i].begin();
    pwmBoards[i].setOscillatorFrequency(27000000);
    pwmBoards[i].setPWMFreq(60);
    Serial.print("PCA9685 Board ");
    Serial.print(i);
    Serial.print(" initialized at address 0x");
    Serial.println(adressesPCA9685[i], HEX);
  }
}

void startFlashingLED(struct led ledPin, int onDurationMs = 500, int offDurationMs = 500, int brightness = 4095) {
  if (!ledPin.isValid()) {
    return;
  }

  if (flashingLEDCount < MAX_FLASHING_LEDS) {
    flashingLEDs[flashingLEDCount].boardNumber = ledPin.indexPCA9685;
    flashingLEDs[flashingLEDCount].pin = ledPin.pin;
    flashingLEDs[flashingLEDCount].brightness = brightness;
    flashingLEDs[flashingLEDCount].onDurationMs = onDurationMs;
    flashingLEDs[flashingLEDCount].offDurationMs = offDurationMs;
    flashingLEDs[flashingLEDCount].lastToggle = millis();
    flashingLEDs[flashingLEDCount].isOn = true;
    flashingLEDs[flashingLEDCount].isFlashing = true;
    pwmBoards[ledPin.indexPCA9685].setPWM(ledPin.pin, 0, brightness);
    flashingLEDCount++;
  }
}

void updateFlashingLEDs() {
  unsigned long now = millis();
  for (int i = 0; i < flashingLEDCount; i++) {
    unsigned long elapsed = now - flashingLEDs[i].lastToggle;
    int totalDuration = flashingLEDs[i].onDurationMs + flashingLEDs[i].offDurationMs;

    elapsed = elapsed % totalDuration;

    int brightness;

    if (elapsed < flashingLEDs[i].onDurationMs) {
      float phaseProgress = (float)elapsed / flashingLEDs[i].onDurationMs;
      brightness = (int)(sin(phaseProgress * PI / 2.0) * flashingLEDs[i].brightness);
    } else {
      unsigned long offElapsed = elapsed - flashingLEDs[i].onDurationMs;
      float phaseProgress = (float)offElapsed / flashingLEDs[i].offDurationMs;
      brightness = (int)(cos(phaseProgress * PI / 2.0) * flashingLEDs[i].brightness);
    }

    if (brightness < 0) brightness = 0;
    if (brightness > 4095) brightness = 4095;

    pwmBoards[flashingLEDs[i].boardNumber].setPWM(flashingLEDs[i].pin, 0, brightness);
  }
}

void turnOffLED(struct led ledPin) {
  if (!ledPin.isValid()) {
    return;
  }

  for (int i = 0; i < flashingLEDCount; i++) {
    if (flashingLEDs[i].boardNumber == ledPin.indexPCA9685 && flashingLEDs[i].pin == ledPin.pin) {
      for (int j = i; j < flashingLEDCount - 1; j++) {
        flashingLEDs[j] = flashingLEDs[j + 1];
      }
      flashingLEDCount--;
      break;
    }
  }

  pwmBoards[ledPin.indexPCA9685].setPWM(ledPin.pin, 0, 0);
}

void setSolidLED(struct led ledPin, int brightness = 4095) {
  if (!ledPin.isValid()) {
    return;
  }

  turnOffLED(ledPin);
  pwmBoards[ledPin.indexPCA9685].setPWM(ledPin.pin, 0, brightness);
}

void settingSignals(std::string signalToDisplay, char* topic) {
  int signalCount = sizeof(mySignals) / sizeof(mySignals[0]);
  int distantCount = sizeof(myDistantSignals) / sizeof(myDistantSignals[0]);

  for (int i = 0; i < signalCount; i++) {
    String myTopic = String(BASE_MQTT_TOPIC) + mySignals[i].suffixMQTT;
    if (myTopic == topic) {
      if (mySignals[i].hasTo == false) {
        turnOffLED(mySignals[i].greenLED);
        turnOffLED(mySignals[i].orangeTopLED);
        turnOffLED(mySignals[i].redLED);
        turnOffLED(mySignals[i].orangeBottomLED);
        turnOffLED(mySignals[i].whiteLED);

        if (signalToDisplay == "Sygnał S1") {
          setSolidLED(mySignals[i].redLED);
        } else if (signalToDisplay == "Sygnał S1z") {
          setSolidLED(mySignals[i].redLED);
          startFlashingLED(mySignals[i].whiteLED);
        } else if (signalToDisplay == "Sygnał S2") {
          setSolidLED(mySignals[i].greenLED);
        } else if (signalToDisplay == "Sygnał S3") {
          startFlashingLED(mySignals[i].greenLED);
        } else if (signalToDisplay == "Sygnał S4") {
          startFlashingLED(mySignals[i].orangeTopLED);
        } else if (signalToDisplay == "Sygnał S5") {
          setSolidLED(mySignals[i].orangeTopLED);
        } else if (signalToDisplay == "Sygnał S10") {
          setSolidLED(mySignals[i].greenLED);
          setSolidLED(mySignals[i].orangeBottomLED);
        } else if (signalToDisplay == "Sygnał S11") {
          startFlashingLED(mySignals[i].greenLED);
          setSolidLED(mySignals[i].orangeBottomLED);
        } else if (signalToDisplay == "Sygnał S12") {
          startFlashingLED(mySignals[i].orangeTopLED);
          setSolidLED(mySignals[i].orangeBottomLED);
        } else if (signalToDisplay == "Sygnał S13") {
          setSolidLED(mySignals[i].orangeTopLED);
          setSolidLED(mySignals[i].orangeBottomLED);
        }
      } else if (mySignals[i].hasTo == true) {
        for (int j = 0; j < distantCount; j++) {
          if (strcmp(mySignals[i].ToName, myDistantSignals[j].name) == 0) {
            turnOffLED(mySignals[i].greenLED);
            turnOffLED(mySignals[i].orangeTopLED);
            turnOffLED(mySignals[i].redLED);
            turnOffLED(mySignals[i].orangeBottomLED);
            turnOffLED(mySignals[i].whiteLED);
            turnOffLED(myDistantSignals[j].greenLED);
            turnOffLED(myDistantSignals[j].orangeLED);

            if (signalToDisplay == "Sygnał S1") {
              setSolidLED(mySignals[i].redLED);
              setSolidLED(myDistantSignals[j].orangeLED);
            } else if (signalToDisplay == "Sygnał S1z") {
              setSolidLED(mySignals[i].redLED);
              startFlashingLED(mySignals[i].whiteLED);
              setSolidLED(myDistantSignals[j].orangeLED);
            } else if (signalToDisplay == "Sygnał S2") {
              setSolidLED(mySignals[i].greenLED);
              setSolidLED(myDistantSignals[j].greenLED);
            } else if (signalToDisplay == "Sygnał S3") {
              startFlashingLED(mySignals[i].greenLED);
              setSolidLED(myDistantSignals[j].greenLED);
            } else if (signalToDisplay == "Sygnał S4") {
              startFlashingLED(mySignals[i].orangeTopLED);
              setSolidLED(myDistantSignals[j].greenLED);
            } else if (signalToDisplay == "Sygnał S5") {
              setSolidLED(mySignals[i].orangeTopLED);
              setSolidLED(myDistantSignals[j].greenLED);
            } else if (signalToDisplay == "Sygnał S10") {
              setSolidLED(mySignals[i].greenLED);
              setSolidLED(mySignals[i].orangeBottomLED);
              startFlashingLED(myDistantSignals[j].orangeLED);
            } else if (signalToDisplay == "Sygnał S11") {
              startFlashingLED(mySignals[i].greenLED);
              setSolidLED(mySignals[i].orangeBottomLED);
              startFlashingLED(myDistantSignals[j].orangeLED);
            } else if (signalToDisplay == "Sygnał S12") {
              startFlashingLED(mySignals[i].orangeTopLED);
              setSolidLED(mySignals[i].orangeBottomLED);
              startFlashingLED(myDistantSignals[j].orangeLED);
            } else if (signalToDisplay == "Sygnał S13") {
              setSolidLED(mySignals[i].orangeTopLED);
              setSolidLED(mySignals[i].orangeBottomLED);
              startFlashingLED(myDistantSignals[j].orangeLED);
            }
          }
        }
      }
    }
  }
}

void MessageReceiving(char* topic, uint8_t* message, unsigned int length) {
  std::string receivedMessage = "";

  for (unsigned int i = 0; i < length; i++) {
    receivedMessage += (char)message[i];
  }

  std::string signalToDisplay = receivedMessage.substr(0, receivedMessage.find(';'));
  settingSignals(signalToDisplay, topic);
}

void reconnect() {
  bool ctd = false;
  Serial.println("Disconnected!");
  while (!ctd) {
    Serial.print("Reconnecting...");
    if (client.connect(DEVICE_NAME)) {
      ctd = true;
      Serial.println("Connected!");
    } else {
      Serial.print(".");
      delay(1000);
    }
  }
}

void subscribingToSignals() {
  int signalCount = sizeof(mySignals) / sizeof(mySignals[0]);
  for (int i = 0; i < signalCount; i++) {
    String topic = String(BASE_MQTT_TOPIC) + mySignals[i].suffixMQTT;
    client.subscribe(topic.c_str());
  }
}
