/*
Auto watering date management API.
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

#ifndef __TIMEDATE_H__
#define __TIMEDATE_H__

typedef struct start_time_s {
  uint8_t hour;
  uint8_t minute;
} timeinfo_t;

/**
 * Initialize time according to time zone configured in config.h.
 */
void time_init(void);

/**
 * Get human readable formatted date.
 */
String time_get_formatted(void);

/**
 * Get current time.
 * @param[out] current time.
 * @return true on success.
 */
bool time_get(timeinfo_t *current_time);

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
