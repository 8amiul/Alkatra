#include "display.h"
#include "control.h"
#include "functions.h"
#include "music.h"
#include <SPI.h>
#include "SoftwareSerial.h"
#include <WiFi.h>




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

  xTaskCreatePinnedToCore(
    requestTask,
    "RequestTask",
    10000,
    NULL,
    1,
    &reqTaskHandle,
    0
  );
}

static unsigned long lastTime = 0;
static const unsigned long interval = 500;
char key;

int batteryPinReading = 0;

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
    batteryPinReading = analogRead(BATTERY_VREAD_PIN);
  }

  if (isGetReq) {
    isGetReq = false;
    urlGen();
    xTaskNotifyGive(reqTaskHandle);
  }
}
