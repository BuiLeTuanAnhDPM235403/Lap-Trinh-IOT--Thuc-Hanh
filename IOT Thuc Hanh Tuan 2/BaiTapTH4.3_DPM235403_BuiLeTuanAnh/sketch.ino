#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// ---------------- Khai báo chân ----------------
const uint8_t PIN_IR      = 8;    // dây vàng của cảm biến hồng ngoại
const uint8_t PIN_LED     = 9;    // LED báo đang có vật cản
const uint8_t PIN_BUZZER  = 10;
const uint8_t PIN_RESET   = 4;    // nút nhấn reset bộ đếm

const unsigned long DEBOUNCE_MS = 60UL;   // chống dội tín hiệu cảm biến
const unsigned long BEEP_MS     = 60UL;   // độ dài tiếng tít

LiquidCrystal_I2C lcd(0x27, 16, 2);

int  soVatDaDem   = 0;
bool coVatTruocDo = false;          // trạng thái đã lọc dội
int  trangThaiCu  = HIGH;           // HIGH = không có vật (mặc định pull-up)
unsigned long tDebounce = 0;
unsigned long tBeepStart = 0;
bool dangKeu = false;
bool lastReset = HIGH;

// ============================================================
void setup() {
  pinMode(PIN_IR, INPUT);
  digitalWrite(PIN_IR, HIGH);   // kích hoạt điện trở kéo lên nội bộ (như sách)
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_BUZZER, OUTPUT);
  pinMode(PIN_RESET, INPUT_PULLUP);

  Serial.begin(9600);
  Serial.println(F("=== BO DEM VAT THE - CAM BIEN HONG NGOAI ==="));

  lcd.init();
  lcd.backlight();
  hienThi();
}

// ============================================================
void loop() {
  unsigned long now = millis();

  // ---- Đọc nút reset ----
  bool rst = digitalRead(PIN_RESET);
  if (rst == LOW && lastReset == HIGH) {
    soVatDaDem = 0;
    Serial.println(F("[RESET] Da dua bo dem ve 0"));
    hienThi();
  }
  lastReset = rst;

  // ---- Đọc cảm biến IR có lọc dội (debounce) ----
  int trangThai = digitalRead(PIN_IR);   // LOW = phat hien vat can
  if (trangThai != trangThaiCu) {
    tDebounce = now;
    trangThaiCu = trangThai;
  }
  if (now - tDebounce > DEBOUNCE_MS) {
    bool coVatMoi = (trangThai == LOW);

    if (coVatMoi && !coVatTruocDo) {
      // Sườn xuống HIGH -> LOW: vừa có 1 vật mới đi vào vùng cảm biến
      soVatDaDem++;
      Serial.print(F("[PHAT HIEN] Vat thu "));
      Serial.println(soVatDaDem);
      hienThi();
      taoTiengBip(now);
    }
    coVatTruocDo = coVatMoi;
  }

  digitalWrite(PIN_LED, coVatTruocDo ? HIGH : LOW);

  // ---- Tắt còi sau BEEP_MS ----
  if (dangKeu && now - tBeepStart >= BEEP_MS) {
    digitalWrite(PIN_BUZZER, LOW);
    dangKeu = false;
  }
}

// ------------------------------------------------------------
void taoTiengBip(unsigned long now) {
  digitalWrite(PIN_BUZZER, HIGH);
  tBeepStart = now;
  dangKeu = true;
}

// ------------------------------------------------------------
void hienThi() {
  lcd.setCursor(0, 0);
  lcd.print(F("CAM BIEN HONG NG"));
  lcd.setCursor(0, 1);
  lcd.print(F("So vat: "));
  lcd.print(soVatDaDem);
  lcd.print(F("     "));
}
