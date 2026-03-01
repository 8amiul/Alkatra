#pragma once
#include <stddef.h>
#include <Arduino.h>

extern char* ssid;
extern char* password;

extern const char* ntpServer;
extern const long  gmtOffset_sec;
extern const int   daylightOffset_sec;

constexpr uint8_t BATTERY_VREAD_PIN = 33;
constexpr float MAX_PIN_READING = 4095.0;
constexpr float MAX_PIN_VOLT = 3.3;
constexpr float BATTERY_VOLT_SMOOTHING_FACTOR = 1.085;
extern int batteryPinReading;

extern struct tm timeinfo;
extern char timeString[30];
extern char dateString[30];
extern char weekString[15];
extern int hr;
extern int isTimeSet;
extern const char* months[];


void shuffle(int *array, int n);
void wifi_connect(void);
int date_allocation(void);
void get_time();
//void mkTimeDateDisplayString();
void sToHMSstring(unsigned long sec, char *buf, size_t buf_size);
void sToMSstring(unsigned long sec, char *buf, size_t buf_size);


extern float vsum;
extern int vcount;
extern float battery_voltage;
void setBatteryVoltage();