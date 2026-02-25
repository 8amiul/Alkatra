#include "music.h"
#include "display.h"
#include "bitmaps.h"
#include "DFRobotDFPlayerMini.h"
#include <SPI.h>
#include "SoftwareSerial.h"
#include <Wire.h>
#include "songs_list.h"
#include "functions.h"
#include "control.h"
#include <math.h>
#include <ArduinoJson.h>
#include <UrlEncode.h>
#include <string>
#include <HTTPClient.h>
//#include <Arduino_JSON.h>

/* ------------------- DF Player mini initialization ----------- */
// Use pins 2 and 3 to communicate with DFPlayer Mini

SoftwareSerial softwareSerial(PIN_MP3_RX, PIN_MP3_TX);

DFRobotDFPlayerMini myDFPlayer;
int isDFPlayerFailed = 0;
int select_music_button_pos = 2;      // the default position where the selector would be is on the play/pause button
int TOTAL_MUSIC_BUTTONS = 8;

bool isPaused = -1;
int currentPlayingMusic = 1;          // Can't be zero
int STATE = STOPPED;
bool isMusicDonePlaying = 0;
int music_volume = DEFAULT_VOLUME;

bool musicHasBeenPlayed = 0;

bool isLoop = 0;

int playShuffleQueue[SONG_COUNT];
bool isShuffle = 0;
int shuffleQueueCurrentIndex = 0;
int shuffleQueueNextIndex = 0;


int currentEQ = NORMAL_EQ;

void DEBUG_MUSIC() {
  Serial.println("----- DEBUG -----!");
  //Serial.printf("State: %d\n",myDFPlayer.readState()); //read mp3 state
  Serial.printf("Current Music File: %d | STATE (VAR): %d | DONE: %d | BUSY_PIN: %d | VOL: %d | isPaused: %d | isFailed: %d\n", currentPlayingMusic, STATE, isMusicDonePlaying, digitalRead(BUSY_PIN), music_volume, isPausedInternal
  , isDFPlayerFailed);

  //Serial.println(myDFPlayer.readVolume()); //read current volume
  //Serial.printf("EQ: %d\n", myDFPlayer.readEQ()); //read EQ setting
  //Serial.println(myDFPlayer.readFileCounts()); //read all file counts in SD card
  //Serial.printf("Current music: %d\n",myDFPlayer.readCurrentFileNumber()); //read current play file number
  //Serial.println(myDFPlayer.readFileCountsInFolder(1)); //read file counts in folder SD:/03

}

void DF_PLAYER_INIT() {  
  pinMode(BUSY_PIN, INPUT);
  int initial_time = millis();

  while ((millis() - initial_time) <= 200) {
    if (digitalRead(BUSY_PIN) == LOW) {
      isDFPlayerFailed = 1;
      break;
    }
  }

  if (myDFPlayer.begin(softwareSerial, true, false)) {
    Serial.println("DF_PLAYER_SOFTWARE_SERIAL_OK");
    myDFPlayer.volume(music_volume);
    myDFPlayer.EQ(currentEQ);
    //myDFPlayer.enableLoopAll();

    for (int i = 0; i < SONG_COUNT; i++) 
      playShuffleQueue[i] = i + 1;

  } else {
    Serial.println("Connecting to DFPlayer Mini failed!");
    isDFPlayerFailed = 1;
  }
}

unsigned long musicStartMillis = 0;
unsigned long musicPausedMillis = 0;
unsigned long musicElapsedTime = 0;
bool isPausedInternal = false;


void musicProgressTimeHandle() {
  if (musicHasBeenPlayed && STATE == PLAYING && !isPausedInternal) {
      musicElapsedTime = (millis() - musicStartMillis) / 1000;
      //Serial.println(musicElapsedTime);
  }
}


// ------------------- Title, Album, Artist printing --------------------
unsigned long lastTime_setTrackTitle = 0;
const unsigned long interval_setTrackTitle = 500;

