/*
  =============================================================
   BÁO ĐỘNG KHÓI / RÒ RỈ GAS DÙNG CẢM BIẾN MQ-2 (CÓ ÂM THANH)
  =============================================================
*/

#include <LiquidCrystal.h>

// ----------------- Khai báo chân -----------------
const int MQ2_PIN     = A0;    // Cảm biến MQ2
const int BUZZER_PIN  = 10;    // Còi báo động (Buzzer)
const int RELAY_PIN   = 13;    // Module Relay

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// ----------------- Thông số cảnh báo -----------------
const float CAL_FACTOR   = 10.23; 
const float TRIP_PERCENT = 65.0;  
const float HYSTERESIS   = 5.0;  

int   d = 0;      
float p = 0.0;    

void setup() {
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT);

  noTone(BUZZER_PIN); // Tắt âm thanh ban đầu
  digitalWrite(RELAY_PIN, LOW);

  Serial.begin(9600);
  lcd.begin(16, 2);

  lcd.setCursor(0, 0);
  lcd.print(" HETHONG BAO DONG");
  lcd.setCursor(0, 1);
  lcd.print("   KHOI / GAS   ");
  delay(2000);
  lcd.clear();
}

void loop() {
  d = analogRead(MQ2_PIN);
  p = (float)d / CAL_FACTOR;

  Serial.print("ADC = ");
  Serial.print(d);
  Serial.print("\t Nong do = ");
  Serial.print(p, 1);
  Serial.println(" %");

  lcd.setCursor(0, 0);
  lcd.print("Gas: ");
  lcd.print(p, 1);
  lcd.print("%    "); 

  if (p >= TRIP_PERCENT) {
    // 1. Kích hoạt Relay
    digitalWrite(RELAY_PIN, HIGH);
    
    // 2. Phát âm thanh báo động (Tần số 1000Hz - Tiếng còi hú/tít)
    tone(BUZZER_PIN, 1000); 

    lcd.setCursor(0, 1);
    lcd.print("CANH BAO CO GAS!");
  } 
  else if (p < (TRIP_PERCENT - HYSTERESIS)) {
    // 1. Tắt Relay
    digitalWrite(RELAY_PIN, LOW);
    
    // 2. Tắt âm thanh còi
    noTone(BUZZER_PIN); 

    lcd.setCursor(0, 1);
    lcd.print("AN TOAN         ");
  }

  delay(300); 
}