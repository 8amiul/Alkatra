#include "functions.h"
#include <WiFi.h>
#include "time.h"
#include "SoftwareSerial.h"
#include <stddef.h>
#include "esp_system.h"

char* ssid     = "/home/8amiul/";
char* password = "#1992470";        // yeah, my wifi password.

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

void sToHMSstring(unsigned long sec, char *buf, size_t buf_size)
{
    unsigned long h = sec / 3600;
    unsigned long m = (sec % 3600) / 60;
    unsigned long s = sec % 60;

    snprintf(buf, buf_size, "%02lu:%02lu:%02lu", h, m, s);
}

void sToMSstring(unsigned long sec, char *buf, size_t buf_size)
{
    unsigned long m = sec / 60;
    unsigned long s = sec % 60;

    snprintf(buf, buf_size, "%02lu:%02lu", m, s);
}

void shuffle(int *array, int n) {
    if (n > 1) {
        for (int i = n - 1; i > 0; i--) {
            // Pick a random index from 0 to i
            int j = esp_random() % (i + 1);
            
            // Swap array[i] with the element at random index
            int temp = array[i];
            array[i] = array[j];
            array[j] = temp;
        }
    }

}
