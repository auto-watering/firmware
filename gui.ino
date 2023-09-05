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
#include "i18n.h"
#include "timedate.h"
#include "settings.h"
#include "valves.h"

typedef struct gui_elements_valve_s {
  String valve_str;
  uint16_t duration_ctrl, force_on_ctrl, force_off_ctrl;
  uint16_t duration_grp, manual_grp;
  uint16_t status_label;
} gui_elements_valve_t;
gui_elements_valve_t gui_elements_valve[VALVE_NUMBER];

typedef struct gui_elements_s {
  String start_time_str[CYCLES_NUMBER];
  uint16_t start_time_ctrl[CYCLES_NUMBER];
  uint16_t cycle_enable_ctrl[CYCLES_NUMBER];
  uint16_t manuel_cycle_enable_ctrl;
  uint16_t general_force_off_ctrl;
  uint16_t schedule_grp, general_manual_grp;
  uint16_t date_label;
  uint16_t status_label;
  String status_str;
} gui_elements_t;
gui_elements_t gui_elements;

uint32_t gui_settings_crc;

int last_current_cycle = -1;

void gui_status_change(bool valves_state[VALVE_NUMBER])
{
  int current_cycle;

  // Valves states
  for (int id = 0; id < VALVE_NUMBER; id++) {
    if (valves_state[id]) {
      ESPUI.getControl(gui_elements_valve[id].status_label)->color = ControlColor::Emerald;
      ESPUI.updateControlValue(gui_elements_valve[id].status_label, gettext("ON"));
    } else {
      ESPUI.getControl(gui_elements_valve[id].status_label)->color = ControlColor::Alizarin;
      ESPUI.updateControlValue(gui_elements_valve[id].status_label, gettext("OFF"));
    }
  }

  // Current cycle
  current_cycle = get_current_cycle();
  if (current_cycle != -1) {
    ESPUI.getControl(gui_elements.status_label)->color = ControlColor::Emerald;
    if (current_cycle == 0) {
      gui_elements.status_str = gettext("Manual cycle in progress");
    } else {
      gui_elements.status_str = gettext("Cycle ") + String(current_cycle) + gettext(" in progress");
    }
  } else {
    ESPUI.getControl(gui_elements.status_label)->color = ControlColor::Alizarin;
    gui_elements.status_str = gettext("No active cycle");
  }
  if (last_current_cycle == 0 && current_cycle != 0) {
    // end of manual cycle
    ESPUI.updateControlValue(gui_elements.manuel_cycle_enable_ctrl, String(0));
  }
  ESPUI.updateControlValue(gui_elements.status_label, gui_elements.status_str);
  last_current_cycle = current_cycle;
}

void gui_start_time_cb(Control *sender, int type, void* user_data)
{
  int index = (int)user_data;
  Serial.print("Start time ");
  Serial.print(index);
  Serial.print(" changed");
  Serial.print(": ");
  Serial.println(sender->value);
  timeinfo_t start_time = str_to_timeinfo_t(sender->value);
  settings_set_cycle_start_time(index, start_time);
}

void gui_cycle_enable_cb(Control *sender, int type, void* user_data)
{
  int index = (int)user_data;
  bool enabled = sender->value.toInt();
  settings_enable_cycle(index, enabled);
  ESPUI.setEnabled(gui_elements.start_time_ctrl[index - 1], enabled);
}

void gui_manual_cycle_enable_cb(Control *sender, int type)
{
  bool enabled = sender->value.toInt();
  timeinfo_t start_time;
  time_get(&start_time);
  settings_set_cycle_start_time(0, start_time);
  settings_enable_cycle(0, enabled);
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
}

