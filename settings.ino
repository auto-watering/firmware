/*
Auto watering settings store.
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

#include <EEPROM.h>
#include <CRC32.h>

#include "config.h"

typedef struct settings_s {
  bool general_force_off;
  timeinfo_t start_time[CYCLES_NUMBER];
  bool cycle_enabled[CYCLES_NUMBER];
} settings_t;
settings_t settings;

typedef struct valve_settings_s {
  uint16_t duration;
  bool force_on, force_off;
} valve_settings_t;
valve_settings_t valve_settings[VALVE_NUMBER];

typedef struct volatile_settings_s {
  timeinfo_t manual_cycle_start_time;
  bool manual_cycle_enabled;
} volatile_settings_t;
volatile_settings_t volatile_settings;

uint32_t settings_crc;

uint32_t settings_compute_crc(void)
{
  CRC32 crc;
  crc.update(reinterpret_cast<const uint8_t*>(&settings), sizeof(settings));
  crc.update(reinterpret_cast<const uint8_t*>(&valve_settings), sizeof(valve_settings));
  return crc.finalize();
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
  settings_crc = settings_compute_crc();
  if (stored_crc != settings_crc) {
    Serial.print("Stored CRC (");
    Serial.print(stored_crc);
    Serial.print(") different than settings CRC (");
    Serial.print(settings_crc);
    Serial.println("), resetting data");
    settings_reset();
  }
  if (!FORCE_ON_PERSISTENT) {
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
  settings_crc = settings_compute_crc();
  EEPROM.put(sizeof(settings) + sizeof(valve_settings), settings_crc);
  Serial.print("Writing EEPROM, CRC is ");
  Serial.println(settings_crc);
  EEPROM.commit();
  Serial.println("Settings stored");
}

bool settings_changed(void)
{
  uint32_t current_crc = settings_compute_crc();
  if (current_crc != settings_crc) {
    return true;
  }
  return false;
}

bool settings_changed(uint32_t *crc)
{
  bool ret = false;
  uint32_t current_crc = settings_compute_crc();
  if (*crc != current_crc) {
    ret = true;
  }
  *crc = current_crc;
  return ret;
}

uint32_t settings_get_crc(void)
{
  settings_compute_crc();
  return settings_crc;
}

bool settings_get_cycle_start_time(int cycle_id, timeinfo_t *start_time)
{
  bool ret;
  if (cycle_id == 0) {
    *start_time = volatile_settings.manual_cycle_start_time;
    ret = volatile_settings.manual_cycle_enabled;
  } else if (cycle_id <= CYCLES_NUMBER) {
    *start_time = settings.start_time[cycle_id - 1];
    ret = settings.cycle_enabled[cycle_id - 1];
  } else {
    start_time->hour = 0;
    start_time->minute = 0;
    ret = false;
  }
  if (start_time->hour > 23 || start_time->minute > 59) {
    start_time->hour = 0;
    start_time->minute = 0;    
  }
  return ret;
}

void settings_set_cycle_start_time(int cycle_id, timeinfo_t start_time)
{
  if (start_time.hour > 23 || start_time.minute > 59) {
    start_time.hour = 0;
    start_time.minute = 0;    
  }
  if (cycle_id == 0) {
    volatile_settings.manual_cycle_start_time = start_time;
  } else if (cycle_id <= CYCLES_NUMBER) {
    settings.start_time[cycle_id - 1] = start_time;
  }
}

void settings_enable_cycle(int cycle_id, bool enable)
{
  if (cycle_id == 0) {
    volatile_settings.manual_cycle_enabled = enable;
  } else if (cycle_id <= CYCLES_NUMBER) {
    settings.cycle_enabled[cycle_id - 1] = enable;
  }
}

bool settings_is_cycle_enabled(int cycle_id)
{
  if (cycle_id == 0) {
    return volatile_settings.manual_cycle_enabled;
  } else if (cycle_id <= CYCLES_NUMBER) {
    return settings.cycle_enabled[cycle_id - 1];
  } else {
    return false;
  }
}

uint8_t settings_get_valve_duration(uint16_t id)
{
  uint16_t duration = valve_settings[id].duration;
  if (duration > VALVE_MAX_OPENED_DURATION) {
    duration = VALVE_MAX_OPENED_DURATION;
  }
  return duration;
}

void settings_set_valve_duration(uint16_t id, uint8_t duration)
{
  if (duration > VALVE_MAX_OPENED_DURATION) {
    duration = VALVE_MAX_OPENED_DURATION;
  }
  valve_settings[id].duration = duration;
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
  if (enable) {
    valve_settings[id].force_off = false;
  }
}

bool settings_get_valve_force_off(uint16_t id)
{
  return valve_settings[id].force_off;
}

void settings_set_valve_force_off(uint16_t id, bool enable)
{
  valve_settings[id].force_off = enable;
  if (enable) {
    valve_settings[id].force_on = false;
  }
}
