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

typedef struct start_time_s {
  uint8_t hour;
  uint8_t minute;
} start_time_t;

/**
 * Load settings from persistent memory.
 */
void settings_load(void);

/**
 * Store settings to persistent memory.
 */
void settings_store(void);

/**
 * Check if settings have changed.
 * @return false if settings are unchanged.
 */
bool settings_changed(void);

/**
 * Get general force ON setting.
 * @return true if enabled.
 */
bool settings_get_general_force_on(void);

/**
 * Set general force ON setting.
 * @param[in] enable / disable.
 */
void settings_set_general_force_on(bool enable);

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
 * Get valve start time setting.
 * @param[in] valve index.
 * @return configured start time.
 */
start_time_t settings_get_valve_start_time(uint16_t id);

/**
 * Set valve start time setting.
 * @param[in] valve index.
 * @param[in] requested start time.
 */
void settings_set_valve_start_time(uint16_t id, start_time_t start_time);

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
 * Convert time string to start_time_t.
 * @param[in] time string, format "HH:MM".
 * @return start_time_t time format.
 */
start_time_t str_to_start_time_t(String time_str);

/**
 * Convert start_time_t to time string.
 * @param[in] start_time_t time format.
 * @return time string, format "HH:MM".
 */
String start_time_t_to_str(start_time_t start_time);

#endif
