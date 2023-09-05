In Arduino IDE, use "ESP32 Dev Module" card.

Required librairies:
- AsyncTCP
- ArduinoJson
- ESPAsyncWebServer (https://github.com/me-no-dev/ESPAsyncWebServer, download and install from a ZIP file)
- ESPUI
- CRC32
- PubSubClient (only if you plan to use MQTT client)

Use `config.h.example` to set your `config.h` file.

If using MQTT with HomeAssistant, an example dashboard code for two cycles and two valves is provided in `HomeAssistant-dashboard-example.yaml`.

