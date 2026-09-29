/* ============================================================
   CHƯƠNG 4 - ARDUINO VÀ CẢM BIẾN
   Mục 4.5D: CẢM BIẾN QUANG (LDR - Light Dependent Resistor)

   Đề tài: ĐÈN ĐƯỜNG TỰ ĐỘNG THEO ÁNH SÁNG MÔI TRƯỜNG

   Sách trình bày 2 mạch analog:
     4.5D1  LDR + Op-Amp LM358 + chiết áp 10K  -> so sánh ngưỡng
     4.5D2  LDR + 2 transistor BC547           -> khuếch đại đóng ngắt
   Bài này thay bộ so sánh phần cứng bằng Arduino:
     - LDR (cầu phân áp 10K) -> A0 : đọc mức sáng dạng analog (0..1023)
     - Chiết áp 10K          -> A1 : chỉnh ngưỡng sáng/tối (thay POT trong sách)
     - Ngõ DO của module     -> D2 : ngõ ra bộ so sánh LM358 có sẵn trên module
     - LED trắng (đèn đường) -> D9 (PWM): trời càng tối đèn càng sáng
     - LED vàng              -> D8 : soi trạng thái ngõ ra DO để đối chiếu
     - LCD 16x2 I2C          -> A4 (SDA), A5 (SCL)

   Cải tiến so với mạch trong sách:
     1. Ngưỡng chỉnh được bằng phần mềm + có TRỄ (hysteresis) nên đèn
        không chớp liên tục khi ánh sáng dao động quanh ngưỡng.
     2. Lọc nhiễu bằng trung bình trượt 8 mẫu.
     3. Điều khiển PWM: sáng dần theo mức tối thay vì chỉ ON/OFF.
   ============================================================ */

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// ---------------- Khai báo chân ----------------
const uint8_t PIN_LDR    = A0;   // ngõ ra analog của cảm biến quang
const uint8_t PIN_POT    = A1;   // chiết áp chỉnh ngưỡng
const uint8_t PIN_DO     = 2;    // ngõ ra số của module (bộ so sánh)
const uint8_t PIN_LAMP   = 9;    // LED trắng - phải là chân PWM
const uint8_t PIN_LED_DO = 8;    // LED báo trạng thái ngõ DO

// ---------------- Tham số ----------------
const bool  AO_TANG_THEO_ANH_SANG = true; // đổi thành false nếu module cho
                                          // giá trị giảm khi trời sáng
const int   HYSTERESIS = 40;    // vùng trễ (đơn vị ADC) tránh chớp đèn
const uint8_t SO_MAU   = 8;     // số mẫu lọc trung bình
const unsigned long CHU_KY_LCD = 250UL;

LiquidCrystal_I2C lcd(0x27, 16, 2);

int  boDem[SO_MAU];
uint8_t viTri = 0;
long tong = 0;

bool denDangSang = false;
unsigned long tLcd = 0;

// ============================================================
void setup() {
  pinMode(PIN_DO, INPUT);
  pinMode(PIN_LAMP, OUTPUT);
  pinMode(PIN_LED_DO, OUTPUT);

  Serial.begin(9600);
  Serial.println(F("=== DEN DUONG TU DONG - CAM BIEN QUANG LDR ==="));
  Serial.println(F("AnhSang\tNguong\tPWM\tTrangThai"));

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print(F(" CAM BIEN QUANG "));
  lcd.setCursor(0, 1);
  lcd.print(F("   LDR  4.5D    "));
  delay(1500);
  lcd.clear();

  // Nạp đầy bộ lọc bằng giá trị đọc được lúc khởi động
  int v = docAnhSang();
  for (uint8_t i = 0; i < SO_MAU; i++) { boDem[i] = v; tong += v; }
}

// ============================================================
void loop() {
  // --- 1. Đọc và lọc mức sáng ---
  int mau = docAnhSang();
  tong -= boDem[viTri];
  boDem[viTri] = mau;
  tong += mau;
  viTri = (viTri + 1) % SO_MAU;
  int anhSang = tong / SO_MAU;          // 0 = tối hẳn, 1023 = sáng nhất

  // --- 2. Đọc ngưỡng từ chiết áp 10K (vai trò của POT trong sơ đồ sách) ---
  int nguong = analogRead(PIN_POT);

  // --- 3. So sánh có vùng trễ ---
  if (!denDangSang && anhSang < nguong - HYSTERESIS) denDangSang = true;
  if ( denDangSang && anhSang > nguong + HYSTERESIS) denDangSang = false;

  // --- 4. Điều khiển đèn: càng tối càng sáng ---
  int pwm = 0;
  if (denDangSang) {
    pwm = map(anhSang, nguong, 0, 60, 255);   // tối hơn ngưỡng -> sáng dần
    pwm = constrain(pwm, 60, 255);
  }
  analogWrite(PIN_LAMP, pwm);

  // --- 5. Soi ngõ ra số DO của module (bộ so sánh phần cứng) ---
  // Module LDR thường cho DO = LOW khi cường độ sáng vượt ngưỡng chỉnh bằng biến trở
  bool doState = digitalRead(PIN_DO);
  digitalWrite(PIN_LED_DO, doState);

  // --- 6. Hiển thị ---
  hienThi(anhSang, nguong, pwm);
}

// ------------------------------------------------------------
int docAnhSang() {
  int raw = analogRead(PIN_LDR);
  return AO_TANG_THEO_ANH_SANG ? raw : (1023 - raw);
}

// ------------------------------------------------------------
void hienThi(int anhSang, int nguong, int pwm) {
  unsigned long now = millis();
  if (now - tLcd < CHU_KY_LCD) return;
  tLcd = now;

  int phanTramSang = map(anhSang, 0, 1023, 0, 100);
  int phanTramNg   = map(nguong,  0, 1023, 0, 100);

  lcd.setCursor(0, 0);
  lcd.print(F("Sang:"));
  in3KyTu(phanTramSang);
  lcd.print(F("% Ng:"));
  in3KyTu(phanTramNg);
  lcd.print(F("%"));

  lcd.setCursor(0, 1);
  if (denDangSang) {
    lcd.print(F("TOI - DEN ON "));
    in3KyTu(map(pwm, 0, 255, 0, 100));
    lcd.print(F("%"));
  } else {
    lcd.print(F("SANG- DEN OFF   "));
  }

  Serial.print(anhSang);  Serial.print('\t');
  Serial.print(nguong);   Serial.print('\t');
  Serial.print(pwm);      Serial.print('\t');
  Serial.println(denDangSang ? F("TOI -> BAT DEN") : F("SANG -> TAT DEN"));
}

// In số 0..100 luôn chiếm 3 ký tự để LCD không bị chữ thừa
void in3KyTu(int v) {
  if (v < 100) lcd.print(' ');
  if (v < 10)  lcd.print(' ');
  lcd.print(v);
}
