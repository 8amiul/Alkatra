#include "display.h"
#include "control.h"
#include "functions.h"
#include "music.h"
#include <SPI.h>
#include "SoftwareSerial.h"


void setup() {
  Serial.begin(9600);
  softwareSerial.begin(9600);
  u8g2.begin();
  DRAW_BOOT_LOGO();
  DF_PLAYER_INIT();
  wifi_connect();
  date_allocation();
  BUTTON_SETUP();
}

static unsigned long lastTime = 0;
static const unsigned long interval = 500;
char key;

void loop() {
  DRAW_SCREEN();
  BUTTON_LOGIC();
  get_time();
  musicProgressTimeHandle();

  // Loop for blocking function
  unsigned long currentTime = millis();
  if (currentTime - lastTime >= interval) {
    lastTime = currentTime;
    checkMusicDonePlaying();
    DEBUG_MUSIC();
    setVolume();


    //myDFPlayer.volume(music_volume);
    //setVolume();
    //musicLoop();
    //Serial.println("[APP] Free memory: " + String(esp_get_free_heap_size()) + " bytes");
    //Serial.printf("RAW: %d\n",analogRead(POTENTIOMETER));
    //Serial.printf("1: %d\n2: %d\n3: %d\n==============\n", digitalRead(BUTTON_1), digitalRead(BUTTON_2), digitalRead(BUTTON_3));
  }

  //currentTime = millis();
  //if (currentTime - lastTime >= 1000) {
  //  myDFPlayer.volume(music_volume);
  //}
  



  if (Serial.available() > 0) {
  Serial.println("available");
  key = Serial.read();
  if (key == 'd') {
    Serial.println("received d");
    myDFPlayer.next();
  }
  else if (key == 'a') {
    Serial.println("received a");
    myDFPlayer.previous();
  }
  else if (key == 'p') {
    Serial.println("received p");
    myDFPlayer.pause();
  }
  else if (key == 'm' && (music_volume+1 < 22)) {
    Serial.println("received m");
    music_volume++;
    myDFPlayer.volume(music_volume);
  }
  else if (key == 'n' && (music_volume-1 > DEFAULT_VOLUME-1)) {
    Serial.println("received n");
    music_volume--;
    myDFPlayer.volume(music_volume);
  }
}
}
