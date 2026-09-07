#pragma once

#include <Arduino.h>

enum signalTypes {
  Sm,
  To
};

struct led {
  int pin;
  int indexPCA9685;

  bool isValid() const {
    return pin >= 0 && indexPCA9685 >= 0;
  }
};

struct flashingLED {
  int pin;
  int boardNumber;
  int brightness;
  unsigned long lastToggle;
  int onDurationMs;
  int offDurationMs;
  bool isOn;
  bool isFlashing;
};

struct distantSignals {
  const char* name;
  const char* currentSignal;
  led greenLED;
  led orangeLED;
};

struct signals {
  const char* name;
  const char* suffixMQTT;
  const char* currentSignal;
  signalTypes type;
  bool hasTo;
  const char* ToName;
  led greenLED;
  led orangeTopLED;
  led redLED;
  led orangeBottomLED;
  led whiteLED;
};

extern distantSignals myDistantSignals[];
extern signals mySignals[];

extern flashingLED flashingLEDs[];
extern int flashingLEDCount;
