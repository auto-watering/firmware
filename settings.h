#ifndef __SETTINGS_H__
#define __SETTINGS_H__

typedef struct start_time_s {
  uint8_t hour;
  uint8_t minute;
} start_time_t;

void settings_reset(void);
void settings_load(void);
void settings_store(void);
bool settings_changed(void);
bool settings_get_general_force_on(void);
void settings_set_general_force_on(bool enable);
bool settings_get_general_force_off(void);
void settings_set_general_force_off(bool enable);
start_time_t settings_get_valve_start_time(uint16_t id);
void settings_set_valve_start_time(uint16_t id, start_time_t start_time);
uint8_t settings_get_valve_duration(uint16_t id);
void settings_set_valve_duration(uint16_t id, uint8_t duration);
bool settings_get_valve_force_on(uint16_t id);
void settings_set_valve_force_on(uint16_t id, bool enable);
bool settings_get_valve_force_off(uint16_t id);
void settings_set_valve_force_off(uint16_t id, bool enable);
start_time_t str_to_start_time_t(String time_str);
String start_time_t_to_str(start_time_t start_time);

#endif
