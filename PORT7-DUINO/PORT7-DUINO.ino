/**
 * Programado por: Carlos Willy Ruiz Villalobos
 * Version: 1.1
 * Email: willyruiz95@gmail.com
 * Cel: +51 959071246
 * Nota: Espero que te sea de utilidad, si te gustó no olvides suscribirte a mi canal y darle like
 * 
*/
#include <LiquidCrystal_I2C.h>
#include <RTClib.h>
#include <EEPROM.h>
#include <DHT.h>



byte post[15]; //bytearray rx
byte tx[28]; //bytearray tx
byte error[4] = {'L',0,'E','\n'};//byte errores
/*--SETUP--POS--EEPROM*/
const byte POS_PERIODO  = 200;//byte
const byte POS_DATEINIT = 1;//usigned long
const byte POS_DAYSINCU = 5;//byte
const byte POS_TEMP     = 6;//float
const byte POS_HUM      = 10;//byte
const byte POS_INTERVAL = 28;//byte
const byte POS_WHILE    = 15;//byte
const byte POS_NACEDORA = 19;//byte
const byte POS_DAYINIT  = 20;//byte
const byte POS_VOLTEO   = 21;//byte
const byte POS_NTEMP    = 22;//float
const byte POS_NHUM     = 26;//byte
const byte POS_ALARM    = 27;//byte

const byte POS_CALIB_TUNIXTIME  = 100;
const byte POS_CALIB_UNIXTIME   = 101;
const byte POS_CALIB_TEMP       = 102;
const byte POS_CALIB_HUM        = 106;

const byte POS_CALIB_TIME_MOD   = 107;
const byte POS_CALIB_TEMP_MOD   = 108;
const byte POS_CALIB_HUM_MOD    = 109;
/*--SETUP--POS--EEPROM*/
/*------variables de configuracion*/
byte   sys_periodo;
unsigned long sys_dateinit;
byte  sys_daysincu;
float sys_temp;
byte  sys_hum;
int   sys_interval;
byte  sys_while;
byte  sys_nacedora;
byte  sys_dayinit;
byte  sys_volteo;
float sys_ntemp;
byte  sys_nhum;
byte  sys_alarm;

byte  sys_calib_tunixtime;
byte  sys_calib_unixtime;
float sys_calib_temp;
byte  sys_calib_hum;

byte  sys_calib_time_mod;
byte  sys_calib_temp_mod;
byte  sys_calib_hum_mod;

byte sys_fday   =0;
byte sys_fmonth =0;
int  sys_fyear  =0;
byte sys_fhour  =0;
byte sys_fminut =0;

byte  in_currentTime        = 0;
int   in_secForRotation     = 0;//variables para rotación
byte  in_setActiveRotation  = 0;
int   in_conterRotation     = 0;
byte  in_controlNacedora    = 1;
byte  in_controlVolteo      = 1;
bool  in_reloj              = true;
bool  in_onReady            = true;
byte  in_time_error         = 2;

/*------variables de configuracion*/

float   TEMPERATURA;
byte    HUMEDAD;
int     sinVal;
int     toneVal=0;

/*--declaracion-de-pines---*/
const byte pinHumidificador = 10;
const byte pinCalefaccion   = 11;
const byte pinVolteo        = 12;
const byte pinLcdOn         = 13;  
const byte pindht22         = 2;     
const byte pinBuzzer        = 3;
const byte pin_btn_enter    = 5;
const byte pin_btn_up       = 7;
const byte pin_btn_down     = 6;
/*--declaracion-de-pines---*/


long          TiempoFuncionando           = 0;
long          MaximoTiempoFuncionamiento  = 0;
const long    SegundosPorDia              = 86400;
boolean       SistemaParado               = false;
int           numRotaciones               = 0;
//unsigned long interval;
unsigned long in_timeSenData              =0;

//paramteros de eventos manuales para activaciono desativacion de calefactor/humidificador/volteo
bool handleIsActive      = false;
bool handleCalefactor    = false;
bool handleHumidificador = false;
bool handleVolteo        = false;
//-------parametros-manuales------------

/*---Vars GUI---------------------------------------------------------------------*/
   byte btn_enter      = 0;
   byte press_se;
   byte press_up;
   byte press_do;
   int  x              = 0;
   byte lgt_menu       = 7;
   byte ico_temp[]     = {B10101,B01110,B11111,B01110,B00100,B00100,B00100,B01110};
   byte ico_hum[]      = {B01110,B11111,B11111,B11111,B00000,B10101,B00000,B01010};
   byte ico_tray[]     = {B11110,B00101,B11110,B00000,B00000,B01111,B10100,B01111};
   byte ico_calendar[] = {B00000,B01010,B11111,B10101,B11011,B10101,B11111,B00000};
   byte ico_clock[]    = {B00000,B00000,B11111,B11111,B11011,B11001,B11111,B11111};
   byte ico_egg[]      = {B00000,B00100,B01110,B11111,B11111,B11111,B01110,B00100};
   byte ico_bell[]     = {B00000,B00100,B01110,B11111,B01110,B11111,B11111,B00100};
   byte ico_tool[]     = {B00000,B00000,B10101,B01110,B11111,B01110,B10101,B00000};
   bool ssetup         = 0; //controla la edicion de valores
   int  ssoption       = -1;//controla la variable a editar por vector asi como el cursor;
   bool ssave          = 0;//guarda la nueva configuracion en la eeprom
   const char  menu[7][12]      = {"Incubadora","Nacedora","Calibracion","Actuadores","Sensores","Fecha","Salir"};
