#include <WiFi.h>

void net_start(void)
{
  Serial.print("Connecting");  
  WiFi.begin("GLaDOS", "la hoz de la vieja provincia teruel");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());
}
