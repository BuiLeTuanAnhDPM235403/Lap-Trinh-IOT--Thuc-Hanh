#include <WiFi.h>
#include <PubSubClient.h>
#include "DHTesp.h"

const char* ssid = "Wokwi-GUEST";
const char* password = "";
const char* mqtt_server = "broker.hivemq.com";
const char* topic_temp = "iot_wokwi_demo_2026/temperature_bai1";

const int DHT_PIN = 15;
DHTesp dhtSensor;
WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}

void reconnect() {
  while (!client.connected()) {
    String clientId = "ESP32_Sender_Bai1_" + String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      Serial.println("Da ket noi HiveMQ!");
    } else {
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  dhtSensor.setup(DHT_PIN, DHTesp::DHT22);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  TempAndHumidity data = dhtSensor.getTempAndHumidity();
  float temp = data.temperature;

  if (!isnan(temp)) {
    String tempStr = String(temp, 1);
    client.publish(topic_temp, tempStr.c_str());
    Serial.println("Da gui nhiet do: " + tempStr + " C");
  }
  delay(2000);
}