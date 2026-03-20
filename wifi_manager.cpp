#include "display.h"
#include "bitmaps.h"
#include "functions.h"
#include "music.h"
#include "game.h"
#include "keyboard.h"
#include "wifi_manager.h"


int ssidToShow = 4;

int WifiMenuYtoAdd = 13;
int SSIDListInitialY = 21;

int WifiMenu_ssidIndex = 0;
int WifiMenu_HighlightIndex = 0;

int ssidToShow_initialPos = 0;
int ssidToShow_finalPos = ssidToShow;

int WifiMenuSCROLL_SIZE = 10;
int WifiMenu_PageCount = 1;
int WifiMenu_MaxPage = ceil(SSID_COUNT / ssidToShow);
unsigned long  Wifiprev = 0;
unsigned long WifiInterval = 1000;



void drawWifiMenu(void) {

    if (WiFi.status() == WL_CONNECTED) {
      current_scr = WIFI_CONNECTED;
      prev_scr = MENU;
    }

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
      WifiMenu_ssidIndex = ssidToShow_initialPos;
    }
    else {
      ssidToShow_initialPos -= ssidToShow;
      ssidToShow_finalPos -= ssidToShow;
      WifiMenu_ssidIndex = ssidToShow_initialPos + WifiMenu_HighlightIndex;
    }

    }

  if (Button->btn2 == LOW) {
    if (ssidToShow_finalPos + ssidToShow > SSID_COUNT) {
      ssidToShow_initialPos = 0;
      ssidToShow_finalPos = ssidToShow;
      WifiMenu_HighlightIndex = 0;
      WifiMenu_ssidIndex = 0;
    }
    else {
      ssidToShow_initialPos += ssidToShow;
      ssidToShow_finalPos += ssidToShow;  
      WifiMenu_ssidIndex = ssidToShow_initialPos + WifiMenu_HighlightIndex;    
    }
  }

  if (Button->btn3 == LOW) {
    prev_scr = current_scr;
    current_scr = KEYBOARD;
  }

  if (Button->btn4 == LOW) {
    if (WifiMenu_HighlightIndex == ssidToShow-1) {
      if (WifiMenu_ssidIndex == SSID_COUNT-1) {
        WifiMenu_HighlightIndex = 0;
        WifiMenu_ssidIndex = 0;
        ssidToShow_initialPos = 0;
        ssidToShow_finalPos = ssidToShow;
      }
      else {
        ssidToShow_finalPos++;
        ssidToShow_initialPos++;
        WifiMenu_ssidIndex++;
      }
    }
    else {
      WifiMenu_HighlightIndex++;
      WifiMenu_ssidIndex++;
    }
  }

  if (Button->btn5 == LOW) {
    if (WifiMenu_HighlightIndex == 0) {
      if (WifiMenu_ssidIndex == 0) {
        WifiMenu_HighlightIndex = ssidToShow-1;
        WifiMenu_ssidIndex = SSID_COUNT-1;
        ssidToShow_initialPos = SSID_COUNT-ssidToShow;
        ssidToShow_finalPos = SSID_COUNT;
      }
      else {
        ssidToShow_finalPos--;
        ssidToShow_initialPos--;
        WifiMenu_ssidIndex--;

      }
    }
    else {
      WifiMenu_HighlightIndex--;
      WifiMenu_ssidIndex--;  
    }
  }

  if (Button->btn6 == LOW) {
    current_scr = MENU;
    WifiMenu_ssidIndex = 0;
    WifiMenu_HighlightIndex = 0;

    ssidToShow_initialPos = 0;
    ssidToShow_finalPos = ssidToShow;
  }

}

void drawConnectedWifi(void) {
    u8g2.clearBuffer();
    DRAW_NAVBAR();
    u8g2.setFontMode(1);
    u8g2.setBitmapMode(1);
    // window border
    u8g2.drawFrame(0, 10, 128, 54);
    int wifiStrength = abs(WiFi.RSSI());

    if (wifiStrength <= 60)
        u8g2.drawXBM(54, 15, 19, 16, image_wifi_full_bits);
    else if (wifiStrength > 60 && wifiStrength <= 70)
        u8g2.drawXBM(54, 24, 19, 16, image_wifi_75_bits);
    else if (wifiStrength > 70 && wifiStrength <= 90)
        u8g2.drawXBM(54, 24, 19, 16, image_wifi_50_bits);
    else if (wifiStrength > 90)
        u8g2.drawXBM(54, 24, 19, 16, image_wifi_not_connected_bits);


    // Layer 9
    u8g2.drawBox(38, 48, 52, 13);

    // Layer 8
    u8g2.setFont(u8g2_font_5x7_tr);
    String ssid = WiFi.SSID();
    drawCenteredStr(42, ssid.c_str());
    //u8g2.drawStr(34, 42, WiFi.SSID());

    // Layer 10
    u8g2.setDrawColor(2);
    u8g2.setFont(u8g2_font_4x6_tr);
    u8g2.drawStr(44, 57, "disconnect");

    u8g2.sendBuffer();
}

void drawConnectedWifi_BUTTON_LOGIC(struct Button_struct* Button) {
    if (Button->btn3 == LOW) {
        WiFi.disconnect();
        current_scr = WIFI;
        prev_scr = WIFI_CONNECTED;
    }
    if (Button->btn6 == LOW) {
        current_scr = MENU;
        prev_scr = WIFI_CONNECTED;
    }
}