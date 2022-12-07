// Simple date conversions and calculations

#include "RTClib.h"

void showDate(const char* txt, const DateTime& dt) {
    Serial.print(txt);
    Serial.print(' ');
    Serial.print(dt.year(), DEC);
    Serial.print('/');
    Serial.print(dt.month(), DEC);
    Serial.print('/');
    Serial.print(dt.day(), DEC);
    Serial.print(' ');
    Serial.print(dt.hour(), DEC);
    Serial.print(':');
    Serial.print(dt.minute(), DEC);
    Serial.print(':');
    Serial.print(dt.second(), DEC);

    Serial.print(" = ");
    Serial.print(dt.unixtime());
    Serial.print("s / ");
    Serial.print(dt.unixtime() / 86400L);
    Serial.print("d since 1970");

    Serial.println();
}

void showTimeSpan(const char* txt, const TimeSpan& ts) {
    Serial.print(ts.totalseconds(), DEC);
    Serial.print(" total seconds ");
    Serial.println();
}

void setup () {
    Serial.begin(57600);


    DateTime dt6 (2021, 12, 1, 10, 26, 0);
    showDate("dt6", dt6);



    DateTime dt10 = dt6 + TimeSpan(0, 0, 2, 0); // Fourty two minutes and fourty two seconds later.
    showDate("dt10", dt10);


    TimeSpan ts2 = dt10 - dt6;
    Serial.print(ts2.totalseconds(), DEC);
}

void loop () {
}
