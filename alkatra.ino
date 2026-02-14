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
  analogSetPinAttenuation(32, ADC_11db); // 0–3.3V
}

static unsigned long lastTime = 0;
static const unsigned long interval = 500;
char key;


void loop() {
  DRAW_SCREEN();
  BUTTON_LOGIC();
  get_time();
  musicProgressTimeHandle();

  //captureWaveform();
  //drawWaveform();

  // Loop for blocking function
  unsigned long currentTime = millis();
  if (currentTime - lastTime >= interval) {
    lastTime = currentTime;
    checkMusicDonePlaying();
    //DEBUG_MUSIC();
    setVolume();
    Serial.println(digitalRead(BUTTON_6));
    //Serial.println(analogRead(DAC_PIN));
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
  
}
