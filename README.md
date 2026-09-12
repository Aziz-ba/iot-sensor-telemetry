# 🌡️ IoT Sensor Telemetry — ESP32 → MQTT → InfluxDB

An end-to-end **IoT telemetry pipeline**: an **ESP32** reads environmental sensors, publishes the readings over **MQTT**, and a Python bridge stores them in **InfluxDB** — ready for time-series dashboards.

A small but complete slice of the data journey, from a physical sensor all the way to a queryable time-series database.

---

## 🔌 Architecture

```
  ┌─────────── ESP32 (Arduino) ───────────┐        MQTT           ┌──────────────┐   InfluxDB    ┌─────────────┐
  │  BME680 (temp/press/hum/gas)           │      publish/         │  MQTT broker │   line write  │  InfluxDB   │
  │  DHT11  (temp/hum)                      │─────  subscribe  ────▶│              │──────────────▶│ time-series │
  │  TMP36 / LDR (temp / light)            │     (WiFi WPA2-Ent)   │              │               │     DB      │
  └────────────────────────────────────────┘                       └──────────────┘               └─────────────┘
                                                                     mqtt2influxdb.py (Python bridge)
```

---

## 🧩 Components

| File | Role |
|------|------|
| [`src/main.cpp`](src/main.cpp) | ESP32 firmware — reads BME680 + DHT11 + analog sensors, connects to WiFi (WPA2-Enterprise) and publishes each reading to its own MQTT topic |
| [`mqtt2influxdb.py`](mqtt2influxdb.py) | Subscribes to the MQTT topics and writes the parsed values into InfluxDB |
| [`platformio.ini`](platformio.ini) | PlatformIO build config (`esp32dev`, Adafruit BME680 / DHT / PubSubClient) |
| `src/secrets.h.example` | Template for credentials — copy to `secrets.h` (git-ignored) |

**Sensors:** BME680 (temperature, pressure, humidity, gas/air-quality), DHT11 (temperature, humidity), TMP36 (temperature), LDR (luminosity).

---

## 🚀 Getting started

**1. Firmware (PlatformIO)**
```bash
cd iot-sensor-telemetry
cp src/secrets.h.example src/secrets.h   # fill in your WiFi + MQTT credentials
pio run --target upload                   # flash the ESP32
pio device monitor
```

**2. MQTT → InfluxDB bridge**
```bash
pip install influxdb paho-mqtt
export MQTT_USERNAME=... MQTT_PASSWORD=... MQTT_HOST=...
python mqtt2influxdb.py
```

> 🔒 No credentials are committed. WiFi/MQTT secrets live only in `secrets.h` (firmware) and environment variables (bridge), both git-ignored.

---

## 🛠️ Tech Stack

![ESP32](https://img.shields.io/badge/ESP32-000000?style=flat-square&logo=espressif&logoColor=white)
![C++](https://img.shields.io/badge/C++-00599C?style=flat-square&logo=cplusplus&logoColor=white)
![PlatformIO](https://img.shields.io/badge/PlatformIO-FF7F00?style=flat-square&logo=platformio&logoColor=white)
![MQTT](https://img.shields.io/badge/MQTT-660066?style=flat-square&logo=mqtt&logoColor=white)
![InfluxDB](https://img.shields.io/badge/InfluxDB-22ADF6?style=flat-square&logo=influxdb&logoColor=white)
![Python](https://img.shields.io/badge/Python-3776AB?style=flat-square&logo=python&logoColor=white)

ESP32 · Arduino/C++ · PlatformIO · BME680 / DHT · MQTT (PubSubClient) · InfluxDB · Python (paho-mqtt)

---

## 🐳 Run the whole stack — no hardware required

Don't have an ESP32 on hand? The [`stack/`](stack/) folder spins up the **entire pipeline** in Docker, with a **software sensor simulator** standing in for the board:

```bash
cd stack
docker compose up --build
```

This starts five services:

| Service | Role |
|---------|------|
| **mosquitto** | MQTT broker (port 1883) |
| **simulator** | publishes realistic BME680/DHT/LDR readings to MQTT (replaces the ESP32) |
| **bridge** | subscribes to MQTT and writes readings into InfluxDB |
| **influxdb** | time-series database (`sensors` DB, port 8086) |
| **grafana** | dashboards at http://localhost:3000 (InfluxDB datasource pre-provisioned) |

So the same **device → broker → bridge → time-series DB → dashboard** flow runs end to end on any laptop. Swap the `simulator` for the real firmware ([`src/main.cpp`](src/main.cpp)) and nothing else changes.

The simulator ([`simulator/simulate.py`](simulator/simulate.py)) generates a plausible daily curve (a slow sine + noise) per sensor, publishing to the same topics the firmware uses.

---

## 📚 What this project demonstrates

- Building an **end-to-end IoT data pipeline** (device → broker → database)
- Reading and calibrating multiple **hardware sensors** on an ESP32
- **MQTT** publish/subscribe messaging over secure WiFi
- Ingesting streaming data into a **time-series database** for analytics

---

## 📄 License

Released under the [MIT License](LICENSE).
