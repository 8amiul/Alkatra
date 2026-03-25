#include "display.h"
#include "control.h"
#include "functions.h"
#include "music.h"
#include <SPI.h>
#include "SoftwareSerial.h"
#include <WiFi.h>


void setup() {
  setCpuFrequencyMhz(160);

  Serial.begin(9600);
  softwareSerial.begin(9600);
  u8g2.begin();
  DRAW_BOOT_LOGO();
  DF_PLAYER_INIT();
  wifi_connect();
  date_allocation();
  BUTTON_SETUP();
  analogSetPinAttenuation(32, ADC_11db);
  
  //ledcSetup(0, 2000, 8);     // frequency, resolution
  ledcAttachPin(12, 0);

  xTaskCreatePinnedToCore(
    requestTask,
    "RequestTask",
    10000,
    NULL,
    1,
    &reqTaskHandle,
    0
  );

  /*
  for (int t = -50; t >= 50; t+= 0.5) {
        float freq = 440 + 200 * tan(t);
    //float val = pow(t, 2) - 1;
    //if (val < 0) val = 0; // Prevent NaN errors
    //float freq = 440 + sqrt(val);
    ledcWriteTone(0, abs(freq));
    
    t += 1;
    Serial.printf("t: %f, freq: %f\n", t, freq);
  }*/
}

static unsigned long lastTime = 0;
static const unsigned long interval = 500;
char key;

static unsigned long lastTime1 = 0;


int batteryPinReading = 0;


void loop() {
  DRAW_SCREEN();
  BUTTON_LOGIC();
  get_time();
  musicProgressTimeHandle();
  LyricsTimespanHandle();
  
  // Loop for blocking function
  unsigned long currentTime = millis();
  if (currentTime - lastTime >= interval) {
    lastTime = currentTime;
    checkMusicDonePlaying();
    setVolume();
    batteryPinReading = analogRead(BATTERY_VREAD_PIN);
    Serial.println(isDFPlayerFailed);



  }

  if (isGetReq) {
    isGetReq = false;
    urlGen();
    xTaskNotifyGive(reqTaskHandle);
  }



  
}
