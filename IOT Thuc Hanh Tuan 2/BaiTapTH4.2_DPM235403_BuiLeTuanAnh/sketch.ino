/*
  BÀI 4.2: MẠCH PHÁT HIỆN VẬT CẢN DÙNG CẢM BIẾN TIỆM CẬN SIÊU ÂM HC-SR04
  ------------------------------------------------------------------------
  Nguyên lý: HC-SR04 phát sóng siêu âm, đo thời gian sóng phản xạ về để
  tính khoảng cách đến vật cản gần nhất.

  Kết nối:
  - HC-SR04: VCC->5V | GND->GND | TRIG->D9 | ECHO->D10
  - LCD 16x2: RS->D12 | E->D11 | D4->D5 | D5->D4 | D6->D3 | D7->D2
              VSS->GND | VDD->5V | RW->GND | VEE->biến trở 10K
  - LED báo hiệu: Anode (qua trở 220ohm) -> D8 | Cathode -> GND

  Nguyên lý hoạt động:
  - Khoảng cách đo được hiển thị liên tục lên LCD (hàng 2).
  - Nếu khoảng cách <= NGUONG_CANH_BAO (20cm): LED sáng, báo "Vat can!"
  - Nếu xa hơn: LED tắt, hiển thị bình thường.
*/

#include <LiquidCrystal.h>

// Khai báo chân LCD: RS, E, DB4, DB5, DB6, DB7
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// Chân kết nối HC-SR04
const int trigPin = 9;
const int echoPin = 10;

// Chân LED báo hiệu vật cản
const int ledPin = 8;

// Ngưỡng khoảng cách để cảnh báo (đơn vị: cm)
const float NGUONG_CANH_BAO = 20.0;

void setup() {
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("Khoang cach:");

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(ledPin, OUTPUT);

  Serial.begin(9600);
}

void loop() {
  float khoangCach = doKhoangCach();

  // Hiển thị khoảng cách lên LCD, hàng thứ 2
  lcd.setCursor(0, 1);
  if (khoangCach <= NGUONG_CANH_BAO) {
    lcd.print("Vat can! ");
    lcd.print(khoangCach, 0);
    lcd.print("cm ");
    digitalWrite(ledPin, HIGH);   // Bật LED cảnh báo
  } else {
    lcd.print(khoangCach, 0);
    lcd.print(" cm         ");    // khoảng trắng để xóa ký tự cũ
    digitalWrite(ledPin, LOW);    // Tắt LED
  }

  // In ra Serial Monitor để theo dõi/debug
  Serial.print("Khoang cach: ");
  Serial.print(khoangCach);
  Serial.println(" cm");

  delay(500);  // cập nhật mỗi 0.5 giây
}

// Hàm đo khoảng cách bằng cảm biến siêu âm HC-SR04
float doKhoangCach() {
  // Phát xung TRIG trong 10 micro giây để bắt đầu đo
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Đo thời gian xung HIGH trên chân ECHO (đơn vị: micro giây)
  long thoiGian = pulseIn(echoPin, HIGH);

  // Công thức đổi thời gian ra khoảng cách (cm): thời gian / 58
  float khoangCach = thoiGian / 58.0;

  return khoangCach;
}
