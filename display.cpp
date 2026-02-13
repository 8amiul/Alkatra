#include "display.h"
#include "bitmaps.h"
#include "functions.h"
#include "music.h"

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

int current_scr = 0;
int total_apps = 8;
int app_index = 0;
int app_marker_pos = 0;

void DRAW_SCREEN() {
    DRAW_NAVBAR();
    switch (current_scr) {
        case HOME: DRAW_HOME_UI(); break;
        case MENU: DRAW_MENU(); break;

        case MUSIC: if (isDFPlayerFailed != 1) DRAW_MUSIC_UI(); break;
            case MUSIC_SCREEN_EQ:
                drawMusicEQ();
            break;
            case MUSIC_SCREEN_SONG_LIST:
            break;
            case MUSIC_SCREEN_VISUALIZER:
                drawMusicVisualizer();
            break;

        default: DRAW_HOME_UI(); break;
    }
}

void DRAW_HOME_UI_TIME(void) {
    /* === time ===*/
    //u8g2.setFont(u8g2_font_haxrcorp4089_tr);
    u8g2.setFont(u8g2_font_timR14_tr);
    if (isTimeSet)
        u8g2.drawStr(5, 27, timeString);
    else if (isTimeSet == -1) {
        u8g2.drawStr(6, 28, "^_^");
    }
    else
        u8g2.drawStr(6, 25, "Syncing...");

    // sun & moon
    if (hr >= 5 && hr <= 17)
        u8g2.drawXBMP(83, 20, 48, 44, image_sun_bits);
    else 
        u8g2.drawXBMP(67, 15, 64, 64, image_moon_bits);

    /* === Day === */
    if (isTimeSet) {
        u8g2.setFont(u8g2_font_5x7_tr);
        u8g2.drawStr(5, 60, weekString);

        /* === Date === */
        u8g2.setFont(u8g2_font_4x6_tr);
        u8g2.drawStr(5, 51, dateString);
    }
}

void DRAW_NAVBAR(void) {
    // HEADPHONE
    u8g2.drawXBMP(100, 2, 9, 7, image_headphone_bits);
    // BATTERY
    // WIFI
    // BLUETOOTH
    // TIME
}

void DRAW_HOME_UI(void) {
    u8g2.clearBuffer();
    u8g2.setFontMode(1);
    u8g2.setBitmapMode(1);

    // window_border
    u8g2.drawFrame(0, 10, 128, 54);

    // Set the Home UI time, date, day and the sun/moon
    DRAW_HOME_UI_TIME();

    u8g2.sendBuffer();
    current_scr = HOME;
}

void DRAW_BOOT_LOGO(void) {
    u8g2.clearBuffer();
    u8g2.setFontMode(1);
    u8g2.setBitmapMode(1);
    /*
    // Layer 2
    u8g2.setFont(u8g2_font_timR24_tr);
    u8g2.drawStr(19, 44, "alkatra");

    // Layer 2
    u8g2.setFont(u8g2_font_timR10_tr);
    u8g2.drawStr(61, 56, "Console");
    */

    u8g2.drawXBMP(4, 10, 120, 44, image_boot_logo_bits);
    u8g2.sendBuffer();
}

void DRAW_MENU(void) {
    u8g2.clearBuffer();
    u8g2.setFontMode(1);
    u8g2.setBitmapMode(1);
    // window_border
    u8g2.drawFrame(0, 10, 128, 54);

    // music
    u8g2.drawXBMP(12, 17, 14, 16, image_music_bits);

    // calculator
    u8g2.drawXBMP(44, 17, 12, 16, image_calculator_bits);

    // clock_alarm
    u8g2.drawXBMP(72, 17, 15, 16, image_clock_alarm_bits);

    // calendar
    u8g2.drawXBMP(102, 17, 15, 16, image_calendar_bits);

    // flower
    u8g2.drawXBMP(12, 41, 16, 16, image_flower_bits);

    // music_sound_wave
    u8g2.drawXBMP(42, 41, 17, 16, image_music_sound_wave_bits);

    // light
    u8g2.drawXBMP(101, 42, 16, 16, image_light_bits);

    // phone_book_open
    u8g2.drawXBMP(71, 42, 17, 16, image_phone_book_open_bits);

    // select
    if (app_marker_pos < 4) {
        u8g2.drawXBMP(7 + (30 * app_marker_pos), 15, 24, 20, image_select_bits); // y=15 is constant value set for 1st row apps
    }
    else if (app_marker_pos >= 4 && app_marker_pos < 8) {
        u8g2.drawXBMP(7 + (30 * (app_marker_pos-4)), 39, 24, 20, image_select_bits); // y=39 us constant for 2nd row apps
    }
    u8g2.sendBuffer();
    current_scr = MENU;
}
