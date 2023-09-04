/*
Auto watering date management.
Copyright (C) 2023 Mathieu ABATI <mathieu.abati@gmail.com>

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

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
  Serial.println(F("NTP time synchronized"));
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

bool time_get(timeinfo_t *current_time)
{
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)){
    Serial.println("Failed to get time !");
    return false;
  }
  current_time->hour = timeinfo.tm_hour;
  current_time->minute = timeinfo.tm_min;
  return true;
}

timeinfo_t str_to_timeinfo_t(String time_str)
{
  timeinfo_t start_time;

  start_time.hour = 0;
  start_time.minute = 0;
  int colidx = time_str.indexOf(':');
  
  if (colidx != -1) {
    start_time.hour = time_str.substring(0, colidx).toInt();
    start_time.minute = time_str.substring(colidx + 1).toInt();
  }
  return start_time;
}

String timeinfo_t_to_str(timeinfo_t start_time)
{
  String time_str;
  if (start_time.hour < 10) {
    time_str = "0" + String(start_time.hour);
  } else {
    time_str = String(start_time.hour);
  }
  time_str += ":";
  if (start_time.minute < 10) {
    time_str += "0" + String(start_time.minute);
  } else {
    time_str += String(start_time.minute);
  }
  return time_str;
}
