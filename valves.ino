/*
Auto watering valves management.
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

#include "valves.h"
#include "settings.h"

bool valves_state[VALVE_NUMBER];

void valve_set(uint16_t id, bool open, bool *opened)
{
  *opened = 0;
  if (valves_state[id] == open) {
    return;
  }
  valves_state[id] = open;
  Serial.print("Valve ");
  Serial.print(id);
  if (open) {
    digitalWrite(VALVE_GPIO[id], HIGH);
    Serial.println(" opened");
    *opened = 1;
  } else {
    digitalWrite(VALVE_GPIO[id], LOW);
    Serial.println(" closed");
  }
}

void valves_init(void)
{
  for (int i = 0; i < VALVE_NUMBER; i++) {
    pinMode(VALVE_GPIO[i], OUTPUT);
    digitalWrite(VALVE_GPIO[i], LOW);
  }
  memset(valves_state, 0, sizeof(valves_state));
  valves_update();
}

bool valve_is_scheduled_time_now(int id)
{
  start_time_t start_time;
  uint8_t duration;
  struct tm now_tm, start_tm, stop_tm;
  time_t now_ts, start_ts, stop_ts;
  
  getLocalTime(&now_tm);
  now_ts = mktime(&now_tm);
  
  start_tm = now_tm;
  start_time = settings_get_valve_start_time(id);
  start_tm.tm_hour = start_time.hour;
  start_tm.tm_min = start_time.minute;
  start_tm.tm_sec = 0;
  start_ts = mktime(&start_tm);
  
  stop_tm = start_tm;
  stop_ts = mktime(&stop_tm);
  duration = settings_get_valve_duration(id);
  stop_ts += duration * 60;
  
  if (start_ts <= now_ts && now_ts < stop_ts) {
    return true;
  }
  return false;
}

void valves_update(void)
{
  bool general_force_on = settings_get_general_force_on();
  bool general_force_off = settings_get_general_force_off();
  bool opened;
  
  if (general_force_on || general_force_off) {
    for (int i = 0; i < VALVE_NUMBER; i++) {
      valve_set(i, general_force_on, &opened);
      if (opened) {
        return; // open only one valve at a time
      }
    }
  } else {
    for (int i = 0; i < VALVE_NUMBER; i++) {
      bool force_on = settings_get_valve_force_on(i);
      bool force_off = settings_get_valve_force_off(i);
      if (force_on || force_off) {
        valve_set(i, force_on, &opened);
      } else {
        valve_set(i, valve_is_scheduled_time_now(i), &opened);
      }
      if (opened) {
        return; // open only one valve at a time
      }
    }
  }
}

bool *get_valves_state(void)
{
  return valves_state;
}
