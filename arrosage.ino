 #include <ESPUI.h>

#include "net.h"
#include "timedate.h"
#include "settings.h"
#include "gui.h"

void setup() {
  Serial.begin(115200);
  while (!Serial);
  settings_load();
  net_start();
  time_start();
  gui_start();
}

void loop() {
  gui_set_date(time_get_formatted());
  if (settings_changed()) {
    settings_store();
  }
  delay(1000);
}
