#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// ---------------- Khai báo chân ----------------
const uint8_t PIN_PIR       = 7;
const uint8_t PIN_LED_ALARM = 11;   // LED đỏ
const uint8_t PIN_LED_READY = 12;   // LED xanh
const uint8_t PIN_BUZZER    = 8;
const uint8_t PIN_BUTTON    = 4;

// ---------------- Hằng số thời gian ----------------
const unsigned long WARMUP_MS   = 5000UL;   // thực tế nên để 30000 (30 giây)
const unsigned long ALARM_MS    = 5000UL;   // giữ báo động 5 giây
const unsigned long BEEP_MS     = 250UL;    // chu kỳ bật/tắt còi
const unsigned long BLINK_MS    = 200UL;    // chu kỳ nháy LED đỏ
const unsigned long DEBOUNCE_MS = 50UL;     // chống dội phím

// ---------------- Đối tượng LCD ----------------
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ---------------- Biến trạng thái ----------------
enum SystemState { WARMUP, READY, ALARM, DISARMED };
SystemState state = WARMUP;

unsigned long tState    = 0;   // mốc thời gian vào trạng thái hiện tại
unsigned long tBeep     = 0;
unsigned long tBlink    = 0;
unsigned long tButton   = 0;
unsigned long tLcd      = 0;

bool beepOn    = false;
bool blinkOn   = false;
bool lastBtn   = HIGH;
int  lastPir   = LOW;
unsigned int  detectCount = 0;

// ============================================================
void setup() {
  pinMode(PIN_PIR, INPUT);
  pinMode(PIN_LED_ALARM, OUTPUT);
  pinMode(PIN_LED_READY, OUTPUT);
  pinMode(PIN_BUZZER, OUTPUT);
  pinMode(PIN_BUTTON, INPUT_PULLUP);   // nút nối xuống GND

  Serial.begin(9600);
  Serial.println(F("=== HE THONG BAO DONG PIR ==="));

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print(F("  CAM BIEN PIR  "));
  lcd.setCursor(0, 1);
  lcd.print(F("Khoi dong...    "));

  tState = millis();
}

// ============================================================
void loop() {
  unsigned long now = millis();

  docNutNhan(now);
  xuLyTrangThai(now);
  capNhatNgoRa(now);
  capNhatLCD(now);
}

// ------------------------------------------------------------
// Đọc nút nhấn: chuyển qua lại giữa canh gác và tắt báo động
void docNutNhan(unsigned long now) {
  bool btn = digitalRead(PIN_BUTTON);
  if (btn != lastBtn && now - tButton > DEBOUNCE_MS) {
    tButton = now;
    if (btn == LOW) {                 // vừa nhấn xuống
      if (state == DISARMED) {
        doiTrangThai(WARMUP, now);
        Serial.println(F("[NUT] Bat lai che do canh gac"));
      } else {
        doiTrangThai(DISARMED, now);
        Serial.println(F("[NUT] Tat che do canh gac"));
      }
    }
    lastBtn = btn;
  }
}

// ------------------------------------------------------------
// Máy trạng thái chính
void xuLyTrangThai(unsigned long now) {
  int pir = digitalRead(PIN_PIR);

  switch (state) {

    case WARMUP:
      if (now - tState >= WARMUP_MS) {
        doiTrangThai(READY, now);
        Serial.println(F("[HE THONG] San sang canh gac"));
      }
      break;

    case READY:
      // Bắt sườn lên: chỉ tính 1 lần khi PIR vừa chuyển LOW -> HIGH
      if (pir == HIGH && lastPir == LOW) {
        detectCount++;
        doiTrangThai(ALARM, now);
        Serial.print(F("[CANH BAO] Phat hien chuyen dong! Lan thu "));
        Serial.println(detectCount);
      }
      break;

    case ALARM:
      // Còn chuyển động thì gia hạn thời gian báo động
      if (pir == HIGH) {
        tState = now;
      } else if (now - tState >= ALARM_MS) {
        doiTrangThai(READY, now);
        Serial.println(F("[HE THONG] Khu vuc an toan"));
      }
      break;

    case DISARMED:
      break;
  }

  lastPir = pir;
}

// ------------------------------------------------------------
void doiTrangThai(SystemState s, unsigned long now) {
  state  = s;
  tState = now;
  tLcd   = 0;          // ép cập nhật LCD ngay
}

// ------------------------------------------------------------
// Điều khiển LED và còi
void capNhatNgoRa(unsigned long now) {
  if (state == ALARM) {
    digitalWrite(PIN_LED_READY, LOW);

    if (now - tBlink >= BLINK_MS) {         // nháy LED đỏ
      tBlink = now;
      blinkOn = !blinkOn;
      digitalWrite(PIN_LED_ALARM, blinkOn);
    }

    if (now - tBeep >= BEEP_MS) {           // còi kêu ngắt quãng
      tBeep = now;
      beepOn = !beepOn;
      if (beepOn) tone(PIN_BUZZER, 2000);
      else        noTone(PIN_BUZZER);
    }
  }
  else {
    noTone(PIN_BUZZER);
    beepOn = false;
    digitalWrite(PIN_LED_ALARM, LOW);
    // LED xanh sáng khi đang canh gác, nháy chậm khi warm-up, tắt khi DISARMED
    if (state == READY) {
      digitalWrite(PIN_LED_READY, HIGH);
    } else if (state == WARMUP) {
      digitalWrite(PIN_LED_READY, (now / 500) % 2);
    } else {
      digitalWrite(PIN_LED_READY, LOW);
    }
  }
}

// ------------------------------------------------------------
// Hiển thị LCD (cập nhật 250 ms/lần để tránh nhấp nháy)
void capNhatLCD(unsigned long now) {
  if (now - tLcd < 250 && tLcd != 0) return;
  tLcd = now;

  lcd.setCursor(0, 0);
  switch (state) {
    case WARMUP: {
      unsigned long conLai = (WARMUP_MS - (now - tState)) / 1000 + 1;
      lcd.print(F("Khoi dong: "));
      lcd.print(conLai);
      lcd.print(F("s  "));
      break;
    }
    case READY:    lcd.print(F("AN TOAN         ")); break;
    case ALARM:    lcd.print(F("!! BAO DONG !!  ")); break;
    case DISARMED: lcd.print(F("DA TAT CANH GAC ")); break;
  }

  lcd.setCursor(0, 1);
  lcd.print(F("So lan: "));
  lcd.print(detectCount);
  lcd.print(F("        "));
}