/*-------------------------------------------------------------------------------------------------------------------*/
LiquidCrystal_I2C lcd(0x3F,16,2);  //0x3F
RTC_DS3231 rtc;
DHT dht(pindht22, DHT22);
/*===============
 Funcion Setup
===============*/
void setup() {
  Serial.begin(9600);
  if (!rtc.begin()){
    error[1] = 3;
    Serial.write(error,4);
    Serial.println(F("Modulo RTC no encontrado !"));  
    in_reloj = false;
  }
  lcd.init();
  lcd.backlight();
  dht.begin();
  pinMode(pinHumidificador, OUTPUT);
  digitalWrite(pinHumidificador,HIGH);
  pinMode(pinCalefaccion, OUTPUT);
  digitalWrite(pinCalefaccion,HIGH);
  pinMode(pinVolteo, OUTPUT);
  digitalWrite(pinVolteo,HIGH);
  pinMode(pinLcdOn, OUTPUT);   //declara pin encendido lcd como salida
  pinMode(pinBuzzer,OUTPUT);

  pinMode(pin_btn_enter,INPUT_PULLUP);
  pinMode(pin_btn_up,INPUT_PULLUP);
  pinMode(pin_btn_down,INPUT_PULLUP);
  
  //creamos los nuevos caracteres
  lcd.createChar (0,ico_temp);
  lcd.createChar (1,ico_hum);
  lcd.createChar (2,ico_tray);
  lcd.createChar (3,ico_calendar);
  lcd.createChar (4,ico_clock);
  lcd.createChar (5,ico_egg);
  lcd.createChar (6,ico_bell);
  lcd.createChar (7,ico_tool);
 
  //----SET-VARS-EEPROM
  getDataSetupEeprom();
  setDataSetup();
  //TiempoFuncionando = sys_dateinit;
}

void loop(){

  if(!in_reloj){
    if(in_time_error>=2) renderError(3);
  }else if(sys_periodo!=1){
    if(in_time_error>=2) renderError(2);
  }else if(!in_onReady){
    if(in_time_error>=2) renderError(1); 
  }else if(SistemaParado){
    if(in_time_error>=2) renderError(0);
  }else;

  /*==========================================*/ 
  if(isOneSecond()){ /*contador de segundos*/
    in_time_error++;
    in_timeSenData++;
    TiempoFuncionando++;
    in_secForRotation++;
    if(in_setActiveRotation) in_conterRotation++;
  }
  
  /*--------------------------SEGMENTO DE GUI----------------------------------------------*/
  if(in_onReady && !SistemaParado && sys_periodo==1 && in_reloj!=false && btn_enter==0){
    screenMain( TiempoFuncionando );
  }

  screenSetup();
  

  /*---------------------------------------------------------------------------------------*/
  
  /*si el intervalo de tiempo es mayor o igual al tiempo de volteo de activa rotacion de bandejas.*/
  if(in_secForRotation > sys_interval && !handleIsActive && in_controlVolteo && in_onReady && !SistemaParado){//in_secForRotation && sys_volteo>0
      in_secForRotation     = 0;
      in_setActiveRotation  = 1;
      numRotaciones = numRotaciones++;
  }

  if(in_setActiveRotation && in_onReady && !SistemaParado){//Realiza rotaciones sin bloquear el codigo
      if(in_conterRotation<=sys_while){
          digitalWrite(pinVolteo,LOW);
      }else{
          digitalWrite(pinVolteo,HIGH);
          in_conterRotation     = 0;
          in_setActiveRotation  = 0;
          lcd.setCursor(11,1);
          lcd.print(" ");//clean rotation
      }
  }
  
  
  if(TiempoFuncionando > MaximoTiempoFuncionamiento && in_onReady && !SistemaParado){//llegando al limite incubacion
      SistemaParado = true;
      apagarSalidas();
  }

 
  /*--------init-nacedora---------------*/
  {
    if(sys_nacedora && in_controlNacedora && in_onReady && !SistemaParado){
        if(TiempoFuncionando >= (sys_dayinit*86400)){
            sys_temp  = sys_ntemp;
            sys_hum   = sys_nhum;
            if(sys_volteo) in_controlVolteo = 0;
            in_controlNacedora=0;
        }
    }  
  }


  /*End nacedora------------------------*/
  if(in_timeSenData>=2 && !SistemaParado){
            in_timeSenData=0;
            /*TEMPERATURA   = dht.readTemperature() + (sys_calib_temp_mod==0 ? sys_calib_temp*-1 : sys_calib_temp);  //read temp | habilitado para caibracion de parametros
            HUMEDAD       = dht.readHumidity()    + (sys_calib_hum_mod ==0 ? sys_calib_hum*-1 : sys_calib_hum);   // read  humidity*/

            TEMPERATURA   = dht.readTemperature();  //read temp
            HUMEDAD       = dht.readHumidity();   // read  humidity

            
            if (TEMPERATURA > sys_temp && !handleIsActive && in_onReady) {
                digitalWrite(pinCalefaccion, HIGH);
                if(sys_alarm==1) noBuzzer();
            }
    
            if(TEMPERATURA <=sys_temp && !handleIsActive && in_onReady){
                digitalWrite(pinCalefaccion, LOW);
                if(sys_alarm==1) buzzer();
            }
    
            if (HUMEDAD > sys_hum && !handleIsActive && in_onReady) {
              digitalWrite(pinHumidificador, HIGH);
            }
            
            if (HUMEDAD <= sys_hum && !handleIsActive && in_onReady) {
              digitalWrite(pinHumidificador, LOW);
            }

            /* byte rtemp[4]; envia temp y hum por serial
             byte realData[8];
             floatToByte(rtemp, TEMPERATURA);
             realData[0] = 'R';
             realData[1] = rtemp[3];
             realData[2] = rtemp[2];
             realData[3] = rtemp[1];
             realData[4] = rtemp[0];
             realData[5] = HUMEDAD;
             realData[6] = 'E';
             realData[7] = '\n';
             Serial.write(realData,8);*/
  }
   
  
}

