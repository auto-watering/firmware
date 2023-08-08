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
  String valve_str;
  uint16_t duration_ctrl, force_on_ctrl, force_off_ctrl;
  uint16_t duration_grp, manual_grp;
  uint16_t status_label;
} gui_elements_valve_t;
gui_elements_valve_t gui_elements_valve[VALVE_NUMBER];

typedef struct gui_elements_s {
  String start_time_str[MAX_START_PER_DAY];
  uint16_t start_time_ctrl[MAX_START_PER_DAY];
  uint16_t general_force_off_ctrl;
  uint16_t schedule_grp, general_manual_grp;
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
  int index = (int)user_data;
  Serial.print("Start time ");
  Serial.print(index);
  Serial.print(" changed");
  Serial.print(": ");
  Serial.println(sender->value);
  start_time_t start_time = str_to_start_time_t(sender->value);
  settings_set_start_time(index, start_time);
}

void gui_start_time_enable_cb(Control *sender, int type, void* user_data)
{
  int index = (int)user_data;
  bool enabled = sender->value.toInt();
  settings_enable_start_time(index, enabled);
  ESPUI.setEnabled(gui_elements.start_time_ctrl[index], enabled);
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

void gui_newline(uint16_t parent)
{
  String newline_style = "background-color: unset;";
  uint16_t elt = ESPUI.addControl(Label, "", "", ControlColor::None, parent);
  ESPUI.setElementStyle(elt, newline_style);
}

void gui_start(void)
{
  String value;
  bool enabled;
  uint16_t elt;
  
  //ESPUI.setVerbosity(Verbosity::VerboseJSON);
  
  String label_style = "background-color: unset; margin-bottom: 0em; display: inline";
  String time_style = "width: auto; color: black";

  // Schedule
  // current date
  gui_elements.date_label = ESPUI.addControl(Label, "Programmation", "", ControlColor::Peterriver, Control::noParent);
  ESPUI.setElementStyle(gui_elements.date_label, label_style);
  gui_elements.schedule_grp = gui_elements.date_label;
  
  // start time
  for (int i = 0; i < MAX_START_PER_DAY; i++) {
    start_time_t start_time;
    bool start_time_enabled = settings_get_start_time(i, &start_time);
    gui_newline(gui_elements.schedule_grp);
    
    // switcher to enable / disable this time
    value = String(start_time_enabled);
    elt = ESPUI.addControl(Switcher, "", value, ControlColor::None, gui_elements.schedule_grp, gui_start_time_enable_cb, (void*)i);
    
    // label
    gui_elements.start_time_str[i] = "Heure d'activation " + String(i + 1);
    elt = ESPUI.addControl(Label, "", gui_elements.start_time_str[i].c_str(), ControlColor::None, gui_elements.schedule_grp);
    ESPUI.setElementStyle(elt, label_style);
    
    // start time input area
    value = start_time_t_to_str(start_time);
    gui_elements.start_time_ctrl[i] = ESPUI.addControl(Text, "", value, ControlColor::None, gui_elements.schedule_grp, gui_start_time_cb, (void*)i);
    ESPUI.setElementStyle(gui_elements.start_time_ctrl[i], time_style);
    ESPUI.setInputType(gui_elements.start_time_ctrl[i], "time");
    ESPUI.setEnabled(gui_elements.start_time_ctrl[i], start_time_enabled);
  }

  // Manual general control
  elt = ESPUI.addControl(Label, "Contrôle manuel général", "Arrêt forcé", ControlColor::Peterriver, Control::noParent);
  ESPUI.setElementStyle(elt, label_style);
  gui_elements.general_manual_grp = elt;
  value = String(settings_get_general_force_off());
  gui_elements.general_force_off_ctrl = ESPUI.addControl(Switcher, "", value, ControlColor::None, gui_elements.general_manual_grp, gui_general_force_off_cb);

  // Valves settings
  for (int i = 0; i < VALVE_NUMBER; i++) {
    gui_elements_valve[i].valve_str = "Vanne " + String(i);
    ESPUI.separator(gui_elements_valve[i].valve_str.c_str());
    
    // open duration
    gui_elements_valve[i].duration_grp = ESPUI.addControl(Label, "Durée d'ouverture", "", ControlColor::Peterriver, Control::noParent);
    ESPUI.setElementStyle(gui_elements_valve[i].duration_grp, label_style);
    value = String(settings_get_valve_duration(i));
    gui_elements_valve[i].duration_ctrl = ESPUI.addControl(Number, "", value, ControlColor::None, gui_elements_valve[i].duration_grp, gui_duration_cb, (void*)i);
    ESPUI.addControl(Min, "", "1", None, gui_elements_valve[i].duration_ctrl);
    ESPUI.addControl(Max, "", "60", None, gui_elements_valve[i].duration_ctrl);
    ESPUI.setElementStyle(ESPUI.addControl(Label, "", "min", ControlColor::Peterriver, gui_elements_valve[i].duration_grp), label_style);
    
    // manual control
    elt = ESPUI.addControl(Label, "Contrôle manuel", "Marche forcée", ControlColor::Peterriver, Control::noParent);
    ESPUI.setElementStyle(elt, label_style);
    gui_elements_valve[i].manual_grp = elt;
    value = String(settings_get_valve_force_on(i));
    gui_elements_valve[i].force_on_ctrl = ESPUI.addControl(Switcher, "", value, ControlColor::None, gui_elements_valve[i].manual_grp, gui_force_on_cb, (void*)i);
    elt = ESPUI.addControl(Label, "", "Arrêt forcé", ControlColor::Peterriver, gui_elements_valve[i].manual_grp);
    ESPUI.setElementStyle(elt, label_style);
    value = String(settings_get_valve_force_off(i));
    gui_elements_valve[i].force_off_ctrl = ESPUI.addControl(Switcher, "", value, ControlColor::None, gui_elements_valve[i].manual_grp, gui_force_off_cb, (void*)i);

    // status
    gui_elements_valve[i].status_label = ESPUI.addControl(Label, "Statut", "OFF", ControlColor::Alizarin, Control::noParent);
    ESPUI.setElementStyle(gui_elements_valve[i].status_label, label_style);
  }

  // Check general control status
  if (settings_get_general_force_off()) {
    gui_general_force_off(true);
  }
  
  ESPUI.begin("Arrosage automatique");
}
