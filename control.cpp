#include "control.h"
#include <arduino.h>
#include "display.h"
#include "music.h"


void BUTTON_SETUP() {
  // Setting button output to INPUT_PULLOUT
  pinMode(BUTTON_1, INPUT_PULLUP);
  pinMode(BUTTON_2, INPUT_PULLUP);
  pinMode(BUTTON_3, INPUT_PULLUP);
  pinMode(BUTTON_4, INPUT_PULLUP);
  pinMode(BUTTON_5, INPUT_PULLUP);
}

void HOME_BUTTON_LOGIC(struct Button_struct* Button) {
  if (Button->btn2 == LOW) {
    current_scr = MENU;

    delay(BUTTON_PRESS_DELAY);
  }
}

void MENU_BUTTON_LOGIC(struct Button_struct* Button) {
  if (Button->btn1 == LOW) {
    if (app_index == 0) {
      current_scr = HOME;
      app_index = 0;
      app_marker_pos = 0;
    }
    else {
      app_marker_pos--;
      app_index--;
    }
    Serial.printf("btn1 pressed, app_marker_pos: %d | app_index: %d\n", app_marker_pos, app_index);
    delay(BUTTON_PRESS_DELAY);
  }

  if (Button->btn2 == LOW) {
    if (app_index < total_apps-1) {   // Set to 8 as in one screen it can only hold 8 app;
      app_marker_pos++;
      app_index++;
      Serial.printf("btn2 pressed, app_marker_pos: %d | app_index: %d\n", app_marker_pos, app_index);
      delay(BUTTON_PRESS_DELAY);
    }
  }

  if (Button->btn3 == LOW) {
    current_scr = app_index+2;
    delay(BUTTON_PRESS_DELAY);
  }
    
}



void BUTTON_LOGIC() {
  struct Button_struct Button;
  Button.btn1 = digitalRead(BUTTON_1);
  Button.btn2 = digitalRead(BUTTON_2);
  Button.btn3 = digitalRead(BUTTON_3);
  Button.btn4 = digitalRead(BUTTON_4);
  Button.btn5 = digitalRead(BUTTON_5);

  if (!Button.btn1 | !Button.btn2 | !Button.btn3 | !Button.btn4 | !Button.btn5) {
    switch (current_scr) {
      case HOME:
        HOME_BUTTON_LOGIC(&Button);
        break;
      case MENU:
        MENU_BUTTON_LOGIC(&Button);
        break;
      case MUSIC:
        MUSIC_BUTTON_LOGIC(&Button);
        break;
      default:
        break;
    }
  }
}
