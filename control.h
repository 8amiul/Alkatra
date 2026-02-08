#pragma once
#include <Arduino.h>


constexpr uint8_t BUTTON_1 = 25;
constexpr uint8_t BUTTON_2 = 26;
constexpr uint8_t BUTTON_3 = 27;
constexpr uint8_t BUTTON_4 = 14;
constexpr uint8_t BUTTON_5 = 13;

constexpr uint8_t BUTTON_PRESS_DELAY = 200;

struct Button_struct {
  bool btn1;
  bool btn2;
  bool btn3;
  bool btn4;
  bool btn5;
};

void BUTTON_SETUP();
void BUTTON_LOGIC();
