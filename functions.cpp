#include "functions.h"
#include <WiFi.h>
#include "time.h"
#include "SoftwareSerial.h"
#include <TimeLib.h>

char* ssid     = "ssid";
char* password = "pass";

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 21600;
const int   daylightOffset_sec = 0;

struct tm timeinfo;

char timeString[30];
char dateString[30];
char weekString[15];
int isTimeSet = 0;
int hr;
const char* months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};


void setTimeDateString() {
  if (getLocalTime(&timeinfo) && isTimeSet) {
    strftime(timeString, sizeof(timeString), "%H:%M:%S", &timeinfo);
    //strftime(dateString, sizeof(dateString), "%d %B %Y", &timeinfo);
    sprintf(dateString, "%d %s %d", timeinfo.tm_mday, months[timeinfo.tm_mon], timeinfo.tm_year + 1900);
    strftime(weekString, sizeof(weekString), "%A", &timeinfo);
    hr = timeinfo.tm_hour;
  }
}

void wifi_connect(void) {
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected.");
}

/*
void get_time() {
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  if (isTimeSet == 0) isTimeSet = 1;
  //Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  setTimeDateString();
  return;
}*/

void get_time() {
  time_t now;
  // This gets the internal ESP32 clock (no internet call!)
  time(&now); 
  
  // Convert it to the local time structure
  localtime_r(&now, &timeinfo);

  // If the year is still 1970, it means it hasn't synced with NTP yet
  if (timeinfo.tm_year > (2016 - 1900)) {
    isTimeSet = 1;
    setTimeDateString();
  }
}

int date_allocation(void) {
  if (WiFi.status() == WL_CONNECTED) {
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    get_time();
    Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
    return 0;
  }
  else {
    Serial.println("WiFI not connected... Time sync failed");
    isTimeSet = -1;
  }
   return 1;
}


