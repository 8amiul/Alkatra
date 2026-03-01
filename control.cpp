#include "control.h"
#include <arduino.h>
#include "display.h"
#include "music.h"

int last_volume_pot_value;
void BUTTON_SETUP() {
  // Setting button output to INPUT_PULLOUT
  pinMode(BUTTON_1, INPUT_PULLUP);
  pinMode(BUTTON_2, INPUT_PULLUP);
  pinMode(BUTTON_3, INPUT_PULLUP);
  pinMode(BUTTON_4, INPUT_PULLUP);
  pinMode(BUTTON_5, INPUT_PULLUP);
  pinMode(BUTTON_6, INPUT_PULLUP);
  
  //last_pot_value = analogRead(POTENTIOMETER);
  //pinMode(POTENTIOMETER, INPUT);
  analogSetPinAttenuation(POTENTIOMETER_PIN, ADC_11db);
  last_volume_pot_value = analogRead(POTENTIOMETER_PIN);
}


void HOME_BUTTON_LOGIC(struct Button_struct* Button) {
  unsigned long now = millis();

  if (Button->btn2 == LOW && now - Button->lastBtn2Time >= BUTTON_PRESS_DELAY) {
    current_scr = MENU;
  }

  Button->lastBtn2Time = now;
}


void MENU_BUTTON_LOGIC(struct Button_struct* Button) {
  unsigned long now = millis();

  if (Button->btn1 == LOW && now - Button->lastBtn1Time >= BUTTON_PRESS_DELAY) {

    if (app_index == 0) {
      current_scr = HOME;
      app_index = 0;
      app_marker_pos = 0;
    }
    else {
      app_marker_pos--;
      app_index--;
    }
    //Serial.printf("btn1 pressed, app_marker_pos: %d | app_index: %d\n", app_marker_pos, app_index);
    Button->lastBtn1Time = now;
  }

  if (Button->btn2 == LOW && now - Button->lastBtn2Time >= BUTTON_PRESS_DELAY) {
    if (app_index < total_apps-1) {   // Set to 8 as in one screen it can only hold 8 app;
      app_marker_pos++;
      app_index++;
    }
    //Serial.printf("btn2 pressed, app_marker_pos: %d | app_index: %d\n", app_marker_pos, app_index);
    Button->lastBtn2Time = now;
  }

  if (Button->btn3 == LOW && now - Button->lastBtn3Time >= BUTTON_PRESS_DELAY) {

    if (app_index == MUSIC_APP && isDFPlayerFailed == 1) {
      Button->lastBtn3Time = now;
      return;
    }

    current_scr = app_index+2;
    
    Button->lastBtn3Time = now;
  }
    
}

int last = 0;
int debounce_delay = 300;             // Prolly that's we need to make things non-delay
void BUTTON_LOGIC() {
  struct Button_struct Button;
  Button.btn1 = digitalRead(BUTTON_1);
  Button.btn2 = digitalRead(BUTTON_2);
  Button.btn3 = digitalRead(BUTTON_3);
  Button.btn4 = digitalRead(BUTTON_4);
  Button.btn5 = digitalRead(BUTTON_5);
  Button.btn6 = digitalRead(BUTTON_6);

  if (!Button.btn1) Serial.println(BUTTON_1);
  if (!Button.btn2) Serial.println(BUTTON_2);
  if (!Button.btn3) Serial.println(BUTTON_3);
  if (!Button.btn4) Serial.println(BUTTON_4);
  if (!Button.btn5) Serial.println(BUTTON_5);
  if (!Button.btn6) Serial.println(BUTTON_6);

  unsigned long now = millis();

  if ((!Button.btn1 || !Button.btn2 || !Button.btn3 || !Button.btn4 || !Button.btn5 || !Button.btn6) && (now - last >= debounce_delay)) { // and this

    switch (current_scr) {
      case HOME: HOME_BUTTON_LOGIC(&Button); break;
      
      case MENU: MENU_BUTTON_LOGIC(&Button); break;
      
      case MUSIC: MUSIC_BUTTON_LOGIC(&Button); break; 
        case MUSIC_SCREEN_EQ:
          MUSIC_SCREEN_EQ_BUTTON_LOGIC(&Button);
          break;
        case MUSIC_SCREEN_SONG_LIST:
          MUSIC_SCREEN_SONG_LIST_BUTTON_LOGIC(&Button);
          break;
        case MUSIC_SCREEN_VISUALIZER:
          MUSIC_SCREEN_VISUALIZER_BUTTON_LOGIC(&Button);
          break;
        case MUSIC_SCREEN_LYRICS:
          MUSIC_SCREEN_LYRICS_BUTTON_LOGIC(&Button);
          break;      

      default: break;
    }

    last = now;
  }
}
