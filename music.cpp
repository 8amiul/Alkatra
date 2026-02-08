#include "music.h"
#include "display.h"
#include "bitmaps.h"
#include "DFRobotDFPlayerMini.h"
#include <SPI.h>
#include "SoftwareSerial.h"
#include <Wire.h>
#include "songs_list.h"
/* ------------------- DF Player mini initialization ----------- */
// Use pins 2 and 3 to communicate with DFPlayer Mini

SoftwareSerial softwareSerial(PIN_MP3_RX, PIN_MP3_TX);

DFRobotDFPlayerMini myDFPlayer;

int select_music_button_pos = 2;      // the default position where the selector would be is on the play/pause button
int total_buttons = 7;
bool isShuffle = 0;
bool isPaused = -1;
int currentMusicFile = 1;
int STATE = STOPPED;
bool isMusicDonePlaying = 0;

void DEBUG_MUSIC() {
  Serial.println("----- DEBUG -----!");
  Serial.printf("State: %d\n",myDFPlayer.readState()); //read mp3 state
  Serial.printf("Current Music File: %d | STATE (VAR): %d | DONE: %d\n", readCurrentFileNumber(), STATE, isMusicDonePlaying);
  //Serial.println(myDFPlayer.readVolume()); //read current volume
  //Serial.printf("EQ: %d\n", myDFPlayer.readEQ()); //read EQ setting
  //Serial.println(myDFPlayer.readFileCounts()); //read all file counts in SD card
  //Serial.printf("Current music: %d\n",myDFPlayer.readCurrentFileNumber()); //read current play file number
  //Serial.println(myDFPlayer.readFileCountsInFolder(1)); //read file counts in folder SD:/03

}

void DF_PLAYER_INIT() {
  if (myDFPlayer.begin(softwareSerial)) {
    Serial.println("DF_PLAYER_SOFTWARE_SERIAL_OK");
    myDFPlayer.volume(DEFAULT_VOLUME);
    myDFPlayer.enableLoopAll();

  } else {
    Serial.println("Connecting to DFPlayer Mini failed!");
  }
  pinMode(BUSY_PIN, INPUT);
}

void DRAW_MUSIC_UI(void) {
    u8g2.clearBuffer();
    u8g2.setFontMode(1);
    u8g2.setBitmapMode(1);
    // window_border
    u8g2.drawFrame(0, 10, 128, 54);

    // Track title
    u8g2.setFont(u8g2_font_timR10_tr);
    u8g2.drawStr(3, 23, "Let it happen");

    // Track artist
    u8g2.setFont(u8g2_font_4x6_tr);
    u8g2.drawStr(4, 41, "Tame Impala");

    // Track album
    u8g2.setFont(u8g2_font_6x10_tr);
    u8g2.drawStr(4, 33, "Currents");

    
    // music_play
    if (myDFPlayer.readState() != PLAYING) {
      u8g2.drawXBMP(42, 51, 8, 8, image_music_play_bits);
    }
    else {
    // music_pause
      u8g2.setDrawColor(2);
      u8g2.drawXBMP(42, 51, 6, 8, image_music_pause_bits);
    }


    // music_previous
    u8g2.drawXBMP(25, 51, 8, 8, image_music_previous_bits);

    // music_next
    u8g2.drawXBMP(57, 51, 8, 8, image_music_next_bits);

    // operation_undo
    u8g2.drawXBMP(73, 51, 6, 8, image_operation_undo_bits);

    // shuffle
    u8g2.drawXBMP(4, 50, 13, 11, image_shuffle_bits);

    // progress-bar
    u8g2.drawLine(5, 46, 69, 46);

    // progress-ending-point
    u8g2.drawXBMP(77, 42, 2, 5, image_progress_ending_point_bits);

    // progress-starting-point
    u8g2.drawXBMP(3, 44, 5, 5, image_progress_starting_point_bits);

    // volume-bar
    u8g2.drawLine(101, 54, 123, 54);

    // volume-starting-point copy 1
    u8g2.drawXBMP(99, 52, 5, 5, image_progress_starting_point_bits);

    // menu
    u8g2.drawXBMP(86, 51, 8, 8, image_menu_bits);

    // close 
    u8g2.setDrawColor(2);
    u8g2.drawXBMP(115, 11, 12, 8, image_close_bits);

    // select     
    u8g2.setDrawColor(2);
    switch (select_music_button_pos) {
      case SHUFFLE:
        u8g2.drawBox(4, 50, 13, 11);
        break;
      case PREV:
        u8g2.drawBox(23, 50, 12, 11);
        break;
      case PAUSE_PLAY:
        u8g2.drawBox(40, 50, 11, 11);
        break;
      case NEXT:
        u8g2.drawBox(55, 50, 12, 11);
        break;
      case LOOP:
        u8g2.drawBox(70, 50, 12, 11);
        break;
      case LIST:
        u8g2.drawBox(84, 50, 12, 11);
        break;
      case CLOSE:
        u8g2.drawBox(115, 11, 12, 8);
        break;
      default:
        u8g2.drawBox(40, 50, 11, 11);
        break;
    }

    u8g2.sendBuffer();
}

int checkMusicDonePlaying() {
  int busy_pin = digitalRead(BUSY_PIN);
  Serial.println(busy_pin);

  if (busy_pin == HIGH && STATE == PLAYING) {
    isMusicDonePlaying = 1;
    //STATE = STOPPED;
    myDFPlayer.next();
    STATE = PLAYING;
    Serial.println("------------ DONE PLAYING --------------");
  }
  else {
    isMusicDonePlaying = 0;
  }

  return STATE;
}

void MUSIC_BUTTON_LOGIC(struct Button_struct* Button) {
  if (Button->btn1 == LOW) {
    select_music_button_pos--;
    if (select_music_button_pos <= -1)
      select_music_button_pos = 6;
    delay(BUTTON_PRESS_DELAY);
  }

  if (Button->btn2 == LOW) {
    select_music_button_pos++;
    if (select_music_button_pos >= total_buttons) {
      select_music_button_pos = 0;
    }
    delay(BUTTON_PRESS_DELAY);
  }

  if (Button->btn3 == LOW) {
    switch (select_music_button_pos) {
      case SHUFFLE:
        if (isShuffle == 0) {
          isShuffle = 1;
        }
        else {
          isShuffle = 0;
        }
        break;
      case PREV:
        myDFPlayer.previous();
        break;
      case PAUSE_PLAY:
        if (STATE == STOPPED || STATE == 0) {
          myDFPlayer.loopFolder(1);
          STATE = PLAYING;
        }
        else if (STATE == PAUSED) {
          myDFPlayer.start();
          STATE = PLAYING;
        }
        else if (STATE == PLAYING) {
          myDFPlayer.pause();
          STATE = PAUSED;
        }
        break;
      case NEXT:
        myDFPlayer.next();
        break;
      case LOOP:
         myDFPlayer.enableLoop();
        break;
      case LIST:

        break;
      case CLOSE:
        current_scr = MENU;
        break;
      default:
        break;
    }
    delay(BUTTON_PRESS_DELAY);
  }

}

