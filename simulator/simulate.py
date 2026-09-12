"""Publish realistic environmental sensor readings to MQTT — no hardware needed.

Mirrors the ESP32 firmware's topics so the rest of the pipeline is identical
whether data comes from a real board or from this simulator.
"""
import os
import time
import math
import random
import json
import paho.mqtt.client as mqtt

HOST = os.environ.get("MQTT_HOST", "localhost")
PORT = int(os.environ.get("MQTT_PORT", "1883"))
ROOT = os.environ.get("TOPIC_ROOT", "devices")

def reading(t):
    """A plausible daily curve + noise for each sensor."""
    day = math.sin(t / 30.0)  # slow oscillation
    return {
        "dht11/temp": round(21 + 4 * day + random.uniform(-0.4, 0.4), 2),
        "dht11/hum": round(50 - 10 * day + random.uniform(-1, 1), 2),
        "bme680/temp": round(21.5 + 4 * day + random.uniform(-0.3, 0.3), 2),
        "bme680/pressure": round(1013 + random.uniform(-2, 2), 2),
        "bme680/hum": round(48 - 9 * day + random.uniform(-1, 1), 2),
        "bme680/gas": round(50 + 20 * random.random(), 2),
        "ldr/luminosite": round(max(0, 500 * (day + 1) + random.uniform(-20, 20)), 1),
    }

def main():
    client = mqtt.Client()
    client.connect(HOST, PORT, 60)
    print(f"Simulator publishing to {HOST}:{PORT} under '{ROOT}/'")
    t = 0
    while True:
        for sub, value in reading(t).items():
            client.publish(f"{ROOT}/{sub}", value)
        client.loop(timeout=0.1)
        t += 1
        time.sleep(2)

if __name__ == "__main__":
    main()
