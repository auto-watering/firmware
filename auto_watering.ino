/*
Auto watering entry point.
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

#include <ESPUI.h>

#include "config.h"
#include "net.h"
#include "timedate.h"
#include "settings.h"
#include "gui.h"
#include "valves.h"

void setup() {
  Serial.begin(115200);
  while (!Serial);
  valves_init();
  settings_load();
  net_start();
  time_init();
  gui_start();
#ifdef USE_MQTT
  mqtt_start();
#endif
  wdt_init();
}

void loop() {
  net_keep_alive();
  valves_update();
  gui_refresh();
#ifdef USE_MQTT
  mqtt_refresh();
#endif
  if (settings_changed()) {
    settings_store();
  }
  wdt_notify();
  delay(100);
}
