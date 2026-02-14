#pragma once
#include "control.h"
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"

constexpr uint8_t DEFAULT_VOLUME =  8;
constexpr uint8_t MAX_VOLUME = 22;
constexpr uint8_t MIN_VOLUME = 0;
constexpr uint8_t PIN_MP3_TX = 17; // Connects to module's RX 
constexpr uint8_t PIN_MP3_RX = 16; // Connects to module's TX
constexpr uint8_t BUSY_PIN = 35;

extern SoftwareSerial softwareSerial;
extern DFRobotDFPlayerMini myDFPlayer;

extern int music_volume;
extern int last_volume_pot_value;
extern bool isPausedInternal;
extern int isDFPlayerFailed;

extern int playShuffleQueue[];
extern bool isShuffle;
extern int shuffleQueueCurrentIndex;
extern int shuffleQueueNextIndex;

enum MUSIC_BUTTONS {SHUFFLE, PREV, PAUSE_PLAY, NEXT, LOOP, LIST, EQ, CLOSE};
enum MUSIC_READ_STATE {STOPPED = 512, PLAYING = 513, PAUSED = 514, ERROR = -1};


void MUSIC_BUTTON_LOGIC(struct Button_struct* Button);
void DRAW_MUSIC_UI(void);
void DF_PLAYER_INIT();
void DEBUG_MUSIC();
int checkMusicDonePlaying();
void setCurrentPlayingMusic(bool incr);
void setVolume();
void musicProgressTimeHandle();

void drawMusicEQ();
void MUSIC_SCREEN_EQ_BUTTON_LOGIC(struct Button_struct* Button);
typedef enum {EQ_SAVE = 0, EQ_UP, EQ_DOWN, EQ_CLOSE, EQ_BTN_TOTAL} MUSIC_EQ_BUTTONS;
enum EQ {NORMAL_EQ, POP_EQ, ROCK_EQ, JAZZ_EQ, CLASSIC_EQ, BASS_EQ, TOTAL_EQ};

void drawMusicVisualizer();
void MUSIC_SCREEN_VISUALIZER_BUTTON_LOGIC(struct Button_struct* Button);

void drawMusicList(void);
void MUSIC_SCREEN_SONG_LIST_BUTTON_LOGIC(struct Button_struct* Button);