int titleBuffer_InitialPos = 0;
const int titleBufferSize = 15;
char titleBuffer[titleBufferSize+1];

void setTrackTitle() {

  strncpy(titleBuffer, songs[currentPlayingMusic-1].title + titleBuffer_InitialPos, titleBufferSize);
  titleBuffer[titleBufferSize] = '\0';

  unsigned long currentTime = millis();
  if (currentTime - lastTime_setTrackTitle >= interval_setTrackTitle) {
    lastTime_setTrackTitle = currentTime;

    titleBuffer_InitialPos++;
    //stop when there aren't enough chars left to fill the window
    if (titleBuffer_InitialPos > strlen(songs[currentPlayingMusic-1].title) - titleBufferSize) {
      titleBuffer_InitialPos = 0;
    }
  }

}

unsigned long lastTime_setTrackAlbum = 0;
const unsigned long interval_setTrackAlbum = 500;

int albumBuffer_InitialPos = 0;
const int albumBufferSize = 8;
char albumBuffer[albumBufferSize+1];

void setTrackAlbum() {

  strncpy(albumBuffer, songs[currentPlayingMusic-1].album + albumBuffer_InitialPos, albumBufferSize);
  albumBuffer[albumBufferSize] = '\0';

  unsigned long currentTime = millis();
  if (currentTime - lastTime_setTrackAlbum >= interval_setTrackAlbum) {
    lastTime_setTrackAlbum = currentTime;

    albumBuffer_InitialPos++;
    //stop when there aren't enough chars left to fill the window
    if (albumBuffer_InitialPos > strlen(songs[currentPlayingMusic-1].album) - albumBufferSize) {
      albumBuffer_InitialPos = 0;
    }
  }
}

unsigned long lastTime_setTrackArtist = 0;
const unsigned long interval_setTrackArtist = 500;

int artistBuffer_InitialPos = 0;
const int artistBufferSize = 8;
char artistBuffer[artistBufferSize+1];

void setTrackArtist() {
  strncpy(artistBuffer, songs[currentPlayingMusic-1].artist + artistBuffer_InitialPos, artistBufferSize);
  artistBuffer[artistBufferSize] = '\0';

  unsigned long currentTime = millis();
  if (currentTime - lastTime_setTrackArtist >= interval_setTrackArtist) {
    lastTime_setTrackArtist = currentTime;

    artistBuffer_InitialPos++;
    //stop when there aren't enough chars left to fill the window
    if (artistBuffer_InitialPos > strlen(songs[currentPlayingMusic-1].artist) - artistBufferSize) {
      artistBuffer_InitialPos = 0;
    }
  }
}

// ------------------------------------------------------------------



// --------------------- Volume section -----------------------------
/*
void setVolume() {
  int new_pot_value = analogRead(POTENTIOMETER_PIN);
  if (abs(new_pot_value - last_volume_pot_value) > 250) {  // dead zone
    last_volume_pot_value = new_pot_value;
    int map_volume = map(new_pot_value, 0, 4095, MIN_VOLUME, MAX_VOLUME);
    
    if (music_volume != map_volume) {
      music_volume = map_volume;
      //myDFPlayer.volume(music_volume);
    }
    Serial.printf("music_vol: %d, map_vol: %d\n", music_volume, map_volume);
  }
  
}
*/

void setVolume() {
  static unsigned long lastRead = 0;
  if (millis() - lastRead < 500) return; 
  lastRead = millis();

  int new_pot_value = analogRead(POTENTIOMETER_PIN);

  if (abs(new_pot_value - last_volume_pot_value) > 250) {
    last_volume_pot_value = new_pot_value;

    int map_volume = map(new_pot_value, 0, 4095, MIN_VOLUME, MAX_VOLUME);

    if (music_volume != map_volume) {
      music_volume = constrain(map_volume, MIN_VOLUME, MAX_VOLUME);
      myDFPlayer.volume(music_volume);
    }

    Serial.printf("music_vol: %d\n", music_volume);
  }
}

// ------------------------------------------------------------------


