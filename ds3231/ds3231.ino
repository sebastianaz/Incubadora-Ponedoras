// Date and time functions using a DS3231 RTC connected via I2C and Wire lib
#include "RTClib.h"

RTC_DS3231 rtc;
DateTime tiempoPasado;

char daysOfTheWeek[7][12] = {"DOM", "LUN", "MAR", "MIE", "JUE", "VIE", "SAB"};

void setup () {
  Serial.begin(57600);

#ifndef ESP8266
  while (!Serial); // wait for serial port to connect. Needed for native USB
#endif

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
  }
  digitalClockDisplay();

}

void loop () {
    DateTime now = rtc.now();
    showTimeActual(now);
    TimeSpan  dt10 = tiempoPasado-now;
    showTimeSpan("dt10", dt10);

    Serial.print("Temperature: ");
    Serial.print(rtc.getTemperature());
    Serial.println(" C");
    Serial.println();
    delay(3000);

}

void digitalClockDisplay() {
 tiempoPasado = rtc.now()+TimeSpan(21,0,0,0);
}


void showTimeSpan(const char* txt, const TimeSpan& ts) {
  Serial.print("Tiempo Faltante: ");
    Serial.print(ts.days(), DEC);
    Serial.print("d: ");
    Serial.print(ts.hours(), DEC);
    Serial.print("h: ");
    Serial.print(ts.minutes(), DEC);
    Serial.print("m: ");
    Serial.print(ts.seconds(), DEC);
    Serial.print("s");
    Serial.println();
}

void showTimeActual(DateTime actual){
    Serial.print(actual.year(), DEC);
    Serial.print('/');
    Serial.print(actual.month(), DEC);
    Serial.print('/');
    Serial.print(actual.day(), DEC);
    Serial.print(" (");
    Serial.print(daysOfTheWeek[actual.dayOfTheWeek()]);
    Serial.print(") ");
    Serial.print(actual.hour(), DEC);
    Serial.print(':');
    Serial.print(actual.minute(), DEC);
    Serial.print(':');
    Serial.print(actual.second(), DEC);
    Serial.println();
 }