void gui_force_off(int id, bool enable)
{
  ESPUI.setEnabled(gui_elements_valve[id].force_on_ctrl, !enable);
  ESPUI.updateControlValue(gui_elements_valve[id].force_off_ctrl, String(enable));
  ESPUI.updateControlValue(gui_elements_valve[id].force_on_ctrl, "0");
  settings_set_valve_force_off(id, enable);
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

void gui_sync_settings(bool force = false)
{
  String value;
  bool enabled;

  if (!force && !settings_changed(&gui_settings_crc)) {
    return;
  }

  // Schedule
  // cycles start time (ignore cycle 0 here, which is manual cycle)
  for (int i = 1; i <= CYCLES_NUMBER; i++) {
    timeinfo_t start_time;
    bool cycle_enabled = settings_get_cycle_start_time(i, &start_time);
    
    // switcher to enable / disable this cycle
    ESPUI.updateControlValue(gui_elements.cycle_enable_ctrl[i - 1], String(cycle_enabled));
    
    // start time input area
    value = timeinfo_t_to_str(start_time);
    ESPUI.updateControlValue(gui_elements.start_time_ctrl[i - 1], value);
    ESPUI.setEnabled(gui_elements.start_time_ctrl[i - 1], cycle_enabled);
  }
  // manual cycle (cycle 0)
  // (manual cycle is disabled automatically when it ends)
  enabled = settings_is_cycle_enabled(0);
  ESPUI.updateControlValue(gui_elements.manuel_cycle_enable_ctrl, String(enabled));

  // Manual general control
  value = String(settings_get_general_force_off());
  ESPUI.updateControlValue(gui_elements.general_force_off_ctrl, value);
  if (settings_get_general_force_off()) {
    gui_general_force_off(true);
  }

  // Valves settings
  for (int i = 0; i < VALVE_NUMBER; i++) {
    // open duration
    ESPUI.updateSlider(gui_elements_valve[i].duration_ctrl, settings_get_valve_duration(i));
    
    // manual control
    value = String(settings_get_valve_force_on(i));
    ESPUI.updateControlValue(gui_elements_valve[i].force_on_ctrl, value);
    value = String(settings_get_valve_force_off(i));
    ESPUI.updateControlValue(gui_elements_valve[i].force_off_ctrl, value);
  }
}

void gui_refresh()
{
  String date = time_get_formatted();
  bool *valves_state = get_valves_state();

  // Date
  gui_set_date(date);

  // Settings
  gui_sync_settings();

  // Status
  gui_status_change(valves_state);
}

void gui_newline(uint16_t parent)
{
  String newline_style = "background-color: unset;";
  uint16_t elt = ESPUI.addControl(Label, "", "", ControlColor::None, parent);
  ESPUI.setElementStyle(elt, newline_style);
}

void gui_start(void)
{
  uint16_t elt;
  
  //ESPUI.setVerbosity(Verbosity::VerboseJSON);
  
  String label_style = "background-color: unset; margin-bottom: 0em; display: inline";
  String time_style = "width: auto; color: black";

  // Schedule
  // current date
  gui_elements.date_label = ESPUI.addControl(Label, gettext("Scheduling"), "", ControlColor::Peterriver, Control::noParent);
  ESPUI.setElementStyle(gui_elements.date_label, label_style);
  gui_elements.schedule_grp = gui_elements.date_label;
  
  // cycles start time (ignore cycle 0 here, which is manual cycle)
  for (int i = 1; i <= CYCLES_NUMBER; i++) {
    gui_newline(gui_elements.schedule_grp);
    
    // switcher to enable / disable this cycle
    gui_elements.cycle_enable_ctrl[i - 1] = ESPUI.addControl(Switcher, "", "0", ControlColor::None, gui_elements.schedule_grp, gui_cycle_enable_cb, (void*)i);
    
    // label
    gui_elements.start_time_str[i - 1] = gettext("Cycle ") + String(i);
    elt = ESPUI.addControl(Label, "", gui_elements.start_time_str[i - 1].c_str(), ControlColor::None, gui_elements.schedule_grp);
    ESPUI.setElementStyle(elt, label_style);
    
    // start time input area
    gui_elements.start_time_ctrl[i - 1] = ESPUI.addControl(Text, "", "00:00", ControlColor::None, gui_elements.schedule_grp, gui_start_time_cb, (void*)i);
    ESPUI.setElementStyle(gui_elements.start_time_ctrl[i - 1], time_style);
    ESPUI.setInputType(gui_elements.start_time_ctrl[i - 1], "time");
  }
  // manual cycle
  gui_newline(gui_elements.schedule_grp);
  gui_elements.manuel_cycle_enable_ctrl = ESPUI.addControl(Switcher, "", "0", ControlColor::None, gui_elements.schedule_grp, gui_manual_cycle_enable_cb);
  elt = ESPUI.addControl(Label, "", gettext("Manual cycle"), ControlColor::None, gui_elements.schedule_grp);
  ESPUI.setElementStyle(elt, label_style);

  // Manual general control
  elt = ESPUI.addControl(Label, gettext("General manual control"), gettext("Force OFF"), ControlColor::Peterriver, Control::noParent);
  ESPUI.setElementStyle(elt, label_style);
  gui_elements.general_manual_grp = elt;
  gui_elements.general_force_off_ctrl = ESPUI.addControl(Switcher, "", "0", ControlColor::None, gui_elements.general_manual_grp, gui_general_force_off_cb);

  // General status
  gui_elements.status_str = gettext("No active cycle");
  gui_elements.status_label = ESPUI.addControl(Label, gettext("Active cycle"), gui_elements.status_str, ControlColor::Alizarin, Control::noParent);
  ESPUI.setElementStyle(gui_elements.status_label, label_style);

  // Valves settings
  for (int i = 0; i < VALVE_NUMBER; i++) {
    gui_elements_valve[i].valve_str = gettext("Valve ") + String(i + 1);
    ESPUI.separator(gui_elements_valve[i].valve_str.c_str());
    
    // open duration
    gui_elements_valve[i].duration_grp = ESPUI.addControl(Label, gettext("Opening duration"), "", ControlColor::Peterriver, Control::noParent);
    ESPUI.setElementStyle(gui_elements_valve[i].duration_grp, label_style);
    ESPUI.setElementStyle(ESPUI.addControl(Label, "", gettext("minutes"), ControlColor::Peterriver, gui_elements_valve[i].duration_grp), label_style);
    gui_elements_valve[i].duration_ctrl = ESPUI.addControl(Slider, "", "0", ControlColor::None, gui_elements_valve[i].duration_grp, gui_duration_cb, (void*)i);
    ESPUI.addControl(Min, "", "0", None, gui_elements_valve[i].duration_ctrl);
    ESPUI.addControl(Max, "", String(VALVE_MAX_OPENED_DURATION), None, gui_elements_valve[i].duration_ctrl);
    
    // manual control
    elt = ESPUI.addControl(Label, gettext("Manual control"), gettext("Force ON"), ControlColor::Peterriver, Control::noParent);
    ESPUI.setElementStyle(elt, label_style);
    gui_elements_valve[i].manual_grp = elt;
    gui_elements_valve[i].force_on_ctrl = ESPUI.addControl(Switcher, "", "0", ControlColor::None, gui_elements_valve[i].manual_grp, gui_force_on_cb, (void*)i);
    elt = ESPUI.addControl(Label, "", gettext("Force OFF"), ControlColor::Peterriver, gui_elements_valve[i].manual_grp);
    ESPUI.setElementStyle(elt, label_style);
    gui_elements_valve[i].force_off_ctrl = ESPUI.addControl(Switcher, "", "0", ControlColor::None, gui_elements_valve[i].manual_grp, gui_force_off_cb, (void*)i);

    // status
    gui_elements_valve[i].status_label = ESPUI.addControl(Label, gettext("Status"), gettext("OFF"), ControlColor::Alizarin, Control::noParent);
    ESPUI.setElementStyle(gui_elements_valve[i].status_label, label_style);
  }
  
  ESPUI.begin(gettext("Automatic watering"));

  gui_sync_settings(true);
  gui_settings_crc = settings_get_crc();
}
