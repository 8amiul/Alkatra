#pragma once
#include <U8g2lib.h>


extern U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2;

enum screens {HOME, MENU, MUSIC, CALC, ALARM, CAL, NOTES, FQGEN, BOOK, LIGHT};
enum apps {MUSIC_APP, CALC_APP, ALARM_APP, CAL_APP, NOTES_APP, FQGEN_APP, BOOK_APP, LIGHT_APP};
extern int current_scr;
extern int total_apps;
extern int app_marker_pos;
extern int app_index;


void DRAW_HOME_UI(void);
void DRAW_BOOT_LOGO(void);

void DRAW_HOME_UI_TIME(void);
void DRAW_SCREEN();
void DRAW_MENU();
void DRAW_NAVBAR(void);