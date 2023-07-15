#include <time.h>

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 0;
const int   daylightOffset_sec = 3600;

void printLocalTime(){
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)){
    Serial.println("Failed to get time !");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
}

void time_start(void)
{
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  printLocalTime();
}

String time_get_formatted(void)
{
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)){
    Serial.println("Failed to get time !");
    return "";
  }
  char buffer[50];
  strftime(buffer, sizeof(buffer), "%A, %B %d %Y %H:%M:%S", &timeinfo);
  return String(buffer);
}
