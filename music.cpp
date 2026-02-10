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

/* ------------------- DF Player mini initialization ----------- */
// Use pins 2 and 3 to communicate with DFPlayer Mini

SoftwareSerial softwareSerial(PIN_MP3_RX, PIN_MP3_TX);

DFRobotDFPlayerMini myDFPlayer;
int isDFPlayerFailed = 0;
int select_music_button_pos = 2;      // the default position where the selector would be is on the play/pause button
int total_buttons = 7;
bool isShuffle = 0;
bool isPaused = -1;
int currentPlayingMusic = 139;
int STATE = STOPPED;
bool isMusicDonePlaying = 0;
int music_volume = DEFAULT_VOLUME;

bool musicHasBeenPlayed = 0;

void musicLoop() {
  checkMusicDonePlaying();
  DEBUG_MUSIC();
  //setVolume();
}

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
    myDFPlayer.EQ(DFPLAYER_EQ_BASS);
    //myDFPlayer.enableLoopAll();
    //Serial.println(myDFPlayer.readFileCounts());
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




    // music_previous
    u8g2.drawXBMP(25, 51, 8, 8, image_music_previous_bits);

    // music_next
    u8g2.drawXBMP(57, 51, 8, 8, image_music_next_bits);

    // operation_undo
    u8g2.drawXBMP(73, 51, 6, 8, image_operation_undo_bits);

    // shuffle
    u8g2.drawXBMP(4, 50, 13, 11, image_shuffle_bits);


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
    setCurrentPlayingMusic(1);
    Serial.println("------------ DONE PLAYING --------------");
    myDFPlayer.next();
    
    while (digitalRead(BUSY_PIN) == HIGH);
    musicStartMillis = millis();
    //STATE = PLAYING;
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
  //Serial.printf("Current music: %d\n",myDFPlayer.readCurrentFileNumber());
}

void MUSIC_BUTTON_LOGIC(struct Button_struct* Button) {
  if (isDFPlayerFailed == 1) return;

  if (Button->btn1 == LOW) {
    select_music_button_pos--;
    if (select_music_button_pos <= -1)
      select_music_button_pos = 6;
    //delay(BUTTON_PRESS_DELAY);
  }

  if (Button->btn2 == LOW) {
    select_music_button_pos++;
    if (select_music_button_pos >= total_buttons) {
      select_music_button_pos = 0;
    }
    //delay(BUTTON_PRESS_DELAY);
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
        if (musicHasBeenPlayed == false || isDFPlayerFailed == 1) return;
        myDFPlayer.previous();
        setCurrentPlayingMusic(0);
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
        if (musicHasBeenPlayed == false || isDFPlayerFailed == 1) return;
        myDFPlayer.next();
        setCurrentPlayingMusic(1);
        STATE = PLAYING;
        while(digitalRead(BUSY_PIN) == HIGH);
        musicStartMillis = millis();
        isPausedInternal = false;
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
    //delay(BUTTON_PRESS_DELAY);
  }

}
