#include "display.h"
#include "bitmaps.h"
#include "functions.h"
#include "music.h"
#include "game.h"
#include <WiFi.h>
#include "keyboard.h"
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
        case SETTINGS: drawWifiMenu(); break;
        case KEYBOARD: drawKeyboard(); break;

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

        char buff[10];
        dtostrf(battery_voltage, 4, 2, buff);
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

    // menu_settings_gear
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

int ssidToShow = 4;

int WifiMenuYtoAdd = 13;
int SSIDListInitialY = 21;

int WifiMenu_MusicIndex = 0;
int WifiMenu_HighlightIndex = 0;

int ssidToShow_initialPos = 0;
int ssidToShow_finalPos = ssidToShow;

int WifiMenuSCROLL_SIZE = 10;
int WifiMenu_PageCount = 1;
int WifiMenu_MaxPage = ceil(SSID_COUNT / ssidToShow);
unsigned long  Wifiprev = 0;
unsigned long WifiInterval = 1000;

void drawWifiMenu(void) {
    u8g2.clearBuffer();
    DRAW_NAVBAR();
    u8g2.setFontMode(1);
    u8g2.setBitmapMode(1);
    // window-border
    u8g2.drawFrame(0, 10, 128, 54);


    // Layer 2
    u8g2.setFont(u8g2_font_NokiaSmallPlain_tf);

    size_t MAX_SSID_LENGTH = 20+3;   // Additional 3 for three dots ...
    char listSSIDBuff[MAX_SSID_LENGTH+1];

    int TotalScannedWifi = 4;
    String SSID = "";
    if (millis() - Wifiprev >= WifiInterval) {
        Wifiprev = millis();
        //TotalScannedWifi = WiFi.scanNetworks();

    }

    for (int i = ssidToShow_initialPos, j = 0; i < ssidToShow_finalPos, j < ssidToShow; i++, j++) {
      String SSID = WiFi.SSID(i);
      if (SSID.length() > 20) {                                                    // Limiting text overflow by using this logic
        strncpy(listSSIDBuff, SSID.c_str(), MAX_SSID_LENGTH);
        
        listSSIDBuff[20] = '.'; listSSIDBuff[21] = '.'; listSSIDBuff[22] = '.';listSSIDBuff[23] = '\0';

        u8g2.drawStr(3, SSIDListInitialY+(WifiMenuYtoAdd * j), listSSIDBuff);
      }
      else 
        u8g2.drawStr(3, SSIDListInitialY+(WifiMenuYtoAdd * j), SSID.c_str());
    }

    // Select
    u8g2.setDrawColor(2);
    u8g2.drawBox(1, 11 + (WifiMenu_HighlightIndex*13), 118, 13);  // add 13 to Y axis for each index to move 


    // scroll
    //u8g2.drawBox(121, constrain(map(WifiMenu_PageCount, 1, MusicList_MaxPage, 18, 47), 18, 47), 5, SCROLL_SIZE);
    u8g2.drawBox(121, constrain(map(ssidToShow_finalPos, ssidToShow, SSID_COUNT, 18, 46), 18, 46), 5, WifiMenuSCROLL_SIZE);

    // scrollbar-border
    u8g2.setDrawColor(1);
    u8g2.drawXBMP(119, 10, 9, 54, image_scroll_border_bits);


    u8g2.sendBuffer();    
}

void WifiMenu_BUTTON_LOGIC(struct Button_struct* Button) {
  if (Button->btn1 == LOW) {
    if (ssidToShow_initialPos <= 0) {
      ssidToShow_initialPos = SSID_COUNT - 4;
      ssidToShow_finalPos = SSID_COUNT;
      WifiMenu_HighlightIndex = 0;
      WifiMenu_MusicIndex = ssidToShow_initialPos;
    }
    else {
      ssidToShow_initialPos -= ssidToShow;
      ssidToShow_finalPos -= ssidToShow;
      WifiMenu_MusicIndex = ssidToShow_initialPos + WifiMenu_HighlightIndex;
    }

    }

  if (Button->btn2 == LOW) {
    if (ssidToShow_finalPos + ssidToShow > SSID_COUNT) {
      ssidToShow_initialPos = 0;
      ssidToShow_finalPos = ssidToShow;
      WifiMenu_HighlightIndex = 0;
      WifiMenu_MusicIndex = 0;
    }
    else {
      ssidToShow_initialPos += ssidToShow;
      ssidToShow_finalPos += ssidToShow;  
      WifiMenu_MusicIndex = ssidToShow_initialPos + WifiMenu_HighlightIndex;    
    }
  }

  if (Button->btn3 == LOW) {
    prev_scr = current_scr;
    current_scr = KEYBOARD;
  }

  if (Button->btn4 == LOW) {
    if (WifiMenu_HighlightIndex == ssidToShow-1) {
      if (WifiMenu_MusicIndex == SSID_COUNT-1) {
        WifiMenu_HighlightIndex = 0;
        WifiMenu_MusicIndex = 0;
        ssidToShow_initialPos = 0;
        ssidToShow_finalPos = ssidToShow;
      }
      else {
        ssidToShow_finalPos++;
        ssidToShow_initialPos++;
        WifiMenu_MusicIndex++;
      }
    }
    else {
      WifiMenu_HighlightIndex++;
      WifiMenu_MusicIndex++;
    }
  }

  if (Button->btn5 == LOW) {
    if (WifiMenu_HighlightIndex == 0) {
      if (WifiMenu_MusicIndex == 0) {
        WifiMenu_HighlightIndex = ssidToShow-1;
        WifiMenu_MusicIndex = SSID_COUNT-1;
        ssidToShow_initialPos = SSID_COUNT-ssidToShow;
        ssidToShow_finalPos = SSID_COUNT;
      }
      else {
        ssidToShow_finalPos--;
        ssidToShow_initialPos--;
        WifiMenu_MusicIndex--;

      }
    }
    else {
      WifiMenu_HighlightIndex--;
      WifiMenu_MusicIndex--;  
    }
  }

  if (Button->btn6 == LOW) {
    current_scr = MENU;
    WifiMenu_MusicIndex = 0;
    WifiMenu_HighlightIndex = 0;

    ssidToShow_initialPos = 0;
    ssidToShow_finalPos = ssidToShow;
  }

}