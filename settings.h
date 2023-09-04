/*
Auto watering settings store API.
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

#ifndef __SETTINGS_H__
#define __SETTINGS_H__

#include "timedate.h"

/**
 * Load settings from persistent memory.
 */
void settings_load(void);

/**
 * Store settings to persistent memory.
 */
void settings_store(void);

/**
 * Check if settings have changed since last call to settings_store().
 * @return false if settings are unchanged.
 */
bool settings_changed(void);

/**
 * Check if settings have changed according to given CRC.
 * @param[in/out] CRC, which is updated on return with the current one.
 * @return false if settings are unchanged.
 */
bool settings_changed(uint32_t *crc);

/**
 * Get current settings CRC.
 * @return Settings CRC.
 */
uint32_t settings_get_crc(void);

/**
 * Get general force OFF setting.
 * @return true if enabled.
 */
bool settings_get_general_force_off(void);

/**
 * Set general force OFF setting.
 * @param[in] enable / disable.
 */
void settings_set_general_force_off(bool enable);

/**
 * Get cycle start time.
 * @param[in] cycle index, 0 is for manual cycle, and other valid indexes depends on CYCLES_NUMBER.
 * @param[out] cycle start time.
 * @return true if cycle is enabled.
 */
bool settings_get_cycle_start_time(int cycle_id, timeinfo_t *start_time);

/**
 * Set cycle start time.
 * @param[in] cycle index, 0 is for manual cycle, and other valid indexes depends on CYCLES_NUMBER.
 * @param[out] cycle start time.
 */
void settings_set_cycle_start_time(int cycle_id, timeinfo_t start_time);

/**
 * Enable or disable cycle.
 * @param[in] cycle index, 0 is for manual cycle, and other valid indexes depends on CYCLES_NUMBER.
 * @param[in] true to enable cycle
 */
void settings_enable_cycle(int cycle_id, bool enable);

/**
 * Check if a cycle is enabled.
  * @param[in] cycle index, 0 is for manual cycle, and other valid indexes depends on CYCLES_NUMBER.
 * @return true if cycle is enabled.
 */
bool settings_is_cycle_enabled(int cycle_id);

/**
 * Get valve opened duration setting.
 * @param[in] valve index.
 * @return configured duration.
 */
uint8_t settings_get_valve_duration(uint16_t id);

/**
 * Set valve start opened duration setting.
 * @param[in] valve index.
 * @param[in] requested duration.
 */
void settings_set_valve_duration(uint16_t id, uint8_t duration);

/**
 * Get valve force ON setting.
 * @param[in] valve index.
 * @return true if enabled.
 */
bool settings_get_valve_force_on(uint16_t id);

/**
 * Set valve force ON setting.
 * @param[in] valve index.
 * @param[in] enable / disable.
 */
void settings_set_valve_force_on(uint16_t id, bool enable);

/**
 * Get valve force OFF setting.
 * @param[in] valve index.
 * @return true if enabled.
 */
bool settings_get_valve_force_off(uint16_t id);

/**
 * Set valve force OFF setting.
 * @param[in] valve index.
 * @param[in] enable / disable.
 */
void settings_set_valve_force_off(uint16_t id, bool enable);

/**
 * Convert time string to timeinfo_t.
 * @param[in] time string, format "HH:MM".
 * @return timeinfo_t time format.
 */
timeinfo_t str_to_timeinfo_t(String time_str);

/**
 * Convert timeinfo_t to time string.
 * @param[in] timeinfo_t time format.
 * @return time string, format "HH:MM".
 */
String timeinfo_t_to_str(timeinfo_t start_time);

#endif
