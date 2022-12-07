

/*
  Incubadora V- 0.1
  Index:
  1------------------------------------------- Lectura Tiempo real con RTC I2C
  1.1----------------------------------------- visualizacion tiempo restante proceso de incubacion
  2------------------------------------------- Visualizacion de fecha:hora en display display 0.96" I2C
  3------------------------------------------- Lectura Temperatura - Humedad con sensor  I2C
  4------------------------------------------- Visualizacion de T-H en Display display 0.96" 
  5------------------------------------------- Control de RELE 4 conecciones (Calefaccion, Iluminacion, Humidificador, Volteo)
  6------------------------------------------- configuracion e inicio ON/OFF del proceso de incubacion 
  7------------------------------------------- Funciones de finDeCarrera del motor de volteo
  8------------------------------------------- Funciones configuracion inicial por ingreso de parametros visualmente por display 0.96"

  Requiere instalar librerias Adafruit GFX y Adafruit SSD1306

  Autor: SAZ  

*/
#include "Adafruit_SHT31.h"
#include <MS5611.h>              // libreria GY-ms5611  Presion Barometrica
#include "RTClib.h"              // libreria para RTC reloj tiempo real
#include <Wire.h>                // libreria para bus I2C
#include <Adafruit_GFX.h>        // libreria para pantallas graficas
#include <Adafruit_SSD1306.h>    // libreria para controlador SSD1306
 
#define ANCHO 128                // reemplaza ocurrencia de ANCHO por 128
#define ALTO 64                  // reemplaza ocurrencia de ALTO por 64
#define display_RESET 4

Adafruit_SHT31 sht31 = Adafruit_SHT31();
Adafruit_SSD1306 display(ANCHO, ALTO, &Wire, display_RESET);  
RTC_DS3231 rtc;                  // Instancia Objeto rtc
DateTime tiempoPasado;           // Declara Fecha de inicio de incubacion
MS5611 baro;                     // Instancia objeto para sensado de presion Bar.


// -----   Declara las constantes -----
int32_t pressure;
int32_t temperatureBaro;
char daysOfTheWeek[7][12] = {"DOM", "LUN", "MAR", "MIE", "JUE", "VIE", "SAB"}; // dias de la semana .dayOfTheWeek()
static const unsigned char PROGMEM char_Temp[174] = {0x00, 0x07, 0xf8, 0x00, 0x00, 0x00, 0x0f, 0xfc, 0x00, 0x00,   0x00, 0x3f, 0xfe, 0x00, 0x00,   0x00, 0x3f, 0xfe, 0x00, 0x00,   0x00, 0x3f, 0xfe, 0x00, 0x00,   0x00, 0x3f, 0xfe, 0x00, 0x00,   0x00, 0x3f, 0xfe, 0x00, 0x00,   0x00, 0x3f, 0xfe, 0x00, 0x00,   0x00, 0x3c, 0x1e, 0x00, 0x00,   0x00, 0x3e, 0x3e, 0x00, 0x00,   0x00, 0x3c, 0x1e, 0x00, 0x00,   0x00, 0x3c, 0x9e, 0x00, 0x00,   0x00, 0x3e, 0x3e, 0x00, 0x00,   0x00, 0x3c, 0x1e, 0x00, 0x00,   0x00, 0x3c, 0x9e, 0x00, 0x00,   0x00, 0x3e, 0x3e, 0x00, 0x00,   0x00, 0x3c, 0x1e, 0x00, 0x00,   0x00, 0x3c, 0x9e, 0x00, 0x00,   0x00, 0x3c, 0x1e, 0x00, 0x00,   0x00, 0x3c, 0x1e, 0x00, 0x00,   0x00, 0xfc, 0x9f, 0x80, 0x00,   0x01, 0xfc, 0x1f, 0xc0, 0x00,   0x0f, 0xfc, 0x1f, 0xfc, 0x00, 0x7f, 0xc8, 0x89, 0xff, 0x00,  0x7f, 0x80, 0x00, 0xff, 0x00,  0xfe, 0x88, 0x88, 0xbf, 0x80, 0xfe, 0x00, 0x00, 0x3f, 0x80, 0xfe, 0x22, 0x22, 0x3f, 0x80, 0xfe, 0x00, 0x00, 0x3f, 0x80, 0x7f, 0x80, 0x00, 0xff, 0x00, 0x3f, 0xf2, 0x23, 0xfe, 0x00, 0x0f, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x7f, 0xff, 0x00, 0x00, };

void setup() {
  Wire.begin();                              // Inicializa bus I2C
  baro = MS5611();                           // Inicializa Barometro
  baro.begin();
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // Inicializa pantalla con direccion 0x3C
  if (! rtc.begin()){
  while (1) delay(10);
  }
  if (! sht31.begin(0x44)) {                 // Set to 0x44 for i2c addr
    while (1) delay(1);
  }
  digitalClockDisplay(); 
  display.clearDisplay();
}
 
