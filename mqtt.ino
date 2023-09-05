/*
Auto watering MQTT mqtt_client implementation.
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

#ifdef USE_MQTT

#include <WiFi.h>
#include <PubSubClient.h>
#include "settings.h"

WiFiClient wifi_client;
PubSubClient mqtt_client(wifi_client);
uint32_t mqtt_settings_crc;

typedef struct mqtt_published_data_s {
  bool general_force_off;
  timeinfo_t cycle_start_time[CYCLES_NUMBER + 1];
  bool cycle_enabled[CYCLES_NUMBER + 1];
  int current_cycle;
  uint16_t valve_duration[VALVE_NUMBER];
  bool valve_force_on[VALVE_NUMBER];
  bool valve_force_off[VALVE_NUMBER];
  bool valves_state[VALVE_NUMBER];
} mqtt_published_data_t;
mqtt_published_data_t mqtt_published_data;

#define MQTT_TOPIC_NAME_FORCE_OFF "force_off"
#define MQTT_TOPIC_FORCE_OFF MQTT_DEVICE_NAME "/" MQTT_TOPIC_NAME_FORCE_OFF
#define MQTT_TOPIC_NAME_CYCLE_ENABLED "enabled_cycle"
#define MQTT_TOPIC_CYCLE_ENABLED MQTT_DEVICE_NAME "/" MQTT_TOPIC_NAME_CYCLE_ENABLED
#define MQTT_TOPIC_NAME_CYCLE_STATUS "status_cycle"
#define MQTT_TOPIC_CYCLE_STATUS MQTT_DEVICE_NAME "/" MQTT_TOPIC_NAME_CYCLE_STATUS
#define MQTT_TOPIC_NAME_CYCLE_HOUR "hour_cycle"
#define MQTT_TOPIC_CYCLE_HOUR MQTT_DEVICE_NAME "/" MQTT_TOPIC_NAME_CYCLE_HOUR
#define MQTT_TOPIC_NAME_CYCLE_MINUTE "minute_cycle"
#define MQTT_TOPIC_CYCLE_MINUTE MQTT_DEVICE_NAME "/" MQTT_TOPIC_NAME_CYCLE_MINUTE
#define MQTT_TOPIC_NAME_VALVE_FORCE_OFF "force_off_valve"
#define MQTT_TOPIC_VALVE_FORCE_OFF MQTT_DEVICE_NAME "/" MQTT_TOPIC_NAME_VALVE_FORCE_OFF
#define MQTT_TOPIC_NAME_VALVE_FORCE_ON "force_on_valve"
#define MQTT_TOPIC_VALVE_FORCE_ON MQTT_DEVICE_NAME "/" MQTT_TOPIC_NAME_VALVE_FORCE_ON
#define MQTT_TOPIC_NAME_VALVE_DURATION "duration_valve"
#define MQTT_TOPIC_VALVE_DURATION MQTT_DEVICE_NAME "/" MQTT_TOPIC_NAME_VALVE_DURATION
#define MQTT_TOPIC_NAME_VALVE_STATE "state_valve"
#define MQTT_TOPIC_VALVE_STATE MQTT_DEVICE_NAME "/" MQTT_TOPIC_NAME_VALVE_STATE
#define MQTT_TOPIC_SET_SUFFIX "/set"

void mqtt_publish_discovery_binary(const char *topic, const char *name)
{
#ifdef MQTT_DISCOVERY_TOPIC_PREFIX
  String discovery_topic = MQTT_DISCOVERY_TOPIC_PREFIX "/binary_sensor/" MQTT_DEVICE_NAME "_" + String(name) + "/config";
  String discovery_message = R"({
    "name": ")" + String(MQTT_DEVICE_NAME) + "_" + String(name) + R"(",
    "state_topic": ")" + String(topic) + R"(",
    "payload_on": "1",
    "payload_off": "0"
  })";
  mqtt_client.publish(discovery_topic.c_str(), discovery_message.c_str(), true);
#endif
}

void mqtt_publish_discovery_switch(const char *topic, const char *name)
{
#ifdef MQTT_DISCOVERY_TOPIC_PREFIX
  String discovery_topic = MQTT_DISCOVERY_TOPIC_PREFIX "/switch/" MQTT_DEVICE_NAME "_" + String(name) + "/config";
  String discovery_message = R"({
    "name": ")" + String(MQTT_DEVICE_NAME) + "_" + String(name) + R"(",
    "state_topic": ")" + String(topic) + R"(",
    "command_topic": ")" + String(topic) + R"(/set",
    "payload_on": "1",
    "payload_off": "0"
  })";
  mqtt_client.publish(discovery_topic.c_str(), discovery_message.c_str(), true);
#endif
}

void mqtt_publish_discovery_number(const char *topic, const char *name, int min = 0, int max = 100)
{
#ifdef MQTT_DISCOVERY_TOPIC_PREFIX
  String discovery_topic = MQTT_DISCOVERY_TOPIC_PREFIX "/number/" MQTT_DEVICE_NAME "_" + String(name) + "/config";
  String discovery_message = R"({
    "name": ")" + String(MQTT_DEVICE_NAME) + "_" + String(name) + R"(",
    "state_topic": ")" + String(topic) + R"(",
    "command_topic": ")" + String(topic) + R"(/set",
    "min": ")" + String(min) + R"(",
    "max": ")" + String(max) + R"(",
    "step": "1"
  })";
  mqtt_client.publish(discovery_topic.c_str(), discovery_message.c_str(), true);
#endif
}

void mqtt_receive(char *topic, byte *payload, unsigned int length)
{
  String topicStr = String(topic);
  String payloadStr = "";
  
  for (int i = 0; i < length; i++) {
    payloadStr += (char)payload[i];
  }
  Serial.println("Reveived MQTT update on " + topicStr + ": " + payloadStr);
  topicStr = topicStr.substring(0, topicStr.length() - strlen(MQTT_TOPIC_SET_SUFFIX));
  
  if (String(topicStr) == MQTT_TOPIC_FORCE_OFF) {
    settings_set_general_force_off(payloadStr == "1");
  } else if (topicStr.startsWith(MQTT_TOPIC_CYCLE_ENABLED)) {
    int index = topicStr.substring(strlen(MQTT_TOPIC_CYCLE_ENABLED)).toInt();
    settings_enable_cycle(index, payloadStr == "1");
  } else if (topicStr.startsWith(MQTT_TOPIC_CYCLE_HOUR)) {
    int index = topicStr.substring(strlen(MQTT_TOPIC_CYCLE_HOUR)).toInt();
    timeinfo_t start_time;
    settings_get_cycle_start_time(index, &start_time);
    start_time.hour = payloadStr.toInt();
    settings_set_cycle_start_time(index, start_time);
  } else if (topicStr.startsWith(MQTT_TOPIC_CYCLE_MINUTE)) {
    int index = topicStr.substring(strlen(MQTT_TOPIC_CYCLE_MINUTE)).toInt();
    timeinfo_t start_time;
    settings_get_cycle_start_time(index, &start_time);
    start_time.minute = payloadStr.toInt();
    settings_set_cycle_start_time(index, start_time);
  } else if (topicStr.startsWith(MQTT_TOPIC_VALVE_FORCE_OFF)) {
    int index = topicStr.substring(strlen(MQTT_TOPIC_VALVE_FORCE_OFF)).toInt();
    settings_set_valve_force_off(index, payloadStr == "1");
  } else if (topicStr.startsWith(MQTT_TOPIC_VALVE_FORCE_ON)) {
    int index = topicStr.substring(strlen(MQTT_TOPIC_VALVE_FORCE_ON)).toInt();
    settings_set_valve_force_on(index, payloadStr == "1");
  } else if (topicStr.startsWith(MQTT_TOPIC_VALVE_DURATION)) {
    int index = topicStr.substring(strlen(MQTT_TOPIC_VALVE_DURATION)).toInt();
    settings_set_valve_duration(index, payloadStr.toInt());
  }
}

void mqtt_publish_data(String topic, String payload)
{
  mqtt_client.publish(topic.c_str(), payload.c_str(), true);
  Serial.println("Published MQTT update on " + topic + ": " + payload);
}

void mqtt_publish_settings(bool force = false)
{
  String topic;
  String payload;
  mqtt_published_data_t new_published_data = mqtt_published_data;

  // Load new settings
  new_published_data.general_force_off = settings_get_general_force_off();
  for (int i = 0; i <= CYCLES_NUMBER; i++) {
    new_published_data.cycle_enabled[i] = settings_get_cycle_start_time(i, &(new_published_data.cycle_start_time[i]));
  }
  for (int i = 0; i < VALVE_NUMBER; i++) {
    new_published_data.valve_force_off[i] = settings_get_valve_force_off(i);
    new_published_data.valve_force_on[i] = settings_get_valve_force_on(i);
    new_published_data.valve_duration[i] = settings_get_valve_duration(i);
  }

  // Publish changes
  if (force || mqtt_published_data.general_force_off != new_published_data.general_force_off) {
    topic = String(MQTT_TOPIC_FORCE_OFF);
    payload = String(new_published_data.general_force_off);
    mqtt_publish_data(topic, payload);
  }
  // Cycles info
  for (int i = 0; i <= CYCLES_NUMBER; i++) {
    // enabled
    if (force || mqtt_published_data.cycle_enabled[i] != new_published_data.cycle_enabled[i]) {
      topic = String(MQTT_TOPIC_CYCLE_ENABLED) + String(i);
      payload = String(new_published_data.cycle_enabled[i]);
      mqtt_publish_data(topic, payload);
    }
    // time
    if (force || mqtt_published_data.cycle_start_time[i].hour != new_published_data.cycle_start_time[i].hour) {
      topic = String(MQTT_TOPIC_CYCLE_HOUR) + String(i);
      payload = String(new_published_data.cycle_start_time[i].hour);
      mqtt_publish_data(topic, payload);
    }
    if (force || mqtt_published_data.cycle_start_time[i].minute != new_published_data.cycle_start_time[i].minute) {
      topic = String(MQTT_TOPIC_CYCLE_MINUTE) + String(i);
      payload = String(new_published_data.cycle_start_time[i].minute);
      mqtt_publish_data(topic, payload);
    }
  }
  // Valves info
  for (int i = 0; i < VALVE_NUMBER; i++) {
    // force OFF
    if (force || mqtt_published_data.valve_force_off[i] != new_published_data.valve_force_off[i]) {
      topic = String(MQTT_TOPIC_VALVE_FORCE_OFF) + String(i);
      payload = String(new_published_data.valve_force_off[i]);
      mqtt_publish_data(topic, payload);
    }
    // force ON
    if (force || mqtt_published_data.valve_force_on[i] != new_published_data.valve_force_on[i]) {
      topic = String(MQTT_TOPIC_VALVE_FORCE_ON) + String(i);
      payload = String(new_published_data.valve_force_on[i]);
      mqtt_publish_data(topic, payload);
    }
    // duration
    if (force || mqtt_published_data.valve_duration[i] != new_published_data.valve_duration[i]) {
      topic = String(MQTT_TOPIC_VALVE_DURATION) + String(i);
      payload = String(new_published_data.valve_duration[i]);
      mqtt_publish_data(topic, payload);
    }
  }

  // Update published data cache
  mqtt_published_data = new_published_data;
}

void mqtt_publish_status(bool force = false)
{
  String topic;
  String payload;
  mqtt_published_data_t new_published_data = mqtt_published_data;

  // Get new status
  new_published_data.current_cycle = get_current_cycle();
  get_valves_state(new_published_data.valves_state);

  // Publish changes
  // Cycles status
  for (int i = 0; i <= CYCLES_NUMBER; i++) {
    if (force || mqtt_published_data.current_cycle != new_published_data.current_cycle) {
      topic = String(MQTT_TOPIC_CYCLE_STATUS) + String(i);
      payload = String(i == new_published_data.current_cycle);
      mqtt_publish_data(topic, payload);
    }
  }
  // Valves states
  for (int i = 0; i < VALVE_NUMBER; i++) {
    if (force || mqtt_published_data.valves_state[i] != new_published_data.valves_state[i]) {
      topic = String(MQTT_TOPIC_VALVE_STATE) + String(i);
      payload = String(new_published_data.valves_state[i]);
      mqtt_publish_data(topic, payload);
    }
  }

  // Update published data cache
  mqtt_published_data = new_published_data;
}

bool mqtt_connect(void)
{
  if (mqtt_client.connect(MQTT_DEVICE_NAME, MQTT_USER, MQTT_PASSWORD)) {
    // Subscribe on events and publish on discovery topic
    mqtt_client.subscribe(MQTT_TOPIC_FORCE_OFF MQTT_TOPIC_SET_SUFFIX);
    mqtt_publish_discovery_switch(MQTT_TOPIC_FORCE_OFF, MQTT_TOPIC_NAME_FORCE_OFF);
    // Cycles info
    for (int i = 0; i <= CYCLES_NUMBER; i++) {
      String topic, name;
      // enabled
      topic = String(MQTT_TOPIC_CYCLE_ENABLED) + String(i);
      name = String(MQTT_TOPIC_NAME_CYCLE_ENABLED) + String(i);
      mqtt_client.subscribe((topic + MQTT_TOPIC_SET_SUFFIX).c_str());
      mqtt_publish_discovery_switch(topic.c_str(), name.c_str());
      // status
      topic = String(MQTT_TOPIC_CYCLE_STATUS) + String(i);
      name = String(MQTT_TOPIC_NAME_CYCLE_STATUS) + String(i);
      mqtt_publish_discovery_binary(topic.c_str(), name.c_str());
      // time
      topic = String(MQTT_TOPIC_CYCLE_HOUR) + String(i);
      name = String(MQTT_TOPIC_NAME_CYCLE_HOUR) + String(i);
      mqtt_client.subscribe((topic + MQTT_TOPIC_SET_SUFFIX).c_str());
      mqtt_publish_discovery_number(topic.c_str(), name.c_str(), 0, 23);
      topic = String(MQTT_TOPIC_CYCLE_MINUTE) + String(i);
      name = String(MQTT_TOPIC_NAME_CYCLE_MINUTE) + String(i);
      mqtt_client.subscribe((topic + MQTT_TOPIC_SET_SUFFIX).c_str());
      mqtt_publish_discovery_number(topic.c_str(), name.c_str(), 0, 59);
    }
    // Valves info
    for (int i = 0; i < VALVE_NUMBER; i++) {
      String topic, name;
      // force OFF
      topic = String(MQTT_TOPIC_VALVE_FORCE_OFF) + String(i);
      name = String(MQTT_TOPIC_NAME_VALVE_FORCE_OFF) + String(i);
      mqtt_client.subscribe((topic + MQTT_TOPIC_SET_SUFFIX).c_str());
      mqtt_publish_discovery_switch(topic.c_str(), name.c_str());
      // force ON
      topic = String(MQTT_TOPIC_VALVE_FORCE_ON) + String(i);
      name = String(MQTT_TOPIC_NAME_VALVE_FORCE_ON) + String(i);
      mqtt_client.subscribe((topic + MQTT_TOPIC_SET_SUFFIX).c_str());
      mqtt_publish_discovery_switch(topic.c_str(), name.c_str());
      // duration
      topic = String(MQTT_TOPIC_VALVE_DURATION) + String(i);
      name = String(MQTT_TOPIC_NAME_VALVE_DURATION) + String(i);
      mqtt_client.subscribe((topic + MQTT_TOPIC_SET_SUFFIX).c_str());
      mqtt_publish_discovery_number(topic.c_str(), name.c_str(), 0, VALVE_MAX_OPENED_DURATION);
      // state
      topic = String(MQTT_TOPIC_VALVE_STATE) + String(i);
      name = String(MQTT_TOPIC_NAME_VALVE_STATE) + String(i);
      mqtt_publish_discovery_binary(topic.c_str(), name.c_str());
    }
    return true;
  } else {
    return false;
  }
}

void mqtt_start(void)
{
  mqtt_client.setServer(MQTT_SERVER, MQTT_PORT);
  mqtt_client.setCallback(mqtt_receive);
  Serial.print("Connecting to MQTT broker... ");
  if (mqtt_connect()) {
    Serial.println("connected");
    mqtt_publish_settings(true);
    mqtt_publish_status(true);
  } else {
    Serial.print("failed, rc=");
    Serial.println(mqtt_client.state());
  }
  mqtt_settings_crc = settings_get_crc();
}

void mqtt_refresh(void)
{
  if (!mqtt_client.connected()) {
    mqtt_connect();
  }
  mqtt_client.loop();

  // Publish changes on settings and status
  if (settings_changed(&mqtt_settings_crc)) {
    mqtt_publish_settings();
  }
  mqtt_publish_status();
}

#endif