void apagarSalidas(){
   digitalWrite(pinCalefaccion,1);
   digitalWrite(pinHumidificador,1);
   digitalWrite(pinVolteo,1);
}


/*
========================================
 Funcion SacaTiempoHorasMinutosSegundos
========================================
*/
void SacaTiempoHorasMinutosSegundos(long iTiempo){
  if (iTiempo < 0) iTiempo *= -1;
  int iminutos = iTiempo / 60;
  int isegundos = iTiempo - (iminutos * 60);
  int ihoras = iminutos / 60;
  iminutos = iminutos - (ihoras * 60);
}


/*
=========================
 Funcion screenMain
=========================
*/
void screenMain( long iTiempo ){
  long iminutos = iTiempo / 60;
  long isegundos = iTiempo - (iminutos * 60);
  long ihoras = iminutos / 60;
  iminutos = iminutos - (ihoras * 60);
  long idias = ihoras / 24;
  ihoras = ihoras - (idias * 24);

  lcd.setCursor(0,0);
  lcd.write(byte(3));
  if(idias<10) lcd.print("0");
  lcd.print(idias);
  lcd.print("d");
  lcd.setCursor(5,0);
  lcd.write(byte(0));
  lcd.print(TEMPERATURA);
  lcd.setCursor(13,0);
  lcd.write(byte(1));
  lcd.print(HUMEDAD);
  lcd.setCursor(0,1);
  lcd.write(byte(4));
  if(ihoras<10) lcd.print("0");
  lcd.print(ihoras);
  lcd.print(":");
  if(iminutos<10) lcd.print("0");
  lcd.print(iminutos);
  lcd.print(":");
  if(isegundos<10) lcd.print("0");
  lcd.print(isegundos);
  if(in_setActiveRotation){
    lcd.setCursor(11,1);
    lcd.write(byte(2));//rotacion
  }
  if(sys_nacedora){
    lcd.setCursor(13,1);
    lcd.write(byte(5));//nacedora
  }
  
  if(sys_alarm){
    lcd.setCursor(15,1);
    lcd.write(byte(6));//alarma
  }else{
     lcd.setCursor(15,1);
    lcd.print(" ");//alarma
  }

  if(!digitalRead(pinCalefaccion)){
    lcd.setCursor(12,1);
    lcd.write(byte(0));//calefactor
  }else{
    lcd.setCursor(12,1);
    lcd.print(" ");//calefactor
  }

  if(!digitalRead(pinHumidificador)){
    lcd.setCursor(14,1);
    lcd.write(byte(1));//Humidificador
  }else{
    lcd.setCursor(14,1);
    lcd.print(" ");//Humidificador
  }


  
}

/*
  ====================
   Funcion buzzer
  ====================
*/
void buzzer(){
    for(int x=0; x<50; x++){
        // convertimos grados en radianes para luego obtener el valor.
        sinVal = (sin(x*(3.1412/180)));
        // generar una frecuencia a partir del valor sin
        toneVal = 1000+(int(sinVal*1000));
        tone(pinBuzzer, toneVal);
        delay(5); 
    } 
    noTone(pinBuzzer);
}
/*
 ==============================
 de arraybyte a integer
 ==============================
 */
 unsigned long getUnsignedLong(byte post[],int i){
      unsigned long  value=0;
      value = (value * 256) + post[i];
      value = (value * 256) + post[i+1];
      value = (value * 256) + post[i+2];
      value = (value * 256) + post[i+3];
      return value;
 }

/*
 ============================
 De array to float
 ============================
*/
float getFloat(byte packet[], int i){
  union u_tag{
  byte bin[4];
  float num;
  } u;
  u.bin[0] = packet[i];
  u.bin[1] = packet[i + 1];
  u.bin[2] = packet[i + 2];
  u.bin[3] = packet[i + 3];
  float out = u.num;
  return out;
}/*
float getFloat(byte packet[], int i){
    float out;
    memcpy(&out, &packet[i], sizeof(float));
    return out;
}*/

/*
  ====================
   Funcion nobuzzer
  ====================
*/
void noBuzzer(){
  noTone(pinBuzzer);
}


/*
======================
 controla si ha pasado un segundo // cn el modulos ds3231
======================
*/
bool isOneSecond(){
  bool isTrue = false;
  DateTime fecha = rtc.now();      // funcion que devuelve fecha y horario en formato
  byte date = fecha.second();
  if(((date==0 && in_currentTime==59) ? 60 : date) - in_currentTime >= 1 ){
     in_currentTime =  date;
     isTrue = true;
  }
  return isTrue;
}

