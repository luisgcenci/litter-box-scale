#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "HX711.h"
#include "secrets.h"

const int DT_PIN = D2;
const int SCK_PIN = D4;
const int TARE_SAMPLES = 30;
const int READING_SAMPLES = 30;
const float CALIBRATION_FACTOR = 10.72;
const int MQTT_PORT = 1883;
const char* TOPIC = "cat/readings";

HX711 scale;
WiFiClient wifiClient;
PubSubClient mqtt(wifiClient);

void connectWifi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print("\nConnected, IP: ");
  Serial.println(WiFi.localIP());
}

void connectMqtt() {
  while (!mqtt.connected()) {
    Serial.print("Connecting to MQTT... ");
    if (mqtt.connect("xiao-scale", "cat/status", 1, true, "offline")) {
      mqtt.publish("cat/status", "online", true);
      Serial.println("connected");
    } else {
      Serial.print("failed, state=");
      Serial.println(mqtt.state());
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000); // give USB serial a moment to start

  scale.begin(DT_PIN, SCK_PIN);
  scale.set_scale(CALIBRATION_FACTOR);
  scale.tare(TARE_SAMPLES); // scale must be empty at startup

  connectWifi();
  mqtt.setServer(MQTT_HOST, MQTT_PORT);
  connectMqtt();
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) connectWifi();
  if (!mqtt.connected()) connectMqtt();
  mqtt.loop();

  float weight = scale.get_units(READING_SAMPLES);
  char payload[16];
  snprintf(payload, sizeof(payload), "%.1f", weight);
  mqtt.publish(TOPIC, payload);
  Serial.println(payload);
  delay(1000);
}
