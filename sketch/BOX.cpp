#include "BOX.h"

BOX::BOX(byte pinSound, byte pinRED, byte pinGREEN, byte pinBLUE, int microSensitivity, int microGain, int threshold, long startBox, int analogValue, int boxStatus, double boxDB, double boxDBSPL) {
  this->pinSound = pinSound;
  this->pinRED = pinRED;
  this->pinGREEN = pinGREEN;
  this->pinBLUE = pinBLUE;
  this->microSensitivity = microSensitivity;
  this->microGain = microGain;
  this->threshold = threshold;
  this->startBox = startBox;
  this->analogValue = analogValue;
  this->boxStatus = boxStatus;
  this->boxDB = boxDB;
  this->boxDBSPL = boxDBSPL;
  init();
}

void BOX::init() {
  pinMode(pinSound, INPUT);
  pinMode(pinRED, OUTPUT);
  pinMode(pinGREEN, OUTPUT);
  pinMode(pinBLUE, OUTPUT);
  int prevStatusBox;
}

void BOX::white(int l_delay) {
  digitalWrite(pinRED, 255);
  digitalWrite(pinGREEN, 255);
  digitalWrite(pinBLUE, 255);
  delay(l_delay);
}

void BOX::green(int l_delay) {
  digitalWrite(pinRED, 0);
  digitalWrite(pinGREEN, 255);
  digitalWrite(pinBLUE, 0);
  delay(l_delay);
}

void BOX::ambar(int l_delay) {
  digitalWrite(pinRED, 255);
  digitalWrite(pinGREEN, 127);
  digitalWrite(pinBLUE, 0);
  delay(l_delay);
}

void BOX::red(int l_delay) {
  digitalWrite(pinRED, 255);
  digitalWrite(pinGREEN, 0);
  digitalWrite(pinBLUE, 0);
  delay(l_delay);
}

void BOX::redAP(int l_delay) {
  for(int i = 0;i<3;i++){
    red(100);
    off(100);
  }
}

void BOX::delic(int l_delay) {
  red(1000);
  green(1000);
  ambar(1000);
  white(1000);
  red(1000);
  delay(l_delay);
}

void BOX::off(int l_delay) {
  digitalWrite(pinRED, 0);
  digitalWrite(pinGREEN, 0);
  digitalWrite(pinBLUE, 0);
  delay(l_delay);
}

int BOX::getStatus() {
  
   unsigned long startMillis= millis();

   unsigned int signalMax = 0;
   unsigned int signalMin = 1024;
 
   // Recopilar durante la ventana
   unsigned int sample;
   
   while (millis() - startMillis < 100)
   {
      sample = analogRead(pinSound);
      
      if (sample < 1024)
      {
         if (sample > signalMax)
         {
            signalMax = sample;  // Actualizar máximo
         }
         else if (sample < signalMin)
         {
            signalMin = sample;  // Actualizar mínimo
         }
      }
   }
   
   unsigned int peakToPeak = signalMax - signalMin;  // Amplitud del sonido
   
   double volts = (peakToPeak * 3.3) / 1024;  // Convertir a tensión

   double gain_db = (1000000*pow(10,this->microSensitivity/20)/1000000);
   
   double volts_db = 20*log10(volts/gain_db); // 0.007943 viene calculado por la sensibilidad (ganancia( del micrófono Gain in decibel (+dB) Loss in decibel (−dB) MAX9814 = -42dB(+-3dB)

   double spl_db = volts_db + 94 + this->microSensitivity - this->microGain; //  94 is a 1 Pa expressed as dB SPL

   int level = map(volts_db, 20, 50, 0, 3);
   
   this->analogValue = sample;
   this->boxDB = volts_db;
   this->boxDBSPL = spl_db;
   
   analogValue = sample;
   
   return level;
}

int BOX::getStatusValue() {
  return this->boxStatus;
}

int BOX::getAnalog() {
  return this->analogValue;;
}

double BOX::getVoltage(){
  return this->analogValue   *  (3.3 / 1024.0); 
}

double BOX::getDB(){
  return this->boxDB;
}
double BOX::getDBSPL(){
  return this->boxDBSPL;
}
int BOX::prevStatus(){
  
}

int BOX::getInitTime(){
  return startBox;
}


void BOX::wakeUP() {

  int level = getStatus();
  
  if(level==0){
    boxStatus = 0;
    white(100);
    }else if(level==1){
      boxStatus = 1;
      green(100);
   }else if(level==2){
    boxStatus = 2;
    ambar(100);
   }else if(level==3){
    boxStatus = 3;
    red(100);
   }else{
    boxStatus = 5;
    white();
  }

}

String BOX::getBoxMode(String boxModeConf){
  if(boxModeConf=="n"){
    return "night";
  }else if(boxModeConf=="d"){
    return "day";
  }else if(boxModeConf=="f"){
    return "full";
  }else if(boxModeConf=="t"){
    return "test";
  }
}

void BOX::runOFF() {
  for(int i = 0;i<3;i++){
      white(200);
      off(200);
  }
  white(200); 
}

void BOX::runUP() {
  
  int level = getStatus();
  
  if(level==0){
    boxStatus = 0;
    white(100);
    }else if(level==1){
      boxStatus = 1;
      green(100);
   }else if(level==2){
    boxStatus = 2;
    ambar(100);
   }else if(level==3){
    boxStatus = 3;
    red(100);
   }else{
    boxStatus = 5;
    white();
  }
}
