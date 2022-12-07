/* Timestamp functions using a DS1307 RTC connected via I2C and Wire lib
**
** Useful for file name
**		` SD.open(time.timestamp()+".log", FILE_WRITE) `
**
**
** Created: 2015-06-01 by AxelTB
** Last Edit:
*/

#include "RTClib.h"

bool evento_inicio = true;  // variable de control para inicio de evento con valor true
bool evento_fin = true;   // variable de control para finalizacion de evento con valor true

RTC_DS1307 rtc;

void setup () {
  Serial.begin(115200);

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1) delay(10);
  }
  // Obtenemos la fecha actual
  fecha = now();
}

void loop() {

  Serial.print("  ");
  Serial.print(day(fecha));
  Serial.print("/");
  Serial.print(month(fecha));
  Serial.print("/");
  Serial.println(year(fecha))
    DateTime time = rtc.now();

 //Full Timestamp
 Serial.println(time.timestamp(DateTime::TIMESTAMP_FULL));

 //Date Only
 Serial.println(time.timestamp(DateTime::TIMESTAMP_DATE));

 //Full Timestamp
 Serial.println(time.timestamp(DateTime::TIMESTAMP_TIME));
 Serial.println("\n");

 //Delay 5s
 delay(5000);
}



void alarma() {
 DateTime fecha = rtc.now();        // funcion que devuelve fecha y horario en formato
              // DateTime y asigna a variable fecha
 if ( fecha.hour() == 14 && fecha.minute() == 30 ){ // si hora = 14 y minutos = 30
    if ( evento_inicio == true ){     // si evento_inicio es verdadero
      digitalWrite(RELE, HIGH);       // activa modulo de rele con nivel alto
      Serial.println( "Rele encendido" );   // muestra texto en monitor serie
      evento_inicio = false;        // carga valor falso en variable de control
    }             // para evitar ingresar mas de una vez
  }

 if ( fecha.hour() == 15 && fecha.minute() == 30 ){ // si hora = 15 y minutos = 30
    if ( evento_fin == true ){        // si evento_fin es verdadero
      digitalWrite(RELE, LOW);        // desactiva modulo de rele con nivel bajo
      Serial.println( "Rele apagado" );     // muestra texto en monitor serie
      evento_fin = false;       // carga valor falso en variable de control
    }             // para evitar ingresar mas de una vez
  }
