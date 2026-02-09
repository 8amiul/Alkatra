#pragma once

extern char* ssid;
extern char* password;

extern const char* ntpServer;
extern const long  gmtOffset_sec;
extern const int   daylightOffset_sec;

extern struct tm timeinfo;
extern char timeString[30];
extern char dateString[30];
extern char weekString[15];
extern int hr;
extern int isTimeSet;
extern const char* months[];



void wifi_connect(void);
int date_allocation(void);
void get_time();
//void mkTimeDateDisplayString();