void DRAW_MUSIC_UI(void) {
    u8g2.clearBuffer();
    u8g2.setFontMode(1);
    u8g2.setBitmapMode(1);
    // window_border
    u8g2.drawFrame(0, 10, 128, 54);

    
    // music progress time
    u8g2.setDrawColor(1);
    u8g2.setFont(u8g2_font_NokiaSmallPlain_tf);
    if (musicElapsedTime >= 3600) {
      char timeBuf[9];
      sToHMSstring(musicElapsedTime, timeBuf, sizeof(timeBuf));
      u8g2.drawStr(82, 47, timeBuf);
    }
    else {
      char timeBuf[6];
      sToMSstring(musicElapsedTime, timeBuf, sizeof(timeBuf));
      u8g2.drawStr(82, 47, timeBuf);
    }
    

    // Track title
    u8g2.setFont(u8g2_font_timR10_tr);
    if (strlen(songs[currentPlayingMusic-1].title) <= titleBufferSize) {
      u8g2.drawStr(3, 23, songs[currentPlayingMusic-1].title);
    }
    else {
      setTrackTitle();
      u8g2.drawStr(3, 23, titleBuffer);
    }

    // Track artist
    u8g2.setFont(u8g2_font_4x6_tr);
    if (strlen(songs[currentPlayingMusic-1].artist) <= artistBufferSize) {
      u8g2.drawStr(4, 41, songs[currentPlayingMusic-1].artist);
    }
    else {
      setTrackArtist();
      u8g2.drawStr(4, 41, artistBuffer);
    }

    // Track album
    u8g2.setFont(u8g2_font_6x10_tr);
    if (strlen(songs[currentPlayingMusic-1].album) <= albumBufferSize) {
      u8g2.drawStr(4, 33, songs[currentPlayingMusic-1].album);
    }
    else {
      setTrackAlbum();
      u8g2.drawStr(4, 33, albumBuffer);
    }

    
    // music_play
    if (myDFPlayer.readState() != PLAYING) {
      u8g2.drawXBMP(42, 51, 8, 8, image_music_play_bits);
    }
    else {
    // music_pause
      u8g2.setDrawColor(2);
      u8g2.drawXBMP(42, 51, 6, 8, image_music_pause_bits);
    }


    // settings
    u8g2.setDrawColor(2);
    u8g2.drawXBMP(118, 24, 7, 9, image_settings_bits);

    // music_previous
    u8g2.drawXBMP(25, 51, 8, 8, image_music_previous_bits);

    // music_next
    u8g2.drawXBMP(57, 51, 8, 8, image_music_next_bits);

    // loop
    u8g2.drawXBMP(73, 51, 6, 8, image_operation_undo_bits);
    // loop_on bar
    if (isLoop == 1)
      u8g2.drawFrame(69, 61, 14, 2);



    // shuffle
    u8g2.drawXBMP(4, 50, 13, 11, image_shuffle_bits);
    // shuffle_on bar
    if (isShuffle == 1) {
      u8g2.drawFrame(3, 61, 15, 2);
    }

    // progress-bar
    int cappedMusicElapsedTime = constrain(musicElapsedTime, 0, songs[currentPlayingMusic-1].length);
    int MusicBarPosX2 = map(cappedMusicElapsedTime, 0, songs[currentPlayingMusic-1].length, 5, 76);
    u8g2.drawLine(5, 46, MusicBarPosX2, 46);

    // progress-ending-point
    u8g2.drawXBMP(77, 42, 2, 5, image_progress_ending_point_bits);

    // progress-starting-point
    u8g2.drawXBMP(3, 44, 5, 5, image_progress_starting_point_bits);



    // volume-bar
    int volume_bar_x2_pos_min = 101;
    int volume_bar_x2_pos_max = 121;
    //u8g2.drawLine(101, 54, 101 + round(((volume_bar_x2_pos_max - volume_bar_x2_pos_min) / MAX_VOLUME) * music_volume), 54);
    int x2_pos = map(music_volume, MIN_VOLUME, MAX_VOLUME, volume_bar_x2_pos_min, volume_bar_x2_pos_max);
    u8g2.drawLine(101, 54, x2_pos, 54);

    // volume-starting-point
    u8g2.drawXBMP(99, 52, 5, 5, image_progress_starting_point_bits);

    // volume-ending-point
    u8g2.drawXBMP(122, 51, 2, 4, image_volume_ending_point_bits);

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
      case EQ:
        u8g2.drawBox(115, 23, 12, 11);
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

void handleShuffle() {
  if (shuffleQueueCurrentIndex == 0 && shuffleQueueNextIndex == 0) {
    myDFPlayer.play(playShuffleQueue[shuffleQueueCurrentIndex]);
    shuffleQueueNextIndex++;
    currentPlayingMusic = playShuffleQueue[shuffleQueueCurrentIndex];
    //Serial.printf("Playing Music: %d\n", playShuffleQueue[shuffleQueueCurrentIndex]);
  }
  else { 
    shuffleQueueCurrentIndex = shuffleQueueNextIndex;
    myDFPlayer.play(playShuffleQueue[shuffleQueueCurrentIndex]);
    //Serial.printf("Playing Music: %d | Next music: %d\n", playShuffleQueue[shuffleQueueCurrentIndex], playShuffleQueue[shuffleQueueCurrentIndex+1]);
    currentPlayingMusic = playShuffleQueue[shuffleQueueCurrentIndex];
    if (shuffleQueueNextIndex+1 >= SONG_COUNT) {
      shuffle(playShuffleQueue, SONG_COUNT);
      shuffleQueueCurrentIndex = 0;
      shuffleQueueNextIndex = 0;
    }
    else {
        shuffleQueueNextIndex++;
    }
  }
}


int checkMusicDonePlaying() {
  int busy_pin = digitalRead(BUSY_PIN);

  if (busy_pin == HIGH && STATE == PLAYING) {
    isMusicDonePlaying = 1;

    if (isLoop == 1) {
      myDFPlayer.play(currentPlayingMusic);
    }
    else {
      if (isShuffle != 1) {
        setCurrentPlayingMusic(1);
        myDFPlayer.next();
      }
      else if (isShuffle == 1) {
        handleShuffle();
      }
    }

    while (digitalRead(BUSY_PIN) == HIGH);
    musicStartMillis = millis();
  }
  else {
    isMusicDonePlaying = 0;
  }
  return STATE;
}

void setCurrentPlayingMusic(bool incr) {
  if (incr) {
    if (currentPlayingMusic == SONG_COUNT)
      currentPlayingMusic = 1;
    else
      currentPlayingMusic++;
  }
  else {
    if (currentPlayingMusic == 1)
      currentPlayingMusic = SONG_COUNT;
    else
      currentPlayingMusic--;
  }
}

void MUSIC_BUTTON_LOGIC(struct Button_struct* Button) {
  if (isDFPlayerFailed == 1) return;

  if (Button->btn1 == LOW) {
    select_music_button_pos--;
    if (select_music_button_pos <= -1)
      select_music_button_pos = TOTAL_MUSIC_BUTTONS-1;
    //delay(BUTTON_PRESS_DELAY);
  }

  if (Button->btn2 == LOW) {
    select_music_button_pos++;
    if (select_music_button_pos >= TOTAL_MUSIC_BUTTONS) {
      select_music_button_pos = 0;
    }
  }

  if (Button->btn3 == LOW) {
    switch (select_music_button_pos) {
      case SHUFFLE:
        if (isShuffle == 0) {
          shuffle(playShuffleQueue, SONG_COUNT);
          shuffleQueueCurrentIndex = 0;
          shuffleQueueNextIndex = 0;
          isShuffle = 1;
        }
        else {
          isShuffle = 0;
        }
        break;
      case PREV:
        if (musicHasBeenPlayed == false || isDFPlayerFailed == 1) return;
        isLoop = 0;
        if (isShuffle != 1) {
          myDFPlayer.previous();
          setCurrentPlayingMusic(0);
        }
        else if (isShuffle == 1) {
          handleShuffle();
        }

        STATE = PLAYING;
        while(digitalRead(BUSY_PIN) == HIGH);
        musicStartMillis = millis();
        isPausedInternal = false;
        break;
      case PAUSE_PLAY:
        if (isDFPlayerFailed == 1) return;
        if (STATE == STOPPED) {
          musicHasBeenPlayed = 1;
          myDFPlayer.play(currentPlayingMusic);
          
          while (digitalRead(BUSY_PIN) == HIGH);
          musicStartMillis = millis();
          STATE = PLAYING;
        }
        else if (STATE == PAUSED) {
          myDFPlayer.start();
          STATE = PLAYING;
          isPausedInternal = false;

          // shift start time forward by pause duration
          musicStartMillis += (millis() - musicPausedMillis);      
        }
        else if (STATE == PLAYING) {
          myDFPlayer.pause();
          STATE = PAUSED;
          isPausedInternal = true;

          // store elapsed time so far
          musicPausedMillis = millis();
        }
        break;
      case NEXT:
        isLoop = 0;
        if (musicHasBeenPlayed == false || isDFPlayerFailed == 1) return;
        if (isShuffle != 1) {
          myDFPlayer.next();
          setCurrentPlayingMusic(1);
        }
        else {
          handleShuffle();
        }

        STATE = PLAYING;
        while(digitalRead(BUSY_PIN) == HIGH);
        musicStartMillis = millis();
        isPausedInternal = false;
        break;

      case LOOP:
         if (isLoop == 1) {
          isLoop = 0;
         }
         else {
          isLoop = 1;
         }
        break;

      case LIST:
        current_scr = MUSIC_SCREEN_SONG_LIST;
        break;
      case EQ:
         current_scr = MUSIC_SCREEN_EQ;
         break;
      case CLOSE:
        current_scr = MENU;
        break;
      default:
        break;
    }
    //delay(BUTTON_PRESS_DELAY);
  }

  if (Button->btn4 == LOW) {
    return;
  }

  if (Button->btn5 == LOW) {
    current_scr = MUSIC_SCREEN_VISUALIZER;
  }
}




//========== EQ Music ==========
int MusicEQ_ButtonPos = EQ_SAVE;
int MusicEQ_EQcount = 0;

void drawMusicEQ(void) {
    u8g2.clearBuffer();
    u8g2.setFontMode(1);
    u8g2.setBitmapMode(1);

    // window_border
    u8g2.setDrawColor(2);
    u8g2.drawFrame(0, 10, 128, 54);

    // top-left text
    u8g2.setDrawColor(1);
    u8g2.setFont(u8g2_font_timR24_tr);
    u8g2.drawStr(2, 38, "E");
    u8g2.setFont(u8g2_font_timR24_tr);
    u8g2.drawStr(9, 53, "Q");

    if (MusicEQ_ButtonPos == EQ_DOWN)
      // down_hvr
      u8g2.drawXBMP(103, 44, 13, 14, image_down_hvr_bits);
    else 
      // down
      u8g2.drawXBMP(103, 43, 13, 14, image_down_bits);

    if (MusicEQ_ButtonPos == EQ_CLOSE)
      // close_hvr
      u8g2.drawXBMP(115, 11, 12, 8, image_close_hvr_bits);
    else 
      // close
      u8g2.drawXBMP(115, 11, 12, 8, image_close_bits);

    // Up_hvr
    if (MusicEQ_ButtonPos == EQ_UP)
      u8g2.drawXBMP(103, 23, 13, 14, image_Up_hvr_bits);
    else
      // Up
      u8g2.drawXBMP(103, 23, 13, 14, image_Up_bits);

    if (MusicEQ_ButtonPos == EQ_SAVE)
      // save_button_hvr
      u8g2.drawXBMP(51, 47, 24, 11, image_save_button_hvr_bits);
    else
      // save_button
      u8g2.drawXBMP(51, 47, 24, 11, image_save_button_1_bits);

    // eq_name_border
    u8g2.drawXBMP(28, 24, 65, 18, image_eq_name_border_bits);

    // EQ_name
    u8g2.setFont(u8g2_font_helvB08_tr);
    switch (MusicEQ_EQcount) {
      case NORMAL_EQ:
        u8g2.drawStr(38, 36, "NORMAL");
        break;
      case POP_EQ:
        u8g2.drawStr(51, 36, "POP");  
        break;
      case ROCK_EQ:
        u8g2.drawStr(45, 36, "ROCK");
        break;
      case JAZZ_EQ:
        u8g2.drawStr(47, 36, "JAZZ");
        break;
      case CLASSIC_EQ:
        u8g2.drawStr(38, 36, "CLASSIC");
        break;
      case BASS_EQ:
        u8g2.drawStr(48, 36, "BASS");
        break;
      default:
        u8g2.drawStr(48, 36, "ERROR");
        break;
    }

    u8g2.sendBuffer();
}

void MUSIC_SCREEN_EQ_BUTTON_LOGIC(struct Button_struct* Button) {
  if (Button->btn4 == LOW) {
    if (MusicEQ_ButtonPos == EQ_SAVE)
      MusicEQ_ButtonPos = EQ_BTN_TOTAL;
    MusicEQ_ButtonPos--;
  }
  
  if (Button->btn5 == LOW) {
    MusicEQ_ButtonPos++;
    if (MusicEQ_ButtonPos >= EQ_BTN_TOTAL)
      MusicEQ_ButtonPos = EQ_SAVE;
  }

  if (Button-> btn3 == LOW) {
    switch (MusicEQ_ButtonPos) {
      case EQ_SAVE:
        currentEQ = MusicEQ_EQcount;
        myDFPlayer.EQ(currentEQ);
        break;
      case EQ_UP:
        if (MusicEQ_EQcount == NORMAL_EQ)
          MusicEQ_EQcount = TOTAL_EQ - 1;
        MusicEQ_EQcount--;
        break;

      case EQ_DOWN:
        MusicEQ_EQcount++;
        if (MusicEQ_EQcount >= TOTAL_EQ)
          MusicEQ_EQcount = NORMAL_EQ;
        break;

      case EQ_CLOSE:
        current_scr = MUSIC;
      break;

      default: break;
    }
  }
}
// ===================================



//========== Wave visualizer ==========
#define SAMPLES 128
#define SAMPLE_DELAY  80
#define ADC_PIN 32
#define CENTER 1950
//#define GAIN 5
#define GAIN 4

unsigned long lastSampleTime = 0;
int sampleIndex = 0;
int samples[SAMPLES];

#define DEADZONE 20


void captureWaveform() {
  for (int i = 0; i < SAMPLES; i++) {
    int raw_adc = analogRead(ADC_PIN);
    // center around 0
    int centered = raw_adc - CENTER;          // CENTER = ADC_BIAS VOLT 1.67 in analogValue.
                                              // We use this center to make AC signals. ESP32 ADC
                                              // can't read negative volt. We create a bias so that
                                              // the negative volts get positve by substracting the bias
                                              // thus we can constrain the AC signal in 0 to 4095
                                              // typically the positve value goes above (4095/2) and negative
                                              // stays below that value. thus we get a center (4095/2) ~ 2048
                                              // It can also be called the idle volts reading from ADC. 1950 is
                                              // in my case.

    if (abs(centered) < DEADZONE) {
      centered = 0;
    }

    // software gain
    centered *= GAIN;
    // shift back
    int amplified = centered + CENTER;
    // clamp
    samples[i] = constrain(amplified, 0, 4095);

    delayMicroseconds(SAMPLE_DELAY);
  }
}


void drawMusicVisualizer() {
  captureWaveform();
  u8g2.clearBuffer();
  u8g2.setDrawColor(1);
  for (int x = 1; x < SAMPLES; x++) {
    int y1 =  map(samples[x - 1], 0, 4095, 63, 0);
    int y2 = map(samples[x], 0, 4095, 63, 0);
  
    u8g2.drawLine(x-1, y1, x, y2);
  }
  u8g2.sendBuffer();
}

void MUSIC_SCREEN_VISUALIZER_BUTTON_LOGIC(struct Button_struct* Button) {
  if (Button->btn4 == LOW) {
    current_scr = MUSIC;
  }
}
// ===================================




//========== Music List ==========
int songsToShow = 4;
int yToAdd = 13;
int MusicListInitialY = 21;

int MusicList_MusicIndex = 0;
int MusicList_HighlightIndex = 0;

int songsToShow_initialPos = 0;
int songsToShow_finalPos = songsToShow;

int SCROLL_SIZE = 10;
int MusicList_PageCount = 1;
int MusicList_MaxPage = ceil(SONG_COUNT / songsToShow);

void drawMusicList(void) {
    u8g2.clearBuffer();
    u8g2.setFontMode(1);
    u8g2.setBitmapMode(1);
    // window-border
    u8g2.drawFrame(0, 10, 128, 54);


    // Layer 2
    u8g2.setFont(u8g2_font_NokiaSmallPlain_tf);

    size_t MAX_LIST_TITLE_SIZE = 20+3;   // Additional 3 for three dots ...
    char listTitleBuff[MAX_LIST_TITLE_SIZE+1];
    for (int i = songsToShow_initialPos, j = 0; i < songsToShow_finalPos, j < songsToShow; i++, j++) {
      
      if (strlen(songs[i].title) > 20) {                                                    // Limiting text overflow by using this logic
        strncpy(listTitleBuff, songs[i].title, MAX_LIST_TITLE_SIZE);
        
        listTitleBuff[20] = '.'; listTitleBuff[21] = '.'; listTitleBuff[22] = '.';listTitleBuff[23] = '\0';

        u8g2.drawStr(3, MusicListInitialY+(yToAdd * j), listTitleBuff);
      }
      else 
        u8g2.drawStr(3, MusicListInitialY+(yToAdd * j), songs[i].title);
    }

    // Select
    u8g2.setDrawColor(2);
    u8g2.drawBox(1, 11 + (MusicList_HighlightIndex*13), 118, 13);  // add 13 to Y axis for each index to move 


    // scroll
    //u8g2.drawBox(121, constrain(map(MusicList_PageCount, 1, MusicList_MaxPage, 18, 47), 18, 47), 5, SCROLL_SIZE);
    u8g2.drawBox(121, constrain(map(songsToShow_finalPos, songsToShow, SONG_COUNT, 18, 46), 18, 46), 5, SCROLL_SIZE);

    // scrollbar-border
    u8g2.setDrawColor(1);
    u8g2.drawXBMP(119, 10, 9, 54, image_scroll_border_bits);


    u8g2.sendBuffer();
}

void MUSIC_SCREEN_SONG_LIST_BUTTON_LOGIC(struct Button_struct* Button) {
  if (Button->btn1 == LOW) {
    if (songsToShow_initialPos <= 0) {
      songsToShow_initialPos = SONG_COUNT - 4;
      songsToShow_finalPos = SONG_COUNT;
      MusicList_HighlightIndex = 0;
      MusicList_MusicIndex = songsToShow_initialPos;
    }
    else {
      songsToShow_initialPos -= songsToShow;
      songsToShow_finalPos -= songsToShow;
      MusicList_MusicIndex = songsToShow_initialPos + MusicList_HighlightIndex;
    }

        Serial.printf("MusicList_MusicIndex: %d | MusicList_HighlightIndex: %d | songsToShow_initialPos: %d | songsToShow_finalPos: %d | MusicList_PageCount: %d | CurrentPlayingMusic: %d\n", MusicList_MusicIndex, MusicList_HighlightIndex, songsToShow_initialPos, songsToShow_finalPos, MusicList_PageCount, currentPlayingMusic);

  }

  if (Button->btn2 == LOW) {
    if (songsToShow_finalPos + songsToShow > SONG_COUNT) {
      songsToShow_initialPos = 0;
      songsToShow_finalPos = songsToShow;
      MusicList_HighlightIndex = 0;
      MusicList_MusicIndex = 0;
    }
    else {
      songsToShow_initialPos += songsToShow;
      songsToShow_finalPos += songsToShow;  
      MusicList_MusicIndex = songsToShow_initialPos + MusicList_HighlightIndex;    
    }
        Serial.printf("MusicList_MusicIndex: %d | MusicList_HighlightIndex: %d | songsToShow_initialPos: %d | songsToShow_finalPos: %d | MusicList_PageCount: %d | CurrentPlayingMusic: %d\n", MusicList_MusicIndex, MusicList_HighlightIndex, songsToShow_initialPos, songsToShow_finalPos, MusicList_PageCount, currentPlayingMusic);
  }

  if (Button->btn3 == LOW) {

    currentPlayingMusic = MusicList_MusicIndex+1;
    myDFPlayer.play(currentPlayingMusic);

    musicHasBeenPlayed = 1;
    isPausedInternal = false;
    STATE = PLAYING;

    while (digitalRead(BUSY_PIN) == HIGH);
    musicStartMillis = millis();

    if (isShuffle == 1) {
      shuffle(playShuffleQueue, SONG_COUNT);
      shuffleQueueCurrentIndex = 0;
      shuffleQueueNextIndex = 0;
    }

  }

  if (Button->btn4 == LOW) {
    if (MusicList_HighlightIndex == songsToShow-1) {
      if (MusicList_MusicIndex == SONG_COUNT-1) {
        MusicList_HighlightIndex = 0;
        MusicList_MusicIndex = 0;
        songsToShow_initialPos = 0;
        songsToShow_finalPos = songsToShow;
      }
      else {
        songsToShow_finalPos++;
        songsToShow_initialPos++;
        MusicList_MusicIndex++;
      }
    }
    else {
      MusicList_HighlightIndex++;
      MusicList_MusicIndex++;
    }
        Serial.printf("MusicList_MusicIndex: %d | MusicList_HighlightIndex: %d | songsToShow_initialPos: %d | songsToShow_finalPos: %d | MusicList_PageCount: %d | CurrentPlayingMusic: %d\n", MusicList_MusicIndex, MusicList_HighlightIndex, songsToShow_initialPos, songsToShow_finalPos, MusicList_PageCount, currentPlayingMusic);
  }

  if (Button->btn5 == LOW) {
    if (MusicList_HighlightIndex == 0) {
      if (MusicList_MusicIndex == 0) {
        MusicList_HighlightIndex = songsToShow-1;
        MusicList_MusicIndex = SONG_COUNT-1;
        songsToShow_initialPos = SONG_COUNT-songsToShow;
        songsToShow_finalPos = SONG_COUNT;
      }
      else {
        songsToShow_finalPos--;
        songsToShow_initialPos--;
        MusicList_MusicIndex--;

      }
    }
    else {
      MusicList_HighlightIndex--;
      MusicList_MusicIndex--;  
    }
        Serial.printf("MusicList_MusicIndex: %d | MusicList_HighlightIndex: %d | songsToShow_initialPos: %d | songsToShow_finalPos: %d | MusicList_PageCount: %d | CurrentPlayingMusic: %d\n", MusicList_MusicIndex, MusicList_HighlightIndex, songsToShow_initialPos, songsToShow_finalPos, MusicList_PageCount, currentPlayingMusic);
  }

  if (Button->btn6 == LOW) {
    current_scr = MUSIC;
    MusicList_MusicIndex = 0;
    MusicList_HighlightIndex = 0;

    songsToShow_initialPos = 0;
    songsToShow_finalPos = songsToShow;
  }

}
// ===================================
