#pragma once
#include <Arduino.h>


constexpr uint8_t BUTTON_1 = 25;
constexpr uint8_t BUTTON_2 = 26;
constexpr uint8_t BUTTON_3 = 27;
constexpr uint8_t BUTTON_4 = 14;
constexpr uint8_t BUTTON_5 = 13;
constexpr uint8_t BUTTON_6 = 15;

constexpr uint8_t POTENTIOMETER_PIN = 34;
constexpr uint16_t POT_MAX = 4095;
constexpr uint16_t POT_MIN = 0;
extern int last_volume_pot_value;

constexpr int BUTTON_PRESS_DELAY = 30;

struct Button_struct {
  bool btn1;
  bool btn2;
  bool btn3;
  bool btn4;
  bool btn5;
  bool btn6;

  unsigned long lastBtn1Time = 0;
  unsigned long lastBtn2Time = 0;
  unsigned long lastBtn3Time = 0;
  unsigned long lastBtn4Time = 0;
  unsigned long lastBtn5Time = 0;
  unsigned long lastBtn6Time = 0;
};

extern int pot_value;

void BUTTON_SETUP();
void BUTTON_LOGIC();
