#pragma once
#include <WiFi.h>

void drawWifiMenu(void);
void WifiMenu_BUTTON_LOGIC(struct Button_struct* Button);
void drawConnectedWifi(void);
void drawConnectedWifi_BUTTON_LOGIC(struct Button_struct* Button);