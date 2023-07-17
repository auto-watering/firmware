#include "valves.h"
#include "settings.h"

bool valves_state[VALVE_NUMBER];

void valve_set(uint16_t id, bool open)
{
  if (valves_state[id] == open) {
    return;
  }
  valves_state[id] = open;
  Serial.print("Valve ");
  Serial.print(id);
  if (open) {
    Serial.println(" opened");
    // TODO
  } else {
    Serial.println(" closed");
    // TODO
  }
}

void valves_init(void)
{
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
  
  if (general_force_on || general_force_off) {
    for (int i = 0; i < VALVE_NUMBER; i++) {
      valve_set(i, general_force_on);
    }
  } else {
    for (int i = 0; i < VALVE_NUMBER; i++) {
      bool force_on = settings_get_valve_force_on(i);
      bool force_off = settings_get_valve_force_off(i);
      if (force_on || force_off) {
        valve_set(i, force_on);
      } else {
        valve_set(i, valve_is_scheduled_time_now(i));
      }
    }
  }
}

bool *get_valves_state(void)
{
  return valves_state;
}
