#pragma once
#include <Arduino.h>

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

constexpr int DEBOUNCE_VAL = 50;
constexpr uint32_t REPORT_PERIOD = 1000;
constexpr int POTAR_RESOLUTION = 5;

constexpr size_t REPORT_MSG_SIZE = 20;

// 16x2 characters + 3 leds + buzzer
constexpr size_t LCD_MSG_SIZE = 16*2 + 3 + 1;
constexpr size_t RCV_BUF_SIZE = LCD_MSG_SIZE + 1;   // lcd msg + \0


