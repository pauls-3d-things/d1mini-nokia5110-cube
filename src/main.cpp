#include <Arduino.h>
#include <RtcDS1307.h>
#include <SPI.h>
#include <U8g2lib.h>
#include <Wire.h>
#include "config.h"
#include "mini-wifi.h"

#define countof(a) (sizeof(a) / sizeof(a[0]))

RtcDS1307<TwoWire> Rtc(Wire);
U8G2_PCD8544_84X48_F_4W_HW_SPI u8g2(U8G2_R2, /* cs=D8*/ 15, /* dc=D4*/ 2, /* reset=D3*/ 0);  // Nokia 5110 Display

MiniWifi wifi("client", WIFI_SSID, WIFI_PASS);

void setup(void) {
  Serial.begin(9600);
  wifi.setDebugStream(&Serial);
  u8g2.begin();
  Wire.begin(D2, D1);

  if (Rtc.LastError() != 0) {
    Serial.print("RTC communications error = ");
    Serial.println(Rtc.LastError());
  }

  // Rtc.SetDateTime(RtcDateTime(__DATE__, __TIME__));

  if (!Rtc.GetIsRunning()) {
    Rtc.SetIsRunning(true);
  }
}
void printDayOfWeek(const RtcDateTime& dt, U8G2& u8g2) {
  u8g2.setFont(u8g2_font_profont10_tf);
  switch (dt.DayOfWeek()) {
    case 1:
      u8g2.drawStr(4, 44, "Monday");
      break;
    case 2:
      u8g2.drawStr(4, 44, "Tuesday");
      break;
    case 3:
      u8g2.drawStr(4, 44, "Wednesday");
      break;
    case 4:
      u8g2.drawStr(4, 44, "Thursday");
      break;
    case 5:
      u8g2.drawStr(4, 44, "Friday");
      break;
    case 6:
      u8g2.drawStr(4, 44, "Saturday");
      break;
    default:
      u8g2.drawStr(4, 44, "Sunday");
  }
}

void printDate(const RtcDateTime& dt, U8G2& u8g2) {
  char datestring[11];
  snprintf_P(datestring, countof(datestring), PSTR("%04u/%02u/%02u"), dt.Year(), dt.Month(), dt.Day());
  u8g2.setFont(u8g2_font_profont12_tf);
  u8g2.drawStr(12, 11, datestring);
}

void printTime(const RtcDateTime& dt, U8G2& u8g2) {
  char datestring[6];
  if (dt.Second() % 2 == 0) {
    snprintf_P(datestring, countof(datestring), PSTR("%02u:%02u"), dt.Hour(), dt.Minute());
  } else {
    snprintf_P(datestring, countof(datestring), PSTR("%02u %02u"), dt.Hour(), dt.Minute());
  }
  u8g2.setFont(u8g2_font_profont29_tf);
  u8g2.drawStr(3, 34, datestring);
}

void printSignalStrength(uint8_t quality, U8G2& u8g2) {
  u8g2.setFont(u8g2_font_profont10_tf);
  u8g2.drawStr(60, 44, String(quality).c_str());
}

void drawSignal(U8G2& u8g2, uint8_t x, uint8_t y, uint8_t strength) {
  for (uint8_t i = 0; i < strength; i++) {
    u8g2.drawCircle(x, y, i * 3, U8G2_DRAW_UPPER_LEFT);
  }
}

void loop(void) {
  RtcDateTime time = Rtc.GetDateTime();

  u8g2.clearBuffer();

  printDayOfWeek(time, u8g2);
  printDate(time, u8g2);
  printTime(time, u8g2);
  if (wifi.isConnected()) {
    drawSignal(u8g2, 81, 45, wifi.getSignalQuality() / 20);
  }
  u8g2.drawRFrame(0, 0, 84, 48, 3);

  u8g2.sendBuffer();

  wifi.checkWifi();
  delay(1000 / 2);
}