/*
 =========================
 Retorna parametros
 ========================= 
*/

void getParams(){
  unsigned long sus_dateinit = 0;
  float sus_temp             = 0;
  byte sus_hum               = 0;
  EEPROM.get(POS_DATEINIT,sus_dateinit);
  EEPROM.get(POS_TEMP,sus_temp);
  EEPROM.get(POS_HUM,sus_hum);
  byte ddinit[4];
  byte atemp[4];
  byte btemp[4];
  intToByte(ddinit, sus_dateinit);
  floatToByte(atemp, sus_temp);
  floatToByte(btemp, sys_ntemp);
  tx[0]=73;
  tx[1] =  ddinit[0];
  tx[2] =  ddinit[1];
  tx[3] =  ddinit[2];
  tx[4] =  ddinit[3];
  
  tx[5] = sys_daysincu;
  
  tx[6] = atemp[3];
  tx[7] = atemp[2];
  tx[8] = atemp[1];
  tx[9] = atemp[0];
  
  tx[10] = sus_hum;
  tx[11] = EEPROM.read(POS_INTERVAL);
  tx[12]= sys_while;
  tx[13]= sys_nacedora;
  tx[14]= sys_dayinit;
  tx[15]= sys_volteo;
  
  tx[16]= btemp[3];
  tx[17]= btemp[2];
  tx[18]= btemp[1];
  tx[19]= btemp[0];
  
  tx[20]= sys_nhum;
  
  tx[21]= (handleIsActive) ? 1 : 0;
  tx[22]= (handleCalefactor) ? 1: 0;
  tx[23]= (handleHumidificador) ? 1:0;
  tx[24]= (handleVolteo) ? 1 : 0;
  tx[25]= sys_alarm;
  tx[26]=69;
  tx[27]= '\n';
  
  Serial.write(tx,28);
  Serial.flush();
}

void getCalibParams(){
  byte mCalibData[13];
  byte cc_temp[4];
  floatToByte(cc_temp, sys_calib_temp);
  mCalibData[0] =   74;
  mCalibData[1] =   sys_calib_tunixtime;
  mCalibData[2] =   sys_calib_unixtime;
  mCalibData[3] =   cc_temp[3];
  mCalibData[4] =   cc_temp[2];
  mCalibData[5] =   cc_temp[1];
  mCalibData[6] =   cc_temp[0];
  mCalibData[7] =   sys_calib_hum;
  mCalibData[8] =   sys_calib_time_mod;
  mCalibData[9] =   sys_calib_temp_mod;
  mCalibData[10] =  sys_calib_hum_mod;
  mCalibData[11] =  69;
  mCalibData[12] = '\n';
  Serial.write(mCalibData,13);
  Serial.flush();
}
/*
 =========================
 Set events handles
 ========================= 
*/
void setEventsHandles(int type,byte value){
           switch(type){
           case 0:
            if(handleIsActive==true){
                if(value==1){
                    handleCalefactor = true;
                    digitalWrite(pinCalefaccion,LOW);
                }else{
                    handleCalefactor = false;
                    digitalWrite(pinCalefaccion,HIGH);
                 
                }
            }
           break; 

           case 1://calefactor
             if(handleIsActive){
                  if(value==1){
                    handleHumidificador = true;
                    digitalWrite(pinHumidificador,LOW);
                  }else{
                    handleHumidificador = false;
                    digitalWrite(pinHumidificador,HIGH);
                  }
             }
           break; 

           case 2://volteo
             if(handleIsActive){
                if(value==1){
                  handleVolteo = true;
                  digitalWrite(pinVolteo,LOW);
                }else{
                  handleVolteo = false;
                  digitalWrite(pinVolteo,HIGH);
                }
             }
           break; 
           case 3://alarma
                if(value==1){
                  sys_alarm =1;
                  EEPROM.put(POS_ALARM,sys_alarm);
                }else{
                  sys_alarm =0;
                  EEPROM.put(POS_ALARM,sys_alarm);
                }
           break; 
           case 4://move bandejas
              in_setActiveRotation=1;
           break; 
           case 10://permite eventos manuales
            if(value==1){
              handleIsActive = true;
              //digitalWrite(pinCalefaccion,1);
              //digitalWrite(pinVolteo,1);
              //digitalWrite(pinHumidificador,1);
            }else{
              handleIsActive = false;
              handleCalefactor=false;
              handleHumidificador=false;
              handleVolteo=false;
            }
            apagarSalidas();
           break; 
         }
}
/*
 ==========================
 de entero a byte array
 input lng entero long unsigned
 ==========================
 */
void intToByte(byte* bytes,long lng){
    bytes[0] = (byte) ((lng & 0xFF000000) >> 24 );
    bytes[1] = (byte) ((lng & 0x00FF0000) >> 16 );
    bytes[2] = (byte) ((lng & 0x0000FF00) >> 8  );
    bytes[3] = (byte) ((lng & 0X000000FF)       );
}
/*
 ==========================
 de flotante a byte array
 input bytes => byte[2]
 input f: flotante
 ==========================
 */
void floatToByte(byte* bytes, float f){
     int length = sizeof(float);
     for(int i = 0; i < length; i++){
       bytes[i] = ((byte*)&f)[i];
     }
}

