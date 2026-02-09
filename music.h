#pragma once
#include "control.h"
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"

constexpr uint8_t DEFAULT_VOLUME =  15;
constexpr uint8_t MAX_VOLUME = 22;
constexpr uint8_t MIN_VOLUME = 0;
constexpr uint8_t PIN_MP3_TX = 17; // Connects to module's RX 
constexpr uint8_t PIN_MP3_RX = 16; // Connects to module's TX
constexpr uint8_t BUSY_PIN = 35;

extern SoftwareSerial softwareSerial;
extern DFRobotDFPlayerMini myDFPlayer;

extern int music_volume;
extern int last_pot_value;

enum MUSIC_BUTTONS {SHUFFLE, PREV, PAUSE_PLAY, NEXT, LOOP, LIST, CLOSE};
enum MUSIC_READ_STATE {STOPPED = 512, PLAYING = 513, PAUSED = 514, ERROR = -1};
void MUSIC_BUTTON_LOGIC(struct Button_struct* Button);
void DRAW_MUSIC_UI(void);
void DF_PLAYER_INIT();
void DEBUG_MUSIC();
int checkMusicDonePlaying();
void setCurrentPlayingMusic(bool incr);
void setVolume();
void musicLoop();
void trackMusicProgress();