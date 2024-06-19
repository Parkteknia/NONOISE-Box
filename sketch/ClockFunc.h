#include <TZ.h>

#define MYTZ TZ_Europe_Madrid

RTC_DS3231 rtcClock;

void printSpecialTime(DateTime date);
void printTime();

void startBoxClock() {

  if (! rtcClock.begin()) { 
   Serial.println("Modulo RTC no encontrado !");
   while (1);         
   }
   
   Serial.print("Retrieving time: ");
  configTime(MYTZ, "pool.ntp.org"); // get UTC time via NTP
  
  time_t now = time(nullptr);
  while (now < 24 * 3600)
  {
    Serial.print(".");
    delay(100);
    now = time(nullptr);
  }
  
  Serial.print("TIME NOW: ");
  Serial.println(now);
  
  rtcClock.adjust(DateTime(__DATE__, __TIME__));
  //DateTime checkDate = rtcClock.now();
  boxClock = rtcClock.now();
  /*
   if(checkDate.year() < 2022){
    Serial.println("Wrong RTC clock date !");
     
   }
   */
   Serial.println(boxClock.hour());
   printTime();
}   

bool checkNightTime(DateTime nowtime){

  DateTime nextdate (nowtime.year(), nowtime.month(), nowtime.day(), 22, 30, 0);
  DateTime nextend (nowtime.year(), nowtime.month(), nowtime.day()+1, 8, 0 , 0);
  
  if(nowtime.unixtime() >= nextdate.unixtime() && nowtime.unixtime() <= nextend.unixtime()){
    return true;
  }
  return true;
}
bool checkDayTime(DateTime nowtime){

  DateTime nextdate (nowtime.year(), nowtime.month(), nowtime.day(), 8, 0, 0);
  DateTime nextend (nowtime.year(), nowtime.month(), nowtime.day(), 22, 30 , 0);
  
  if(nowtime.unixtime() >= nextdate.unixtime() && nowtime.unixtime() <= nextend.unixtime()){
    return true;
  }
  return false;
}
void printSpecialTime(DateTime date){
  Serial.print(date.day());     // funcion que obtiene el dia de la fecha completa
  Serial.print("/");       // caracter barra como separador
  Serial.print(date.month());     // funcion que obtiene el mes de la fecha completa
  Serial.print("/");       // caracter barra como separador
  Serial.print(date.year());      // funcion que obtiene el año de la fecha completa
  Serial.print(" ");       // caracter espacio en blanco como separador
  Serial.print(date.hour());      // funcion que obtiene la hora de la fecha completa
  Serial.print(":");       // caracter dos puntos como separador
  Serial.print(date.minute());      // funcion que obtiene los minutos de la fecha completa
  Serial.print(":");       // caracter dos puntos como separador
  Serial.println(date.second());
}
void printTime(){
  Serial.print(boxClock.day());     // funcion que obtiene el dia de la fecha completa
  Serial.print("/");       // caracter barra como separador
  Serial.print(boxClock.month());     // funcion que obtiene el mes de la fecha completa
  Serial.print("/");       // caracter barra como separador
  Serial.print(boxClock.year());      // funcion que obtiene el año de la fecha completa
  Serial.print(" ");       // caracter espacio en blanco como separador
  Serial.print(boxClock.hour());      // funcion que obtiene la hora de la fecha completa
  Serial.print(":");       // caracter dos puntos como separador
  Serial.print(boxClock.minute());      // funcion que obtiene los minutos de la fecha completa
  Serial.print(":");       // caracter dos puntos como separador
  Serial.println(boxClock.second());    // funcion que obtiene los segundos de la fecha completa
}

bool runTime(){
  
  //DateTime datebox = rtcClock.now();

  //boxClock = datebox;
  
  if(boxModeConf=="n"){
      bool timeToRun = checkNightTime(boxClock);
      return timeToRun;
  }else if(boxModeConf=="d"){
      bool timeToRun = checkDayTime(boxClock);
      //Serial.print("Time to run: ");
      //Serial.println(timeToRun);
      return timeToRun;
  }else if(boxModeConf=="f"){
      bool timeToRun = checkDayTime(boxClock);
      //Serial.print("Time to run: ");
      //Serial.println(timeToRun);
      return timeToRun;
  }else if(boxModeConf == "t"){
    return true;
  }
}