void getDataSetupEeprom(){
    sys_periodo   = EEPROM.read(POS_PERIODO);
    EEPROM.get(POS_DATEINIT,sys_dateinit);
    sys_daysincu  = EEPROM.read(POS_DAYSINCU);
    EEPROM.get(POS_TEMP,sys_temp);
    sys_hum       = EEPROM.read(POS_HUM);
    sys_interval  = EEPROM.read(POS_INTERVAL);
    sys_while     = EEPROM.read(POS_WHILE);
    
    sys_nacedora  = EEPROM.read(POS_NACEDORA);
    sys_dayinit   = EEPROM.read(POS_DAYINIT);
    sys_volteo    = EEPROM.read(POS_VOLTEO);
    EEPROM.get(POS_NTEMP,sys_ntemp);
    sys_nhum      = EEPROM.read(POS_NHUM);
    sys_alarm     = EEPROM.read(POS_ALARM);

    sys_calib_tunixtime = EEPROM.read(POS_CALIB_TUNIXTIME);
    sys_calib_unixtime  = EEPROM.read(POS_CALIB_UNIXTIME);
    EEPROM.get(POS_CALIB_TEMP,sys_calib_temp);
    sys_calib_hum       = EEPROM.read(POS_CALIB_HUM);

    sys_calib_time_mod       = EEPROM.read(POS_CALIB_TIME_MOD);
    sys_calib_temp_mod       = EEPROM.read(POS_CALIB_TEMP_MOD);
    sys_calib_hum_mod        = EEPROM.read(POS_CALIB_HUM_MOD);
   
}

void setDataSetup(){
     if(sys_periodo==255){
        sys_update_incubadora(1613697690,21,37.50,55,2,15,1);
        sys_update_nacedora(0,18,0,33.50,75);
        EEPROM.update(POS_ALARM,1);
        sys_update_calibracion(0,1,5,0,1);
        sys_update_calibracion(1,1,0,0,0);
        sys_update_calibracion(2,1,0,0,0);
        getDataSetupEeprom();/*recuperamos valor de eeprom*/
    }
    sys_interval = (sys_interval*60)*60;//De horas a segundos
    renderDateinit();
    MaximoTiempoFuncionamiento = (long)sys_daysincu * SegundosPorDia;
    if(TiempoFuncionando > MaximoTiempoFuncionamiento) SistemaParado = true;
}

void renderDateinit(){
  int adition = 0;
  //adition = (sys_calib_tunixtime==0) ? sys_calib_unixtime * 3600 : sys_calib_unixtime *60;
  adition = 5 * 3600;
  //if(sys_calib_time_mod==0) adition*=-1;
  unsigned long currentTime = (rtc.now().unixtime()+adition);
  EEPROM.get(POS_DATEINIT,sys_dateinit);
  
  /*validamos calidad de fechas*/
  if(currentTime < sys_dateinit){
    in_onReady = false;
  }else if(currentTime >= sys_dateinit && in_onReady==false){
    in_onReady = true;
  }else;
  /*----------------------------*/

  DateTime    currentUnixtime(sys_dateinit);
  sys_fday    = currentUnixtime.day();
  sys_fmonth  = currentUnixtime.month();
  sys_fyear   = currentUnixtime.year();
  sys_fhour   = currentUnixtime.hour();
  sys_fminut  = currentUnixtime.minute();
  
 if(in_onReady){ 
  sys_dateinit        = (currentTime - sys_dateinit);//time in seconds
  TiempoFuncionando   =  sys_dateinit;
 }
}

/**
 * GRAPHIC INTERFACE USER IN SETUP
 */
