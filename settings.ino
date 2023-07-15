#include <EEPROM.h>
#include <CRC32.h>

#include "config.h"

typedef struct settings_s {
  bool general_force_on, general_force_off;
} settings_t;
settings_t settings;

typedef struct valve_settings_s {
  start_time_t start_time;
  uint16_t duration;
  bool force_on, force_off;
} valve_settings_t;
valve_settings_t valve_settings[VALVE_NUMBER];

uint32_t settings_crc;

void settings_compute_crc(void)
{
  CRC32 crc;
  crc.update(reinterpret_cast<const uint8_t*>(&settings), sizeof(settings));
  crc.update(reinterpret_cast<const uint8_t*>(&valve_settings), sizeof(valve_settings));
  settings_crc = crc.finalize();
}

void settings_reset(void)
{
  memset(&settings, 0x00, sizeof(settings));
  memset(&valve_settings, 0x00, sizeof(valve_settings));
  settings_store();
}

void settings_load(void)
{
  EEPROM.begin(512);
  EEPROM.get(0, settings);
  EEPROM.get(sizeof(settings), valve_settings);
  uint32_t stored_crc;
  EEPROM.get(sizeof(settings) + sizeof(valve_settings), stored_crc);
  Serial.println("Settings loaded");
  settings_compute_crc();
  if (stored_crc != settings_crc) {
    Serial.print("Stored CRC (");
    Serial.print(stored_crc);
    Serial.print(") different than settings CRC (");
    Serial.print(settings_crc);
    Serial.println("), resetting data");
    settings_reset();
  }
  if (!FORCE_ON_PERSISTENT) {
    settings.general_force_on = false;
    for (int i = 0; i < VALVE_NUMBER; i++) {
      valve_settings[i].force_on = false;
    }
  }
}

void settings_store(void)
{
  EEPROM.begin(512);
  EEPROM.put(0, settings);
  EEPROM.put(sizeof(settings), valve_settings);
  settings_compute_crc();
  EEPROM.put(sizeof(settings) + sizeof(valve_settings), settings_crc);
  Serial.print("Writing EEPROM, CRC is ");
  Serial.println(settings_crc);
  EEPROM.commit();
  Serial.println("Settings stored");
}

bool settings_changed(void)
{
  uint32_t last_crc = settings_crc;
  settings_compute_crc();
  if (last_crc != settings_crc) {
    return true;
  }
  return false;
}

start_time_t settings_get_valve_start_time(uint16_t id)
{
  return valve_settings[id].start_time;
}

void settings_set_valve_start_time(uint16_t id, start_time_t start_time)
{
  valve_settings[id].start_time = start_time;
}

uint8_t settings_get_valve_duration(uint16_t id)
{
  return valve_settings[id].duration;
}

void settings_set_valve_duration(uint16_t id, uint8_t duration)
{
  valve_settings[id].duration = duration;
}

bool settings_get_general_force_on(void)
{
  return settings.general_force_on;
}

void settings_set_general_force_on(bool enable)
{
  settings.general_force_on = enable;
}

bool settings_get_general_force_off(void)
{
  return settings.general_force_off;
}

void settings_set_general_force_off(bool enable)
{
  settings.general_force_off = enable;
}

bool settings_get_valve_force_on(uint16_t id)
{
  return valve_settings[id].force_on;
}

void settings_set_valve_force_on(uint16_t id, bool enable)
{
  valve_settings[id].force_on = enable;
}

bool settings_get_valve_force_off(uint16_t id)
{
  return valve_settings[id].force_off;
}

void settings_set_valve_force_off(uint16_t id, bool enable)
{
  valve_settings[id].force_off = enable;
}

start_time_t str_to_start_time_t(String time_str)
{
  start_time_t start_time;

  start_time.hour = 0;
  start_time.minute = 0;
  int colidx = time_str.indexOf(':');
  
  if (colidx != -1) {
    start_time.hour = time_str.substring(0, colidx).toInt();
    start_time.minute = time_str.substring(colidx + 1).toInt();
  }
  return start_time;
}

String start_time_t_to_str(start_time_t start_time)
{
  String time_str;
  if (start_time.hour < 10) {
    time_str = "0" + String(start_time.hour);
  } else {
    time_str = String(start_time.hour);
  }
  time_str += ":";
  if (start_time.minute < 10) {
    time_str += "0" + String(start_time.minute);
  } else {
    time_str += String(start_time.minute);
  }
  return time_str;
}
