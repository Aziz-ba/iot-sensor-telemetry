"""Subscribe to the sensor topics and write each reading into InfluxDB."""
import os
from influxdb import InfluxDBClient
import paho.mqtt.client as mqtt

MQTT_HOST = os.environ.get("MQTT_HOST", "localhost")
MQTT_PORT = int(os.environ.get("MQTT_PORT", "1883"))
ROOT = os.environ.get("TOPIC_ROOT", "devices")
INFLUX_HOST = os.environ.get("INFLUXDB_HOST", "localhost")
INFLUX_DB = os.environ.get("INFLUXDB_DATABASE", "sensors")

influx = InfluxDBClient(host=INFLUX_HOST, port=8086, database=INFLUX_DB)
influx.create_database(INFLUX_DB)

def on_connect(client, userdata, flags, rc):
    client.subscribe(f"{ROOT}/#")
    print("Bridge subscribed to", f"{ROOT}/#")

def on_message(client, userdata, msg):
    sensor = msg.topic.split("/", 1)[-1]
    try:
        value = float(msg.payload)
    except ValueError:
        return
    influx.write_points([{ "measurement": sensor, "fields": { "value": value } }])

client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message
client.connect(MQTT_HOST, MQTT_PORT, 60)
client.loop_forever()
