#include <time.h>
#include <esp_sntp.h>

const char* ntp_server = "pool.ntp.org";
const long gmt_offset_sec = 0;
const int daylight_offset_sec = 3600;

void time_print()
{
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)){
    Serial.println("Failed to get time !");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
}

void time_synced_cb(struct timeval *tv)
{
  Serial.println(F("NTP time synched"));
}

void time_init(void)
{
  sntp_set_sync_interval(24 * 60 * 60 * 1000UL); // time sync every 24 hours
  sntp_set_time_sync_notification_cb(time_synced_cb);
  configTime(gmt_offset_sec, daylight_offset_sec, ntp_server);
  setenv("TZ", TIMEZONE, 1);
  tzset();
  time_print();
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

bool time_get(int *hour, int *minute)
{
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)){
    Serial.println("Failed to get time !");
    return false;
  }
  *hour = timeinfo.tm_hour;
  *minute = timeinfo.tm_min;
  return true;
}
