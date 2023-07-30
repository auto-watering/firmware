/*
Auto watering GUI.
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

#include <ESPUI.h>

#include "config.h"

typedef struct gui_elements_valve_s {
  String valve_label;
  uint16_t start_ctrl, duration_ctrl, force_on_ctrl, force_off_ctrl;
  uint16_t auto_grp, manual_grp;
  uint16_t status_label;
} gui_elements_valve_t;
gui_elements_valve_t gui_elements_valve[VALVE_NUMBER];

typedef struct gui_elements_s {
  uint16_t general_force_on_ctrl, general_force_off_ctrl;
  uint16_t general_manual_grp;
  uint16_t date_label;
} gui_elements_t;
gui_elements_t gui_elements;

void gui_status_change(int id, bool status)
{
  if (status) {
    ESPUI.getControl(gui_elements_valve[id].status_label)->color = ControlColor::Emerald;
    ESPUI.updateControlValue(gui_elements_valve[id].status_label, "ON");
  } else {
    ESPUI.getControl(gui_elements_valve[id].status_label)->color = ControlColor::Alizarin;
    ESPUI.updateControlValue(gui_elements_valve[id].status_label, "OFF");
  }
}

void gui_start_time_cb(Control *sender, int type, void* user_data)
{
  int id = (int)user_data;
  Serial.print("Start date changed for valve ");
  Serial.print(id);
  Serial.print(": ");
  Serial.println(sender->value);
  start_time_t start_time = str_to_start_time_t(sender->value);
  settings_set_valve_start_time(id, start_time);
}

void gui_duration_cb(Control *sender, int type, void* user_data)
{
  int id = (int)user_data;
  Serial.print("Duration changed for valve ");
  Serial.print(id);
  Serial.print(": ");
  Serial.print(sender->value);
  Serial.println(" minutes");
  settings_set_valve_duration(id, sender->value.toInt());
}

void gui_force_on(int id, bool enable)
{
  ESPUI.setEnabled(gui_elements_valve[id].force_off_ctrl, !enable);
  ESPUI.updateControlValue(gui_elements_valve[id].force_on_ctrl, String(enable));
  ESPUI.updateControlValue(gui_elements_valve[id].force_off_ctrl, "0");
  settings_set_valve_force_on(id, enable);
  settings_set_valve_force_off(id, false);
}

void gui_force_off(int id, bool enable)
{
  ESPUI.setEnabled(gui_elements_valve[id].force_on_ctrl, !enable);
  ESPUI.updateControlValue(gui_elements_valve[id].force_off_ctrl, String(enable));
  ESPUI.updateControlValue(gui_elements_valve[id].force_on_ctrl, "0");
  settings_set_valve_force_off(id, enable);
  settings_set_valve_force_on(id, false);
}

void gui_force_on_cb(Control *sender, int type, void* user_data)
{
  int id = (int)user_data;
  Serial.print("Force ON for valve ");
  Serial.print(id);
  bool enabled = sender->value.toInt();
  if (enabled) {
    Serial.println(" enabled");
  } else {
    Serial.println(" disabled");
  }
  gui_force_on(id, enabled);
}

void gui_force_off_cb(Control *sender, int type, void* user_data)
{
  int id = (int)user_data;
  Serial.print("Force OFF for valve ");
  Serial.print(id);
  bool enabled = sender->value.toInt();
  if (enabled) {
    Serial.println(" enabled");
  } else {
    Serial.println(" disabled");
  }
  gui_force_off(id, enabled);
}

void gui_general_force_on(bool enable)
{
  Serial.print("General force ON ");
  if (enable) {
    Serial.println(" enabled");
  } else {
    Serial.println(" disabled");
  }
  settings_set_general_force_on(enable);
  for (int i = 0; i < VALVE_NUMBER; i++) {
    gui_force_on(i, enable);
    ESPUI.setEnabled(gui_elements_valve[i].force_on_ctrl, !enable);
    ESPUI.setEnabled(gui_elements_valve[i].force_off_ctrl, !enable);
  }
  ESPUI.setEnabled(gui_elements.general_force_off_ctrl, !enable);
}

void gui_general_force_off(bool enable)
{
  Serial.print("General force OFF ");
  if (enable) {
    Serial.println(" enabled");
  } else {
    Serial.println(" disabled");
  }
  settings_set_general_force_off(enable);
  for (int i = 0; i < VALVE_NUMBER; i++) {
    gui_force_off(i, enable);
    ESPUI.setEnabled(gui_elements_valve[i].force_on_ctrl, !enable);
    ESPUI.setEnabled(gui_elements_valve[i].force_off_ctrl, !enable);
  }
  ESPUI.setEnabled(gui_elements.general_force_on_ctrl, !enable);
}

void gui_general_force_on_cb(Control *sender, int type)
{
  bool enable = sender->value.toInt();
  gui_general_force_on(enable);
}

void gui_general_force_off_cb(Control *sender, int type)
{
  bool enable = sender->value.toInt();
  gui_general_force_off(enable);
}

void gui_set_date(String date)
{
  ESPUI.updateLabel(gui_elements.date_label, date);
}

void gui_refresh(String date, bool valves_state[VALVE_NUMBER])
{ 
  // Date
  gui_set_date(date);

  for (int i = 0; i < VALVE_NUMBER; i++) {
    // Status
    gui_status_change(i, valves_state[i]);
  }
}

void gui_start(void)
{
  String value;
  bool enabled;
  
  //ESPUI.setVerbosity(Verbosity::VerboseJSON);
  
  String labelStyle = "background-color: unset; margin-bottom: 0em; display: inline";
  String timeStyle = "width: auto; color: black";

  // Manual general control
  gui_elements.general_manual_grp = ESPUI.addControl(Label, "Contrôle manuel général", "Marche forcée", ControlColor::Peterriver, Control::noParent);
  ESPUI.setElementStyle(gui_elements.general_manual_grp, labelStyle);
  value = String(settings_get_general_force_on());
  gui_elements.general_force_on_ctrl = ESPUI.addControl(Switcher, "", value, ControlColor::Peterriver, gui_elements.general_manual_grp, gui_general_force_on_cb);
  ESPUI.setElementStyle(ESPUI.addControl(Label, "", "Arrêt forcé", ControlColor::Peterriver, gui_elements.general_manual_grp), labelStyle);
  value = String(settings_get_general_force_off());
  gui_elements.general_force_off_ctrl = ESPUI.addControl(Switcher, "", value, ControlColor::Peterriver, gui_elements.general_manual_grp, gui_general_force_off_cb);

  // Date
  gui_elements.date_label = ESPUI.addControl(Label, "Heure", "", ControlColor::Peterriver, Control::noParent);
  ESPUI.setElementStyle(gui_elements.date_label, labelStyle);

  for (int i = 0; i < VALVE_NUMBER; i++) {
    gui_elements_valve[i].valve_label = "Vanne " + String(i);
    ESPUI.separator(gui_elements_valve[i].valve_label.c_str());
    
    // Automatic control
    gui_elements_valve[i].auto_grp = ESPUI.addControl(Label, "Programmation", "Heure d'activation", ControlColor::Peterriver, Control::noParent);
    ESPUI.setElementStyle(gui_elements_valve[i].auto_grp, labelStyle);
    start_time_t start_time = settings_get_valve_start_time(i);
    value = start_time_t_to_str(start_time);
    gui_elements_valve[i].start_ctrl = ESPUI.addControl(Text, "", value, ControlColor::Peterriver, gui_elements_valve[i].auto_grp, gui_start_time_cb, (void*)i);
    ESPUI.setElementStyle(gui_elements_valve[i].start_ctrl, timeStyle);
    ESPUI.setInputType(gui_elements_valve[i].start_ctrl, "time");
    ESPUI.setElementStyle(ESPUI.addControl(Label, "", "Durée", ControlColor::Peterriver, gui_elements_valve[i].auto_grp), labelStyle);
    value = String(settings_get_valve_duration(i));
    gui_elements_valve[i].duration_ctrl = ESPUI.addControl(Number, "", value, ControlColor::Peterriver, gui_elements_valve[i].auto_grp, gui_duration_cb, (void*)i);
    ESPUI.addControl(Min, "", "1", None, gui_elements_valve[i].duration_ctrl);
    ESPUI.addControl(Max, "", "60", None, gui_elements_valve[i].duration_ctrl);
    ESPUI.setElementStyle(ESPUI.addControl(Label, "", "min", ControlColor::Peterriver, gui_elements_valve[i].auto_grp), labelStyle);
    
    // Manual control
    gui_elements_valve[i].manual_grp = ESPUI.addControl(Label, "Contrôle manuel", "Marche forcée", ControlColor::Peterriver, Control::noParent);
    ESPUI.setElementStyle(gui_elements_valve[i].manual_grp, labelStyle);
    value = String(settings_get_valve_force_on(i));
    gui_elements_valve[i].force_on_ctrl = ESPUI.addControl(Switcher, "", value, ControlColor::Peterriver, gui_elements_valve[i].manual_grp, gui_force_on_cb, (void*)i);
    ESPUI.setElementStyle(ESPUI.addControl(Label, "", "Arrêt forcé", ControlColor::Peterriver, gui_elements_valve[i].manual_grp), labelStyle);
    value = String(settings_get_valve_force_off(i));
    gui_elements_valve[i].force_off_ctrl = ESPUI.addControl(Switcher, "", value, ControlColor::Peterriver, gui_elements_valve[i].manual_grp, gui_force_off_cb, (void*)i);

    // Status
    gui_elements_valve[i].status_label = ESPUI.addControl(Label, "Statut", "OFF", ControlColor::Alizarin, Control::noParent);
    ESPUI.setElementStyle(gui_elements_valve[i].status_label, labelStyle);
  }

  // Check general control status
  if (settings_get_general_force_off()) {
    gui_general_force_off(true);
  } else if (settings_get_general_force_on()) {
    gui_general_force_on(true);
  }
  
  ESPUI.begin("Arrosage automatique");
}
