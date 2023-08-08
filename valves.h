/*
Auto watering valves management API.
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

#ifndef __VALVES_H__
#define __VALVES_H__

/**
 * Initialize valves.
 * If config.h has FORCE_ON_PERSISTENT set, some valves may be opened according to force ON settings.
 */
void valves_init(void);

/**
 * Update valves states according to settings and current time.
 * Only one valve is opened on each call, to limit power supply power call.
 * So, if several valves are to be opened at the same time, the call must be repeated after a short delay.
 */
void valves_update(void);

/**
 * Get current valves states.
 * @return bool array, true if valve is opened, else false.
 */
bool *get_valves_state(void);

/**
 * Get current watering cycle.
 * @return cycle ID, or -1 if none currently active.
 */
int get_current_cycle(void);

#endif