void screenSetup(){
  /*debouncing*/ 
   if (digitalRead(pin_btn_enter) == LOW) press_se = 1;
   if (digitalRead(pin_btn_up) == LOW) press_up = 1;
   if (digitalRead(pin_btn_down) == LOW) press_do = 1;
   /*---------------------------------------------------*/

   /* button enter*/
   if (digitalRead(pin_btn_enter) == HIGH && press_se == 1){
          press_se=0;
          /*entramos a pantalla 1*/
          if(btn_enter<2) btn_enter++;
         
          /*Salimos de pantalla 1*/
          if(x==(lgt_menu-1)){
              btn_enter=0;
              x=0;
          }
        
          /*entramos a pantalla 2 opciones de vista y configuracion*/
          if(btn_enter==2){
           if(ssoption==0) cleanSetup(1);//salimos de menu
          
           switch(x){
              case 0://options incubadora
                  if(ssave && ssoption==6){
                      cleanSetup(0);
                      /*Guardams en eeprom opciones de incubadora*/
                      sys_update_incubadora(0,sys_daysincu,sys_temp,sys_hum,(sys_interval/3600),sys_while,1);
                      toneSalve();
                  }
                  if(ssoption>0 && ssoption<6){ //entramos a setup-edicion
                    ssetup=!ssetup;
                    ssave=1;
                  }
              break;
              case 1://options nacedora
                  if(ssave && ssoption==6){
                      cleanSetup(0);
                      /*Guardams en eeprom varables de nacedora*/
                      sys_update_nacedora(sys_nacedora,sys_dayinit,sys_volteo,sys_ntemp, sys_nhum);
                      in_controlNacedora = 1;
                      in_controlVolteo = 1;
                      toneSalve();
                   }
                   if(ssoption>0 && ssoption<6){
                      ssetup=!ssetup;
                      ssave=1;
                    }
              break;
              case 3://actuadores
                   if(ssoption==0) cleanSetup(1);
                   if(ssoption>0 && ssoption<4){
                      ssetup=!ssetup;
                      //handleIsActive = !handleIsActive;
                      setEventsHandles(10,!handleIsActive);
                     
                      /*modificamos handle_active para activar o desactivar los eventos manuales*/
                    }
              break;
              case 5://options dateinit
                  if(ssave && ssoption==6){
                      cleanSetup(0);
                      /*Guardams en eeprom opciones de fecha de incubacion*/
                      DateTime currentDate (sys_fyear, sys_fmonth, sys_fday, sys_fhour, sys_fminut, 0);
                      unsigned long myDate = currentDate.unixtime()+(5*3600);
                      EEPROM.put(POS_DATEINIT,myDate);
                      renderDateinit();
                      SistemaParado = false;
                      getDataSetupEeprom();
                      toneSalve();
                  }
                  if(ssoption>0 && ssoption<6){ //entramos a setup-edicion
                    ssetup=!ssetup;
                    ssave=1;
                  }
              break;
          }
         }
         lcd.clear();
   }

   
   if (digitalRead(pin_btn_up) == HIGH && press_up == 1){
          press_up=0;
          if(btn_enter>0){
           
            if(btn_enter==1){//pantalla 2
                x--;
                x = x<0 ? x = lgt_menu-1 : x;
            }
            
            if(btn_enter==2){//pantalla 3
              if(!ssetup){
               ssoption--;
               if(x==0 || x==1 || x==5) ssoption = ssoption<0 ? ssoption =6  : ssoption;
               if(x==2 || x==4) ssoption = ssoption<0 ? ssoption =0  : ssoption;
               if(x==3) ssoption = ssoption<0 ? ssoption =3  : ssoption;
              }else{
                  if(x==0)  setupMenuVarios(0,0);//incubadora
                  if(x==1)  setupMenuVarios(1,0);//nacedora
                  if(x==3)  setupMenuVarios(2,0); //actuadores
                  if(x==5)  setupMenuVarios(3,0); //fecha init
              }
            }else;

            lcd.clear();
          }
   }

   
   if (digitalRead(pin_btn_down) == HIGH && press_do == 1){
            press_do=0;
            if(btn_enter==1){//pantalla 2
                x++;
                x = x>(lgt_menu-1) ? x = 0 : x;
            }else if(btn_enter==2){//pantalla 3
                if(!ssetup){
                  ssoption++;
                  if(x==0 || x==1 || x==5) ssoption = ssoption>6 ? ssoption = 0 : ssoption;
                  if(x==2 || x==4) ssoption = ssoption>0 ? ssoption = 0 : ssoption;
                  if(x==3) ssoption = ssoption>3 ? ssoption = 0 : ssoption;
                }else{//ingresamos a edicion de parametros;
                   if(x==0) setupMenuVarios(0,1);//incubadora
                   if(x==1) setupMenuVarios(1,1);//nacedora
                   if(x==3) setupMenuVarios(2,1);//actuadores
                   if(x==5) setupMenuVarios(3,1);//fecha init
                }
            }else;
          lcd.clear();
          }
   //}

      if(btn_enter==1){//menu second btn_setup=2
            lcd.setCursor(0,0);
            lcd.print("MAIN MENU");
            lcd.setCursor(0,1);
            lcd.print(menu[x]);
      }else if(btn_enter==2){//opciones de menu vistas y setup
              switch(x){
                case 0://incubadora
                    lcd.setCursor(0,0);
                    lcd.write(byte(3));
                    if(sys_daysincu<10) lcd.print("0");
                    lcd.print(sys_daysincu);
                    ssoption==1 && lcd.print("<");//daysincu
                    lcd.setCursor(4,0);
                    lcd.write(byte(0));
                    lcd.print(sys_temp);
                     ssoption==2 && lcd.print("<");//temp
                    lcd.setCursor(11,0);
                    lcd.write(byte(1));//rain
                    if(sys_hum<10) lcd.print("0");
                    lcd.print(sys_hum);
                    ssoption==3 && lcd.print("<");//hum
                    lcd.setCursor(0,1);
                    lcd.write(byte(2));//tray
                    if(sys_interval<10) lcd.print("0");
                    lcd.print(sys_interval/3600);
                    ssoption==4 && lcd.print("<");//tray
                    lcd.setCursor(4,1);
                    lcd.write(byte(4));//clock
                    if(sys_while<10) lcd.print("0");
                    lcd.print(sys_while);
                    ssoption==5 && lcd.print("<");//while
                    
                    if(ssetup){//setup save
                      lcd.setCursor(10,1);
                      lcd.write(byte(7));
                    }
                    if(!ssetup && ssave){
                        lcd.setCursor(10,1);
                        ssoption==6 ? lcd.print(">S"): lcd.print(" S");
                     }                                       
                    lcd.setCursor(14,1);
                    ssoption==0 ? lcd.print(">E") : lcd.print(" E");
                break;
                case 1://nacedora
                    lcd.setCursor(0,0);
                    lcd.print((sys_nacedora ? "SI" : "NO"));
                    ssoption==1 && lcd.print("<");
                    lcd.setCursor(3,0);
                    lcd.write(byte(3));//adays
                    if(sys_dayinit<10) lcd.print("0");
                    lcd.print(sys_dayinit);
                    ssoption==2 && lcd.print("<");
                    lcd.setCursor(7,0);
                    lcd.write(byte(0));//atemp
                    lcd.print(sys_ntemp);
                    ssoption==3 && lcd.print("<");
                    lcd.setCursor(0,1);
                    lcd.write(byte(1));//rain
                    lcd.print(sys_nhum);
                    ssoption==4 && lcd.print("<");
                    lcd.setCursor(4,1);
                    lcd.write(byte(2));//tray
                    lcd.print((sys_volteo ? "SI" : "NO"));
                    ssoption==5 && lcd.print("<");
                    if(ssetup){//setup save
                      lcd.setCursor(10,1);
                      lcd.write(byte(7));
                    }
                    if(!ssetup && ssave){
                        lcd.setCursor(10,1);
                        ssoption==6 ? lcd.print(">S"): lcd.print(" S");
                     }                                       
                    lcd.setCursor(14,1);
                    ssoption==0 ? lcd.print(">E") : lcd.print(" E");
                break;
                case 2://calib
                    lcd.setCursor(0,0);
                    lcd.write(byte(4));
                    if(sys_calib_unixtime<10) lcd.print("0");
                    lcd.print(sys_calib_unixtime);
                    lcd.setCursor(6,0);
                    lcd.write(byte(1));
                    if(sys_calib_hum<10) lcd.print("0");
                    lcd.print(sys_calib_hum);
                    lcd.setCursor(11,0);
                    lcd.write(byte(0));
                    lcd.print(sys_calib_temp);
                    lcd.setCursor(14,1);
                    ssoption==0 ? lcd.print(">E") : lcd.print("E");
                break;
                case 3://actuadores
                    lcd.setCursor(0,0);
                    lcd.write(byte(0));
                    lcd.print(handleCalefactor ? "ON" : "OF");
                    ssoption==1 && lcd.print("<");
                    lcd.setCursor(4,0);
                    lcd.write(byte(1));
                    lcd.print(handleHumidificador ? "ON" : "OF");
                    ssoption==2 && lcd.print("<");
                    lcd.setCursor(8,0);
                    lcd.write(byte(2));
                    lcd.print(handleVolteo ? "ON" : "OF");
                    ssoption==3 && lcd.print("<");
                    /*setup and save*/
                    if(ssetup){
                      lcd.setCursor(10,1);
                      lcd.write(byte(7));
                    }
                    if(!ssetup && ssave){
                        lcd.setCursor(10,1);
                        ssoption==6 ? lcd.print(">S"): lcd.print(" S");
                     }                                       
                    lcd.setCursor(14,1);
                    ssoption==0 ? lcd.print(">E") : lcd.print(" E");
                break;
                case 4://sensores
                    lcd.setCursor(0,0);
                    lcd.write(byte(0));
                    lcd.print(TEMPERATURA);
                    lcd.setCursor(0,1);
                    lcd.write(byte(1));
                    if(HUMEDAD<10) lcd.print("0");
                    lcd.print(HUMEDAD);
                    lcd.setCursor(14,1);
                    ssoption==0 ? lcd.print(">E") : lcd.print("E");
                break;
                case 5://fecha incubación
                    lcd.setCursor(0,0);
                    if(sys_fday<10) lcd.print("0");
                    lcd.print(sys_fday);
                    ssoption==1 ? lcd.print("<") : lcd.print("-");
                    lcd.setCursor(3,0);
                    if(sys_fmonth<10) lcd.print("0");
                    lcd.print(sys_fmonth);
                    ssoption==2 ? lcd.print("<"): lcd.print("-");
                    lcd.setCursor(6,0);
                    lcd.print(sys_fyear);
                    ssoption==3 && lcd.print("<");
                    lcd.setCursor(0,1);
                    if(sys_fhour<10) lcd.print("0");
                    lcd.print(sys_fhour);
                    ssoption==4 ?lcd.print("<") : lcd.print(":");
                    lcd.setCursor(3,1);
                    if(sys_fminut<10) lcd.print("0");
                    lcd.print(sys_fminut);
                    ssoption==5 && lcd.print("<");
                    
                    if(ssetup){//setup save
                      lcd.setCursor(12,1);
                      lcd.write(byte(7));
                    }
                    if(!ssetup && ssave){
                        lcd.setCursor(14,0);
                        ssoption==6 ? lcd.print(">S"): lcd.print(" S");
                     }                                       
                    lcd.setCursor(14,1);
                    ssoption==0 ? lcd.print(">E") : lcd.print(" E");
                break;
              }
        
      }else;
   
}

