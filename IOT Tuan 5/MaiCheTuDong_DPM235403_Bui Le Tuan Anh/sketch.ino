#include <ESP32Servo.h>

#define BUTTON_RAIN_PIN 13 // Chân nút bấm đóng vai trò cảm biến mưa[cite: 1]
#define SERVO_PIN       18 // Chân điều khiển Servo[cite: 1]

Servo maiChe;

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_RAIN_PIN, INPUT_PULLUP);
  maiChe.attach(SERVO_PIN);
  maiChe.write(0); // Mở mái che (0 độ)
}

void loop() {
  // Khi nhấn và giữ nút bấm = giả lập có mưa
  int inputState = digitalRead(BUTTON_RAIN_PIN);

  if (inputState == LOW) {
    maiChe.write(90); // Có mưa: Servo quay 90 độ kéo mái che[cite: 1]
    Serial.println("Gia lap CO MUA -> Keo mai che (90 deg)");
  } else {
    maiChe.write(0);  // Không mưa: Thu mái che về 0 độ[cite: 1]
    Serial.println("Gia lap KHONG MUA -> Thu mai che (0 deg)");
  }
  delay(300);
}