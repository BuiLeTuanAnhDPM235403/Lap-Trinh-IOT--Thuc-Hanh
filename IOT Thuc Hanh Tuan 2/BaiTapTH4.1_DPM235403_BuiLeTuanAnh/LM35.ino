#include <LiquidCrystal.h>

// Khai báo chân LCD tương ứng với sơ đồ (RS, E, D4, D5, D6, D7)
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

#define ADC_VREF_mV     5000.0 // in millivolt
#define ADC_RESOLUTION 1024.0
#define PIN_LM35        A0

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
  lcd.print("Temp Monitor");
  delay(1000);
  lcd.clear();
}

void loop() {
  // Đọc giá trị ADC từ cảm biến LM35
  int adcVal = analogRead(PIN_LM35);

  // Chuyển giá trị ADC sang điện áp (mV)
  float milliVolt = adcVal * (ADC_VREF_mV / ADC_RESOLUTION);

  // Chuyển điện áp sang nhiệt độ độ C và F
  float tempC = milliVolt / 10.0;
  float tempF = tempC * 9.0 / 5.0 + 32.0;

  // In ra Serial Monitor
  Serial.print("Temperature: ");
  Serial.print(tempC);
  Serial.print("°C  ~  ");
  Serial.print(tempF);
  Serial.println("°F");

  // In ra màn hình LCD
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(tempC, 1);
  lcd.print((char)223); // Ký tự độ °
  lcd.print("C   ");

  lcd.setCursor(0, 1);
  lcd.print("Temp: ");
  lcd.print(tempF, 1);
  lcd.print((char)223);
  lcd.print("F   ");

  delay(1000);
}