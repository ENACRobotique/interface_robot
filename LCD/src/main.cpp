#include <Arduino.h>
#include <LiquidCrystal.h>

//////////////// pinout /////////////
constexpr pin_size_t LED_R = PIN_PA5;
constexpr pin_size_t LED_G = PIN_PA3;
constexpr pin_size_t LED_B = PIN_PB0;

constexpr pin_size_t BUZZ = PIN_PA6;

constexpr pin_size_t LCD_EN = PIN_PA1;
constexpr pin_size_t LCD_RS = PIN_PA2;
constexpr pin_size_t LCD_D4 = PIN_PC0;
constexpr pin_size_t LCD_D5 = PIN_PC1;
constexpr pin_size_t LCD_D6 = PIN_PC2;
constexpr pin_size_t LCD_D7 = PIN_PC3;

constexpr pin_size_t POTAR = PIN_PA4;
constexpr pin_size_t BTN_OK = PIN_PA7;
constexpr pin_size_t BTN_RET = PIN_PB5;
constexpr pin_size_t BTN_COLOR = PIN_PB4;
constexpr pin_size_t TIRETTE = PIN_PB1;

/////////////////////////////////////

constexpr int DEBOUNCE_VAL = 2000;
constexpr uint32_t LCD_UPDATE_PERIOD = 100;
constexpr uint32_t REPORT_PERIOD = 200;

LiquidCrystal lcd(LCD_RS, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7);


typedef struct {
  pin_size_t pin;
  int debounce;
  int state;
} btn_t;

btn_t ok_btn = {BTN_OK, -1, 0};
btn_t ret_btn = {BTN_RET, -1, 0};
btn_t col_btn = {BTN_COLOR, -1, 0};
btn_t tirette = {TIRETTE, -1, 0};

void btn_cb(btn_t* btn) {
  if(digitalRead(btn->pin) == btn->state) {
    btn->debounce = DEBOUNCE_VAL;
  }
}

void isr() {
  btn_cb(&ok_btn);
  btn_cb(&ret_btn);
  btn_cb(&col_btn);
  btn_cb(&tirette);
}

void setup() {
  Serial.begin(115200);
  lcd.begin(16, 2);
  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(LED_B, OUTPUT);

  pinMode(BTN_OK, INPUT_PULLUP);
  pinMode(BTN_RET, INPUT_PULLUP);
  pinMode(BTN_COLOR, INPUT_PULLUP);
  pinMode(TIRETTE, INPUT_PULLUP);

  attachInterrupt(BTN_OK, isr, CHANGE);
  attachInterrupt(BTN_RET, isr, CHANGE);
  attachInterrupt(BTN_COLOR, isr, CHANGE);
  attachInterrupt(TIRETTE, isr, CHANGE);

  lcd.clear();

}

void debounce_update(btn_t* btn) {
  if(btn->debounce != -1) {
    btn->debounce -= 1;
    if(btn->debounce == 0 && digitalRead(btn->pin) == btn->state) {
      btn->state ^= 1;
    }
  }
}

uint32_t time_lcd_update = 0;
uint32_t report_time = 0;

void loop() {
  debounce_update(&ok_btn);
  debounce_update(&ret_btn);
  debounce_update(&col_btn);
  debounce_update(&tirette);
  int potar_val = 1023 - analogRead(POTAR);

  if(millis() - report_time > REPORT_PERIOD) {
    report_time = millis();
    char buf[50];
    // ok, ret, color, tirette, potar
    snprintf(buf, 50, "%d %d %d %d %d", ok_btn.state, ret_btn.state, col_btn.state, tirette.state, potar_val);
    Serial.println(buf);
  }

  if(millis() - time_lcd_update > LCD_UPDATE_PERIOD) {
    time_lcd_update = millis();
    lcd.clear();
    lcd.print(potar_val);
    lcd.setCursor(10, 0);
    static int color_state = col_btn.state;
    static int color = 0;
    if(col_btn.state && color_state != col_btn.state)
    {
      color = (color + 1) % 2;
    }
    color_state = col_btn.state;

    static constexpr char* COLORS[] = {
      "BLEU",
      "JAUNE"
    };
    lcd.print(COLORS[color]);

    lcd.setCursor(8, 1);
    lcd.print(ret_btn.state);
    lcd.setCursor(11, 1);
    lcd.print(ok_btn.state);
    lcd.setCursor(14, 1);
    lcd.print(tirette.state);
  }

  // if(tirette.state) {
  //   tone(BUZZ, 440, 1000);
  // }

  


  digitalWrite(LED_G, ok_btn.state);
  digitalWrite(LED_R, ret_btn.state);
  digitalWrite(LED_B, tirette.state);
}
