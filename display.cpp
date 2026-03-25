#include "display.h"
#include "bitmaps.h"
#include "functions.h"
#include "music.h"
#include "game.h"
#include "keyboard.h"
#include "wifi_manager.h"
#include "graph.h"

//#include "snake.h"

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

int current_scr = 0;
int prev_scr = 0;
int total_apps = 8;
int app_index = 0;
int app_marker_pos = 0;

void DRAW_SCREEN() {
    switch (current_scr) {
        case HOME: DRAW_HOME_UI(); break;
        case MENU: DRAW_MENU(); break;

        case MUSIC: if (isDFPlayerFailed != 1) DRAW_MUSIC_UI(); break;
            case MUSIC_SCREEN_EQ:
                drawMusicEQ();
            break;
            case MUSIC_SCREEN_SONG_LIST:
                drawMusicList();
            break;
            case MUSIC_SCREEN_VISUALIZER:
                drawMusicVisualizer();
            break;
            case MUSIC_SCREEN_LYRICS:
                drawMusicLyrics();
            break;

        case GAMES: DRAW_GAME_MENU(); break;
            case SNAKE:
                drawSnakeGame();
            break;
        
        case ESPFETCH: DRAW_ESPfetch(); break;
        case WIFI: drawWifiMenu(); break;
            case WIFI_CONNECTED:
                drawConnectedWifi();
            break;
        case KEYBOARD: drawKeyboard(); break;

        case EQnGEN: drawEQnGEN(); break;

        default: DRAW_HOME_UI(); break;
    }
    
}

void DRAW_HOME_UI_TIME(void) {
    /* === time ===*/
    //u8g2.setFont(u8g2_font_haxrcorp4089_tr);
    u8g2.setFont(u8g2_font_NokiaSmallPlain_tf);
    if (isTimeSet)
        u8g2.drawStr(5, 27, timeString);
    else
        u8g2.drawStr(7, 26, "Syncing...");
    

    if (isTimeSet == -1)
        u8g2.drawStr(7, 26, ":(");

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
//String string_to_print = String(batteryPinReading);


void DRAW_NAVBAR(void) {
    
    // HEADPHONE
    if (isDFPlayerFailed == 0)
        u8g2.drawXBMP(90, 2, 9, 7, image_headphone_bits);
            
    // BATTERY
        // battery-charge
        uint8_t battery_charge_x = 108;
        battery_voltage = (batteryPinReading / MAX_PIN_READING) * MAX_PIN_VOLT * BATTERY_VOLT_SMOOTHING_FACTOR * 2;    
        u8g2.setDrawColor(1);

        setBatteryVoltage();

        /*
        char buff[10];
        dtostrf(battery_voltage, 0, 2, buff);
        buff[strlen(buff)] = 'V';
        buff[strlen(buff)+1] = '\0';
        u8g2.setFont(u8g2_font_NokiaSmallPlain_tf);
        u8g2.drawStr(2, 9, buff); */

        char buff[10];
        snprintf(buff, sizeof(buff), "%.2fV", battery_voltage);
        u8g2.setFont(u8g2_font_NokiaSmallPlain_tf);
        u8g2.drawStr(2, 9, buff);

        if (battery_voltage >= 4.2) battery_charge_x = 108;
        else if (battery_voltage < 4.2 && battery_voltage >= 3.95) battery_charge_x = 108;
        else if (battery_voltage < 3.95 && battery_voltage >= 3.70) battery_charge_x = 116;
        else if (battery_voltage < 3.70 && battery_voltage >= 3.50) battery_charge_x = 120;
        else if (battery_voltage < 3.50 && battery_voltage >= 3.25) battery_charge_x = 124;
        else if (battery_voltage < 3.25) battery_charge_x = 127;

        u8g2.drawBox(battery_charge_x, 3, 20, 5);

        // battery-border
        u8g2.drawXBMP(105, 2, 23, 7, image_battery_border_bits);

    // WIFI
    // BLUETOOTH
    // TIME

}

void DRAW_HOME_UI(void) {
    u8g2.clearBuffer();
    DRAW_NAVBAR();
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

void DRAW_ESPfetch(void) {
    u8g2.clearBuffer();
    DRAW_NAVBAR();
    u8g2.setFontMode(1);
    u8g2.setBitmapMode(1);
    // window_border
    u8g2.drawFrame(0, 10, 128, 54);

    // New_Project
    u8g2.drawXBMP(78, -5, 64, 64, image_esp32_bits);

    // Layer 7
    u8g2.setFont(u8g2_font_profont10_tr);
    u8g2.drawStr(4, 19, "8amiul@alkatra");

    // line
    u8g2.drawLine(4, 21, 62, 21);

    // Layer 9
    u8g2.setFont(u8g2_font_4x6_tr);
    u8g2.drawStr(4, 30, ESP.getChipModel());

    // Layer 10
    u8g2.drawStr(4, 38, "2 Core | 400 MHz");

    // Layer 11
    char heapString[100];
    sprintf(heapString, "Free Heap: %d/%d KB", ESP.getFreeHeap(), ESP.getHeapSize());
    u8g2.drawStr(4, 45, heapString);

    // Layer 12
    char uptimeString[100];
    unsigned long sec = millis() / 1000UL;
    unsigned long h = sec / 3600;
    unsigned long m = (sec % 3600) / 60;
    unsigned long s = sec % 60;

    if (h)
        snprintf(uptimeString, 100, "uptime: %02lu hour, %02lu mins", h, m);
    else
        snprintf(uptimeString, 100, "uptime: %02lu mins", m);
    u8g2.drawStr(4, 52, uptimeString);

    u8g2.sendBuffer();
}

void DRAW_MENU(void) {
    u8g2.clearBuffer();
    DRAW_NAVBAR();
    u8g2.setFontMode(1);
    u8g2.setBitmapMode(1);
    // window_border
    u8g2.drawFrame(0, 10, 128, 54);

    // music
    u8g2.drawXBMP(12, 17, 14, 16, image_music_bits);

    // calculator
    u8g2.drawXBMP(44, 17, 12, 16, image_calculator_bits);

    // games
    u8g2.drawXBMP(73, 18, 15, 12, image_games_bits);

    // calendar
    u8g2.drawXBMP(102, 17, 15, 16, image_calendar_bits);

    // flower
    u8g2.drawXBMP(12, 41, 16, 16, image_flower_bits);

    // music_sound_wave
    u8g2.drawXBMP(42, 41, 17, 16, image_music_sound_wave_bits);

    // menu_WIFI_gear
    u8g2.drawXBMP(101, 42, 16, 16, image_menu_settings_gear_bits);

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

void drawCenteredStr(int y, const char *str) {
  // 1. Get the width of the display in pixels
  int displayWidth = u8g2.getDisplayWidth();
  
  // 2. Calculate the pixel width of the specific string
  int strWidth = u8g2.getStrWidth(str);
  
  // 3. Calculate the X position to center it
  int x = (displayWidth - strWidth) / 2;
  
  // 4. Draw the string at the calculated coordinates
  u8g2.drawStr(x, y, str);
}