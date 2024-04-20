#include <Arduino.h>
#include <LiquidCrystal.h>

#include "params.h"

constexpr int NB_PITCH = 21;
int PITCH[NB_PITCH] = {
   440,  494,  523,  587,  659,  698,  784,
   880,  988, 1046, 1175, 1318, 1397, 1568,
  1760, 1975, 2093, 2349, 2637, 2794, 3135
};


LiquidCrystal lcd(LCD_RS, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7);


typedef struct {
  pin_size_t pin;
  int debounce;     //  counter for button debouce
  int state;
  /**
   * Valeurs possibles pour event:
   *    'N': pas d'évènements
   *    'P': pressed
   *    'R': released
  */
  char event;
} btn_t;


btn_t ok_btn = {BTN_OK, -1, 0, 'N'};
btn_t ret_btn = {BTN_RET, -1, 0, 'N'};
btn_t col_btn = {BTN_COLOR, -1, 0, 'N'};
btn_t tirette = {TIRETTE, -1, 0, 'N'};
int last_potar = 0;   // last potar value sent

// last report time
uint32_t report_time = 0;
char report_msg[REPORT_MSG_SIZE];

// receive buffer
char rcv_buf[RCV_BUF_SIZE];
size_t rcv_offset;


void snprintf_events(char *str, size_t size, int potar_val);


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

  // initialize report message
  snprintf_events(report_msg, REPORT_MSG_SIZE, 0);

  // initialise buffer de réception
  memset(rcv_buf, '-', RCV_BUF_SIZE);

  lcd.clear();
}

void debounce_update(btn_t* btn) {
  if(btn->debounce != -1) {
    btn->debounce -= 1;
    if(btn->debounce == 0 && digitalRead(btn->pin) == btn->state) {
      btn->state ^= 1;
      if(btn->state) {
        btn->event = 'P';
      } else {
        btn->event = 'R';
      }
    }
  }
}

void snprintf_events(char *str, size_t size, int potar_val) {
  snprintf(str, size, "%c %c %c %c %d",
    ok_btn.event, ret_btn.event,
    col_btn.event, tirette.event,
    potar_val);
  
  // reset events
  ok_btn.event = 'N';
  ret_btn.event = 'N';
  col_btn.event = 'N';
  tirette.event = 'N';

  last_potar = potar_val;
  
  // send message
  Serial.println(report_msg);
  report_time = millis();
}



void loop() {
  debounce_update(&ok_btn);
  debounce_update(&ret_btn);
  debounce_update(&col_btn);
  debounce_update(&tirette);

  int potar_val = 1023 - analogRead(POTAR);

  // si un événement a eu lieu, envoyer le message
  if(ok_btn.event ||
    ret_btn.event ||
    col_btn.event ||
    tirette.event ||
    abs(potar_val - last_potar) > POTAR_RESOLUTION
    ) {
      snprintf_events(report_msg, REPORT_MSG_SIZE, potar_val);
  }


  // envoyer périodiquement le message
  if(millis() - report_time > REPORT_PERIOD) {
    snprintf_events(report_msg, REPORT_MSG_SIZE, potar_val);
  }

  
  // receive bytes
  int bytes_available = Serial.available();
  // check for buffer overflow
  if(rcv_offset + bytes_available <= RCV_BUF_SIZE) {
    // add bytes read to the buffer
    Serial.readBytes(rcv_buf+rcv_offset, bytes_available);
    rcv_offset += bytes_available;

    // try getting the lenght of the string (\0 terminated)
    size_t len = strnlen(rcv_buf, RCV_BUF_SIZE);
      // if(len != RCV_BUF_SIZE) {
    if(len == LCD_MSG_SIZE) {
      // display first line
      lcd.setCursor(0, 0);
      lcd.write(rcv_buf, 16);
      // display 2nd line
      lcd.setCursor(0, 1);
      lcd.write(rcv_buf+16, 16);

      digitalWrite(LED_R, rcv_buf[32] =! '0');
      digitalWrite(LED_R, rcv_buf[33] =! '0');
      digitalWrite(LED_R, rcv_buf[34] =! '0');

      if(rcv_buf[35] < 'A') {
        noTone(BUZZ);
      } else if(rcv_buf[35] <= 'A' + NB_PITCH) {
        int pitch_idx = rcv_buf[35] - 'A';
        tone(BUZZ, PITCH[pitch_idx]);
      }
      

      // TODO check that the line return is working
      lcd.print(rcv_buf);
      memset(rcv_buf, '-', RCV_BUF_SIZE);
    }
    
    
  }

}

