#include <DHT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// =========================
// DHT22
// =========================
#define DHTPIN 2
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

// =========================
// LCD I2C
// =========================
LiquidCrystal_I2C lcd(0x27, 16, 2);

// =========================
// SETUP
// =========================
void setup() {
  Serial.begin(9600);

  dht.begin();

  lcd.init();
  lcd.backlight();

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("HE THONG TX/RX");

  lcd.setCursor(0, 1);
  lcd.print("Dang khoi dong");

  delay(2000);

  lcd.clear();

  Serial.println("================================");
  Serial.println("  HE THONG DO NHIET DO - DO AM");
  Serial.println("================================");
  Serial.println("TX: DHT22 -> Arduino -> nRF24L01");
  Serial.println("RX: nRF24L01 -> Arduino -> LCD");
  Serial.println();
}

// =========================
// LOOP
// =========================
void loop() {

  // -------------------------
  // TX: Doc DHT22
  // -------------------------
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  // Kiem tra loi cam bien
  if (isnan(temperature) || isnan(humidity)) {

    Serial.println("TX: LOI DHT22!");

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("LOI DHT22!");

    lcd.setCursor(0, 1);
    lcd.print("Kiem tra cam bien");

    delay(2000);
    return;
  }

  // -------------------------
  // Tao packet TX
  // -------------------------
  String packet =
    "T:" + String(temperature, 1) +
    ",H:" + String(humidity, 1);

  // -------------------------
  // Hien thi TX Serial
  // -------------------------
  Serial.println("--------------------------------");

  Serial.print("TX - Nhiet do: ");
  Serial.print(temperature, 1);
  Serial.println(" C");

  Serial.print("TX - Do am: ");
  Serial.print(humidity, 1);
  Serial.println(" %");

  Serial.print("TX -> nRF24L01: ");
  Serial.println(packet);

  // -------------------------
  // MO PHONG TRUYEN
  // nRF24L01 TX
  //      |
  //      v
  // nRF24L01 RX
  // -------------------------
  delay(300);

  Serial.print("RX <- nRF24L01: ");
  Serial.println(packet);

  // -------------------------
  // RX: Hien thi LCD
  // -------------------------
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Nhiet do:");

  lcd.setCursor(10, 0);
  lcd.print(temperature, 1);

  lcd.write(223);
  lcd.print("C");

  lcd.setCursor(0, 1);
  lcd.print("Do am:");

  lcd.setCursor(10, 1);
  lcd.print(humidity, 1);

  lcd.print("%");

  Serial.println("RX -> LCD: Da hien thi");
  Serial.println();

  delay(2000);
}