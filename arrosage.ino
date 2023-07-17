 #include <ESPUI.h>

#include "net.h"
#include "timedate.h"
#include "settings.h"
#include "gui.h"
#include "valves.h"

void setup() {
  Serial.begin(115200);
  while (!Serial);
  settings_load();
  net_start();
  time_init();
  gui_start();
  valves_init();
}

void loop() {
  valves_update();
  gui_refresh(time_get_formatted(), get_valves_state());
  if (settings_changed()) {
    settings_store();
  }
  delay(1000);
}
