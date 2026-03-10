#pragma once
#include <U8g2lib.h>


extern U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2;

enum screens {HOME, MENU, MUSIC, CALC, GAMES, CAL, ESPFETCH, FQGEN, BOOK, SETTINGS, KEYBOARD};
enum icon {MUSIC_ICO, CALC_ICO, GAMES_ICO, CAL_ICO, ESPFETCH_ICO, FQGEN_ICO, BOOK_ICO, SETTINGS_ICO};

enum MUSIC_SCREENS {MUSIC_SCREEN_SONG_LIST =  20, MUSIC_SCREEN_EQ, MUSIC_SCREEN_VISUALIZER, MUSIC_SCREEN_LYRICS};
enum GAME_SCREENS {SNAKE = 30, BRICBREK, TETR, AIRSTRIKE};

extern int current_scr;
extern int total_apps;
extern int app_marker_pos;
extern int app_index;
extern int prev_scr;

void DRAW_HOME_UI(void);
void DRAW_BOOT_LOGO(void);

void DRAW_HOME_UI_TIME(void);
void DRAW_SCREEN();
void DRAW_MENU();
void DRAW_NAVBAR(void);

void DRAW_ESPfetch(void);
void drawWifiMenu(void);
void WifiMenu_BUTTON_LOGIC(struct Button_struct* Button);