void setupMenuVarios(byte op,bool type){
    if(op==0){//incubadora
        ssoption==1 && (type ? sys_daysincu++ : sys_daysincu--);
        ssoption==2 && (type ? (sys_temp= sys_temp+0.1) : (sys_temp= sys_temp-0.1) );
        ssoption==3 && (type ? sys_hum++ : sys_hum--);
        if(ssoption==4) {
          sys_interval = (sys_interval/3600);
               if(type){
                 sys_interval++;
               }else{
                 sys_interval--; 
               }
           sys_interval = (sys_interval*3600);
        }
        
        
        ssoption==5 && (type ? sys_while++ : sys_while--);
    }else if(op==1){//nacedora
        ssoption==1 && (sys_nacedora = !sys_nacedora);
        ssoption==2 && (type ? sys_dayinit++ : sys_dayinit--);
        ssoption==3 && (type ? (sys_ntemp= sys_ntemp+0.1) :(sys_ntemp= sys_ntemp-0.1) ) ;
        ssoption==4 && (type ? sys_nhum++ : sys_nhum--);
        ssoption==5 && (sys_volteo = !sys_volteo);
    }else if(op==2){//actuadores
       /* ssoption==1 && (handleCalefactor = !handleCalefactor);
        ssoption==2 && (handleHumidificador = !handleHumidificador);
        ssoption==3 && (handleVolteo = !handleVolteo);
       */
        if(ssoption==1) setEventsHandles(0,!handleCalefactor);
        if(ssoption==2) setEventsHandles(1,!handleHumidificador);
        if(ssoption==3) setEventsHandles(2,!handleVolteo);
       /*---------------------------------------------------------*/
    }else if(op==3){//fecha de inicio
        ssoption==1 && (type ? sys_fday++ : sys_fday--);
        ssoption==2 && (type ? sys_fmonth++ : sys_fmonth--);
        ssoption==3 && (type ? sys_fyear++ : sys_fyear--);
        ssoption==4 && (type ? sys_fhour++ : sys_fhour--);
        ssoption==5 && (type ? sys_fminut++ : sys_fminut--);
    }else;
}

