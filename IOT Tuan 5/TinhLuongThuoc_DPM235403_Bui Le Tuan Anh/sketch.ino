#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

Adafruit_SSD1306 display(128, 64, &Wire, -1);

// Hàm tam giác trimf chuẩn hóa
float trimf(float x, float a, float b, float c) {
  if (a == b && x <= a) return 1.0; // Vai trái tại 37 °C
  if (b == c && x >= b) return 1.0; // Vai phải tại 41 °C
  if (x <= a || x >= c) return 0.0;
  if (x > a && x <= b) return (x - a) / (b - a);
  if (x > b && x < c) return (c - x) / (c - b);
  return 0.0;
}

float tinhLieuThuocFuzzy(float t) {
  // 1. Mờ hóa biến Sốt theo khoảng mở rộng chuẩn
  float mu_SN  = trimf(t, 37.0, 37.0, 38.0); // Đỉnh 37°C
  float mu_S   = trimf(t, 37.0, 38.0, 39.0); // Đỉnh 38°C
  float mu_SC  = trimf(t, 38.0, 39.0, 41.0); // Đỉnh 39°C, chân phải tới 41°C
  float mu_SRC = trimf(t, 39.0, 41.0, 41.0); // Chân trái từ 39°C, đỉnh 41°C

  // 2. Các giá trị liều thuốc tương ứng (mg)
  float val_T = 200.0, val_BT = 400.0, val_C = 600.0, val_CN = 1000.0;

  // 3. Giải mờ trọng tâm
  float tuSo  = (mu_SN * val_T) + (mu_S * val_BT) + (mu_SC * val_C) + (mu_SRC * val_CN);
  float mauSo = mu_SN + mu_S + mu_SC + mu_SRC;

  if (mauSo == 0) return 200.0;
  return tuSo / mauSo;
}

void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
}

void loop() {
  static float temp = 40.0; // Thử mặc định 40°C

  if (Serial.available() > 0) {
    float val = Serial.parseFloat();
    if (val >= 37.0 && val <= 41.0) temp = val;
  }

  float thuoc_mg = tinhLieuThuocFuzzy(temp);

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("FUZZY CONTROL");
  
  display.setTextSize(2);
  display.setCursor(0, 18);
  display.print(temp, 1); display.println(" C");
  
  display.setCursor(0, 42);
  display.print(thuoc_mg, 1); display.println(" mg");
  display.display();

  delay(1000);
}