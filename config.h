/*
Auto watering settings.
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

#ifndef __CONFIG_H__
#define __CONFIG_H__

#define VALVE_NUMBER 5 // 5 valves installed
#define FORCE_ON_PERSISTENT false // is valves force ON setting persistent accross reboots?
#define TIMEZONE "CET-1CEST,M3.5.0,M10.5.0/3" // Europe/Paris
int VALVE_GPIO[VALVE_NUMBER] = {
  12, // valve 0
  14, // valve 1
  27, // valve 2
  26, // valve 3
  25, // valve 4
  // 33 // valve 5
};

#endif