void cleanSetup(byte screen){
    ssoption=-1;//controlador de cursor
    ssave=0; //guardado
    ssetup=0;//edicion
    if(screen>0) btn_enter=screen;
}

void renderError(byte type){
  in_time_error=0;
  error[1]=type;
  Serial.write(error,4);
  lcd.setCursor(0,0);
  switch(type){
    case 0://sistema parado
      apagarSalidas();
      if(btn_enter<=0){
        lcd.print("Sistema parado");
        lcd.setCursor(0,1);
        lcd.print("fin Incubacion");
      }
    break;
    case 1://onready
      in_timeSenData++;
      renderDateinit();
      apagarSalidas();
      if(btn_enter<=0){
        lcd.print("Esperando fecha");
        lcd.setCursor(0,1);
        lcd.print("de inicio");
      }
    break;
    case 2://periodo
      apagarSalidas();
      if(btn_enter<=0){
        lcd.print("Terminal sin");
        lcd.setCursor(0,1);
        lcd.print("Formato");
      }
    break;
    case 3://relog
     if(btn_enter<=0){
      lcd.print("Falta");
      lcd.setCursor(0,1);
      lcd.print("Modulo reloj");
     }
    break;
  }
}

void sys_update_incubadora(long u_dateinit,byte u_daysincu,float u_temp,byte u_hum,byte u_interval,byte u_rotation,byte u_periodo){
  if(u_dateinit>0)  EEPROM.put(POS_DATEINIT,u_dateinit);
  if(u_temp>0)      EEPROM.put(POS_TEMP,u_temp);
  if(u_periodo>0)   EEPROM.update(POS_PERIODO,u_periodo);
  if(u_daysincu>0)  EEPROM.update(POS_DAYSINCU,u_daysincu);
  if(u_hum>0)       EEPROM.update(POS_HUM,u_hum);
  if(u_interval>0)  EEPROM.update(POS_INTERVAL,u_interval);
  if(u_rotation >0) EEPROM.update(POS_WHILE,u_rotation);
}

void sys_update_nacedora(byte u_nacedora,byte u_dayinit,byte u_volteo,float u_ntemp, byte u_nhum){
      if(u_nacedora!= 255) EEPROM.update(POS_NACEDORA,sys_nacedora);
      if(u_dayinit>0)  EEPROM.update(POS_DAYINIT,u_dayinit);
      if(u_volteo!=255) EEPROM.update(POS_VOLTEO,u_volteo);
      if(u_ntemp>0)     EEPROM.put(POS_NTEMP,u_ntemp);
      if(u_nhum>0)      EEPROM.update(POS_NHUM,u_nhum);
      if(u_nacedora==0){
          EEPROM.get(POS_TEMP,sys_temp);
          sys_hum = EEPROM.read(POS_HUM);
      }
}

void sys_update_calibracion(byte type, bool mode,byte val_a, float val_b, byte hourMinut){
      switch(type){
        case 0:
          if(hourMinut>0)EEPROM.update(POS_CALIB_TUNIXTIME, (hourMinut==1 ? 0 : 1) );
          EEPROM.update(POS_CALIB_UNIXTIME,val_a);
          EEPROM.update(POS_CALIB_TIME_MOD,mode);
        break;
        case 1:
          EEPROM.put(POS_CALIB_TEMP,val_b);
          EEPROM.update(POS_CALIB_TEMP_MOD,mode);
        break;
        case 2:
          EEPROM.update(POS_CALIB_HUM,0);
          EEPROM.update(POS_CALIB_HUM_MOD,mode);  
        break;
      }
}

void toneSalve(){
    tone(pinBuzzer,1000);   
    delay(300); 
    tone(pinBuzzer,2000);
    delay(100); 
    noTone(pinBuzzer);  
    delay(300);
    tone(pinBuzzer,2000);
    delay(300); 
    tone(pinBuzzer,1000);
    delay(100); 
    noTone(pinBuzzer);   
}
