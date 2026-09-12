// librairies
#include <Arduino.h>
#include "DHT.h"
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_BME680.h"
#include <WiFi.h>
#include <esp_wpa2.h>
#include <WiFiClient.h>
#include <PubSubClient.h>


#include "secrets.h"   // WiFi + MQTT credentials (see secrets.h.example)

static WiFiClient wifiClient;
static PubSubClient mqttClient(MQTT_SERVER, MQTT_PORT, wifiClient);



// pins 
#define DHT11PIN 32
#define LIGHT_SENSOR_PIN 34
#define BME_SCK 18
#define BME_MISO 19
#define BME_MOSI 23
#define BME_CS 5

#define SEALEVELPRESSURE_HPA (1013.25)

// variables
Adafruit_BME680 bme(BME_CS, BME_MOSI, BME_MISO, BME_SCK);

DHT dht(DHT11PIN, DHT11);

const int tempPin = 35; 

int tempValeur;    
float volt;     
float temp;     

void setup()
{
  
  Serial.begin(9600);

  dht.begin();

  
  while (!Serial);
  Serial.println(F("BME680 async test"));

  if (!bme.begin()) {
    Serial.println(F("Could not find a valid BME680 sensor, check wiring!"));
    while (1);
  }

  
  bme.setTemperatureOversampling(BME680_OS_8X);
  bme.setHumidityOversampling(BME680_OS_2X);
  bme.setPressureOversampling(BME680_OS_4X);
  bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
  bme.setGasHeater(320, 150); 

  WiFi.begin(SSID, WPA2_AUTH_PEAP, LOGIN, LOGIN, PASSPHRASE);
 while (WiFi.status() != WL_CONNECTED) {
 Serial.print(".");
 delay(100);
 }
 Serial.print(F("\nIP address: "));
 Serial.println(WiFi.localIP());

 boolean conn = mqttClient.connect("hems-l517", MQTT_USERNAME, MQTT_PASSWORD);
  if (conn) {
    Serial.println(F("MQTT connection success"));
} else {
    Serial.println(F("MQTT connection failed"));
}

}

void loop()
{
  //read dht11
  float humi = dht.readHumidity();
  float temp = dht.readTemperature();
  Serial.print("DHT 11 Temperature: ");
  Serial.print(temp);
  Serial.println(" ºC ");
  Serial.print("DHT 11 Humidity: ");
  Serial.print(humi);
  Serial.println(" % ");
  delay(2000);

//read tmp36
tempValeur = analogRead(tempPin);
volt = tempValeur/1023.0;             
temp = (volt - 0.5) * 100 ;         
Serial.print("TMP36 Temperature : "); 
Serial.print(temp);                  
Serial.println (" °C");       
delay(2000); 

// read light sensor
int analogValue = analogRead(LIGHT_SENSOR_PIN);

  Serial.print("LDR : ");
  Serial.print(analogValue);   

  
  if (analogValue < 40) {
    Serial.println(" => Dark");
  } else if (analogValue < 800) {
    Serial.println(" => Dim");
  } else if (analogValue < 2000) {
    Serial.println(" => Light");
  } else if (analogValue < 3200) {
    Serial.println(" => Bright");
  } else {
    Serial.println(" => Very bright");
  }

  delay(2000);


  // BME680 sensor
  unsigned long endTime = bme.beginReading();
  if (endTime == 0) {
    Serial.println(F("Failed to begin reading :("));
    return;
  }
  
  if (!bme.endReading()) {
    Serial.println(F("Failed to complete reading :("));
    return;
  }

  

  Serial.print(F("BME 680 Temperature : "));
  Serial.print(bme.temperature);
  Serial.println(F(" °C"));

  Serial.print(F("BME 680 pressure : "));
  Serial.print(bme.pressure / 100.0);
  Serial.println(F(" hPa"));

  Serial.print(F("BME 680 humidity : "));
  Serial.print(bme.humidity);
  Serial.println(F(" %"));

  Serial.print(F("BME 680 gas : "));
  Serial.print(bme.gas_resistance / 1000.0);
  Serial.println(F(" KOhms"));

  Serial.print(F("BME 680 altitude : "));
  Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
  Serial.println(F(" m"));

  Serial.println();
  delay(2000);

  // MQTT
  mqttClient.connect(MQTT_ID, MQTT_USERNAME, MQTT_PASSWORD);
  


  mqttClient.publish("your-topic-root/dht11/temp", String(temp).c_str());
  mqttClient.publish("your-topic-root/dht11/hum", String(humi).c_str());
  mqttClient.publish("your-topic-root/tmp36/temp", String(temp).c_str());
  mqttClient.publish("your-topic-root/ldr/luminosité", String(analogValue).c_str());
  mqttClient.publish("your-topic-root/bme680/temp", String(bme.temperature).c_str());
  mqttClient.publish("your-topic-root/bme680/pressure", String(bme.pressure / 100.0).c_str());
  mqttClient.publish("your-topic-root/bme680/hum", String(bme.humidity).c_str());
  mqttClient.publish("your-topic-root/bme680/gas", String(bme.gas_resistance / 1000.0).c_str());
  mqttClient.publish("your-topic-root/bme680/altitude", String(bme.readAltitude(SEALEVELPRESSURE_HPA)).c_str());

}