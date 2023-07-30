/*
Auto watering GUI API.
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

#ifndef __GUI_H__
#define __GUI_H__

/*
 * Start serving web GUI.
 * Network has to be initialized before with net_start() from net.h.
 */
void gui_start(void);

/*
 * Refresh GUI display.
 * @param[in] human readable date string, as returned by time_get_formatted() from timedate.h.
 * @param[in] valves states array (true: open, false: closed), this array can be retrieved using get_valves_state() from vales.h.
 */
void gui_refresh(String date, bool valves_state[]);

#endif
