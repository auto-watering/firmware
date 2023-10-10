# Auto Watering

Below are detailed two methods to work on this project:
- Arduino IDE
- PlatformIO

But first of all, you have to configure your project.
Use `config.h.example` to set your `config.h` file with the right options for
you.

If using MQTT with HomeAssistant, an example dashboard code for two cycles and
two valves is provided in `HomeAssistant-dashboard-example.yaml`.

Arduino IDE
===========

In Arduino IDE, use "ESP32 Dev Module" card.

Required librairies:
- AsyncTCP
- ArduinoJson
- ESPAsyncWebServer (https://github.com/me-no-dev/ESPAsyncWebServer, download and install from a ZIP file)
- ESPUI
- CRC32
- PubSubClient (only if you plan to use MQTT client)

PlatformIO
==========

platformio run
platformio run -t nobuild -t upload

