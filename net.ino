#include <WiFi.h>
#include "config_wifi.h"

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
}