void loop() {
  display.clearDisplay();                                 // Limpia pantalla
  DateTime ahora = rtc.now();                             // Obtiene fecha de inicio con Timmer rtc
  TimeSpan  tiempoIncubacion = ahora-tiempoPasado;        // calcula el tiempo transcurrido desde inicio
  tiempoTrans(ahora);                                     // Funcion Muestra por pantalla con formato definido
  //====  display sensores =========
  Sensor_presionBar();
  Sensor_SHT31_TempHum();

  showTimeSpan("tiempoIncubacion", tiempoIncubacion);     // Impresion tiempo de Incubacion. DEBE ejecutarse al final
  //================================
  
  if(rtc.getTemperature()<=28){
        Intermitente_ICON_Temp();
    }
 }


//1------------------------------------------- Lectura Tiempo real con RTC I2C
void tiempoTrans(DateTime now){
  display.setTextColor(WHITE);                  // establece color al unico disponible (pantalla monocromo)
  display.setCursor(0, 57);                     // ubica cursor en inicio de coordenadas 0,57
  display.setTextSize(1);                       // establece tamano de texto en 1
  display.print("(");display.print(daysOfTheWeek[now.dayOfTheWeek()]); display.print(") ");
                                                // escribe Fecha hora formato => AAAA/MM/DD  HH:MM:SS
  display.print(now.day(),DEC);display.print("/"); display.print(now.month(),DEC);display.print("/");display.print(now.year()-2000,DEC);display.print(" "); display.print(now.hour(),DEC);display.print(":"); display.print(now.minute(),DEC);display.print(":"); display.print(now.second(),DEC); 
  display.setCursor(0,0);
  display.print(rtc.getTemperature());
}
//1------------------------------------------- Lectura Tiempo real con RTC I2C

//1.1----------------------------------------- visualizacion tiempo restante proceso de incubacion
void digitalClockDisplay() {
 tiempoPasado = rtc.now();//+TimeSpan(21,0,0,0);
}
void showTimeSpan(const char* txt, const TimeSpan& ts) {
  display.setCursor(0, 46);                    // ubica cursor en inicio de coordenadas 0,0
  display.setTextSize(1);                      // establece tamano de texto en 1
  display.print("T-out:");
  display.print(ts.days(), DEC);
  display.print("d/");
  display.print(ts.hours(), DEC);
  display.print("h:");
  display.print(ts.minutes(), DEC);
  display.print("m:");
  display.print(ts.seconds(), DEC);
  display.print("s");
  display.display();
}
void Sensor_SHT31_TempHum(){
  float t = sht31.readTemperature();
  float h = sht31.readHumidity();
  display.setCursor(0, 40);                    // ubica cursor en inicio de coordenadas 0,0
  display.setTextSize(1);                      // establece tamano de texto en 1
  display.print("Hum:");
  display.print(h);
  display.print("%");
  display.setCursor(0, 30);                    // ubica cursor en inicio de coordenadas 0,0
  display.print("Tem:");
  display.setTextSize(1);                      // establece tamano de texto en 1
  display.print(t);
  display.print("*C");
}


void Sensor_presionBar(){
  pressure = baro.getPressure();               // Obtiene Presion de sensor barometrico
  temperatureBaro = baro.getTemperature();     // Obtiene temperatura de sensor barometrico         
  char* buffn="     ";                         //Cadena donde almacenaremos el número convertido
  dtostrf(temperatureBaro/100.0,2,2,buffn);    //Llamada a la función
  char bufferTem[5]="     ";                   //Buffer de la cadena donde se devuelve todo, número formateado y cadena concatenada
  char* formatoT="%s";                         //Cadena con la mascara a convertir
  sprintf(bufferTem, formatoT, buffn);
  display.setCursor(0, 10);                    // ubica cursor en inicio de coordenadas 0,0
  display.setTextSize(1);                      // establece tamano de texto en 1
  display.print("Tem: ");
  display.print(bufferTem);
  display.print(" *C");

  char* buffnP="      ";                        //Cadena donde almacenaremos el número convertido
  dtostrf(pressure/100.0,4,1,buffnP);           //Llamada a la función
  char bufferPre[6]=" ";                        //Buffer de la cadena donde se devuelve todo, número formateado y cadena concatenada
  char* formatoP="%s";
  sprintf(bufferPre, formatoP, buffnP);
  display.setCursor(0, 20);                     // ubica cursor en inicio de coordenadas 0,20                                 
  display.setTextSize(1);
  display.print("Pre: ");
  display.print(bufferPre);
  display.print(" mm");
  
}
void Intermitente_ICON_Temp(){
  delay(500);
  display.drawBitmap(97, 1, char_Temp, 33, 33, 1);
  display.display(); 
  delay(500);
  //============
  }
 //1.1----------------------------------------- visualizacion tiempo restante proceso de incubacion
