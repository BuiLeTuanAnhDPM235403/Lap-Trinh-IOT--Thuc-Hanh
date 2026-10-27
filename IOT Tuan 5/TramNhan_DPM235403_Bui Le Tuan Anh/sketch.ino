#include <WiFi.h>
#include <PubSubClient.h>
#include <LiquidCrystal_I2C.h>

const char* ssid = "Wokwi-GUEST";
const char* password = "";
const char* mqtt_server = "broker.hivemq.com";
const char* topic_temp = "iot_wokwi_demo_2026/temperature_bai1";

const int LED_PIN = 2;
LiquidCrystal_I2C lcd(0x27, 16, 2);
WiFiClient espClient;
PubSubClient client(espClient);

void callback(char* topic, byte* payload, unsigned int length) {
  String message = "";
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  float temp = message.toFloat();
  Serial.println("Nhan duoc nhiet do: " + String(temp, 1) + " C");
  
  // Nhiệt độ > 35°C đèn sáng, <= 35°C đèn tắt
  if (temp > 35.0) {
    digitalWrite(LED_PIN, HIGH);
  } else {
    digitalWrite(LED_PIN, LOW);
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp: " + String(temp, 1) + " C");
  lcd.setCursor(0, 1);
  lcd.print(temp > 35.0 ? "Den: ON (Sang)" : "Den: OFF (Tat)");
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Dang ket noi HiveMQ...");
    String clientId = "ESP32_Receiver_Bai1_" + String(random(0xffff), HEX);
    
    if (client.connect(clientId.c_str())) {
      Serial.println("Thanh cong!");
      client.subscribe(topic_temp);
      
      // Báo ngay lên LCD khi kết nối MQTT thành công
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("MQTT Connected!");
      lcd.setCursor(0, 1);
      lcd.print("Waiting data...");
    } else {
      Serial.print("Loi, rc=");
      Serial.println(client.state());
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Connecting WiFi");

  WiFi.begin(ssid, password, 6); // Thêm kênh 6 giúp Wokwi bắt WiFi tức thì
  while (WiFi.status() != WL_CONNECTED) {
    delay(200);
    Serial.print(".");
  }
  Serial.println("\nWiFi Connected!");

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Connecting MQTT");

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}