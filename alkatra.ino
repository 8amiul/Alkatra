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

  // Loop for blocking function
  unsigned long currentTime = millis();
  if (currentTime - lastTime >= interval) {
    lastTime = currentTime;
    checkMusicDonePlaying();
    setVolume();

  }

  
}
