/*
Auto watering internationalization strings for french.
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

#include "config.h"
#ifdef LANG_FR
#ifndef __I18N_LANG_H__
#define __I18N_LANG_H__

#include "i18n.h"

i18n_translation_t I18N_TRANSLATIONS[] = {
  {.msgid = "OFF", .translation = "OFF"},
  {.msgid = "ON", .translation = "ON"},
  {.msgid = "Cycle ", .translation = "Cycle "},
  {.msgid = " in progress", .translation = " en cours"},
  {.msgid = "Manual cycle in progress", .translation = "Cycle manuel en cours"},
  {.msgid = "No active cycle", .translation = "Aucun cycle en cours"},
  {.msgid = "Scheduling", .translation = "Programmation"},
  {.msgid = "General manual control", .translation = "Contrôle manuel général"},
  {.msgid = "Force OFF", .translation = "Arrêt forcé"},
  {.msgid = "Active cycle", .translation = "Cycle actif"},
  {.msgid = "Valve ", .translation = "Vanne "},
  {.msgid = "Opening duration", .translation = "Durée d'ouverture"},
  {.msgid = "min", .translation = "min"},
  {.msgid = "Manual control", .translation = "Contrôle manuel"},
  {.msgid = "Force ON", .translation = "Marche forcée"},
  {.msgid = "Force OFF", .translation = "Arrêt forcé"},
  {.msgid = "Status", .translation = "Statut"},
  {.msgid = "Automatic watering", .translation = "Arrosage automatique"},
  {.msgid = "Manual cycle", .translation = "Cycle manuel"},
};

#endif
#endif
