/*
Auto watering network.
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

#include <WiFi.h>
#include "config_wifi.h"

#define RECONNECT_INTERVAL 30000
unsigned long previous_reconnect_date;

void net_start(void)
{
  Serial.print("Connecting");  
  WiFi.begin(WIFI_ESSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());
  previous_reconnect_date = millis();
}

void net_keep_alive(void)
{
  unsigned long current_date = millis();
  if ((WiFi.status() != WL_CONNECTED) && (current_date - previous_reconnect_date >= RECONNECT_INTERVAL)) {
    Serial.println("Connection lost, reconnecting");
    WiFi.disconnect();
    WiFi.reconnect();
    previous_reconnect_date = current_date;
  }
}
