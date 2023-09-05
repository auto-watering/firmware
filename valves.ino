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
#include "timedate.h"

bool valves_state[VALVE_NUMBER];
int current_cycle = -1;

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
}

int get_now_scheduled_valve(void)
{
  bool cycle_enabled;
  timeinfo_t start_time;
  uint8_t duration, duration_sum;
  struct tm now_tm, start_tm, stop_tm;
  time_t now_ts, start_ts, stop_ts;
  
  getLocalTime(&now_tm);
  now_ts = mktime(&now_tm);
  
  start_tm = now_tm;

  for (int i = 0; i <= CYCLES_NUMBER; i++) {
    cycle_enabled = settings_get_cycle_start_time(i, &start_time);
    if (!cycle_enabled) {
      continue;
    }
    start_tm.tm_hour = start_time.hour;
    start_tm.tm_min = start_time.minute;
    start_tm.tm_sec = 0;
    start_ts = mktime(&start_tm);
    duration_sum = 0;
    for (int j = 0; j < VALVE_NUMBER; j++) {
      stop_tm = start_tm;
      stop_ts = mktime(&stop_tm);
      duration = settings_get_valve_duration(j);
      duration_sum += duration;
      stop_ts += duration_sum * 60;
      if (start_ts <= now_ts && now_ts < stop_ts) {
        current_cycle = i;
        return j;
      }
    }
  }
  if (settings_is_cycle_enabled(0)) {
    settings_enable_cycle(0, false); // disable manual cycle
  }
  current_cycle = -1;
  return -1;
}

void valves_update(void)
{
  bool general_force_off = settings_get_general_force_off();
  bool opened;
  int now_scheduled;

  if (general_force_off) {
    for (int i = 0; i < VALVE_NUMBER; i++) {
      valve_set(i, false, &opened);
    }
  } else {
    now_scheduled = get_now_scheduled_valve();
    for (int i = 0; i < VALVE_NUMBER; i++) {
      bool force_on = settings_get_valve_force_on(i);
      bool force_off = settings_get_valve_force_off(i);
      if (force_on) {
         valve_set(i, true, &opened);
      } else if (force_off) {
         valve_set(i, false, &opened);
      } else if (i == now_scheduled) {
         valve_set(i, true, &opened);
      } else {
         valve_set(i, false, &opened);
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

void get_valves_state(bool states[VALVE_NUMBER])
{
  for (int i = 0; i < VALVE_NUMBER; i++) {
    states[i] = valves_state[i];
  }
}

int get_current_cycle(void)
{
  if (settings_get_general_force_off()) {
    return -1;
  }
  return current_cycle;
}
