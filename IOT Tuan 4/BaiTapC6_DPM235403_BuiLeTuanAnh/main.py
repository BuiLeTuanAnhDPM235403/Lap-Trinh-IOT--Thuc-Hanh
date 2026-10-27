#include <WiFi.h>

const char* ssid = "Wokwi-GUEST";
const char* password = "";

const char* host = "api.thingspeak.com";

// PHẢI LÀ WRITE API KEY
const char* writeAPIKey = "TYKRKMYOKE2TH102";

void setup() {
    Serial.begin(115200);
    delay(1000);

    Serial.println();
    Serial.println("=== ESP32 + ThingSpeak ===");

    Serial.print("Dang ket noi WiFi: ");
    Serial.println(ssid);

    // Channel 6 giúp Wokwi bỏ qua bước scan WiFi
    WiFi.begin(ssid, password, 6);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println();
    Serial.println("Da ket noi WiFi");

    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
}

void loop() {

    // ThingSpeak Free: tối thiểu 15 giây/update
    delay(16000);

    Serial.println();
    Serial.println("================================");

    Serial.print("Dang ket noi den: ");
    Serial.println(host);

    WiFiClient client;

    const int httpPort = 80;

    if (!client.connect(host, httpPort)) {
        Serial.println("LOI: Khong ket noi duoc ThingSpeak!");
        return;
    }

    Serial.println("Da ket noi ThingSpeak");

    // Gia tri test
    int value = 123;

    // Tao URL
    String url = "/update?api_key=";
    url += writeAPIKey;
    url += "&field1=";
    url += value;

    Serial.print("GET ");
    Serial.println(url);

    // Gui HTTP GET
    client.print(
        String("GET ") + url + " HTTP/1.1\r\n" +
        "Host: " + host + "\r\n" +
        "Connection: close\r\n\r\n"
    );

    Serial.println("Dang cho phan hoi...");

    unsigned long timeout = millis();

    while (client.available() == 0) {

        if (millis() - timeout > 10000) {
            Serial.println("LOI: Het thoi gian cho!");
            client.stop();
            return;
        }
    }

    // Doc response
    Serial.println("----- RESPONSE -----");

    while (client.available()) {
        String line = client.readStringUntil('\r');
        Serial.print(line);
    }

    Serial.println();
    Serial.println("--------------------");

    client.stop();

    Serial.println("Da dong ket noi");
}