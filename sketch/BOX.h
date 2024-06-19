#ifndef BOX_H
#define BOX_H

#include <Arduino.h>


class BOX {

  private:

    byte pinSound;
    byte pinRED;
    byte pinGREEN;
    byte pinBLUE;
    int microSensitivity;
    int microGain;
    int threshold;
    long startBox;
    int analogValue;
    int boxStatus;
    double boxDB;
    double boxDBSPL;
        
  public:
    
    BOX(byte pinSound,byte pinRED, byte pinGREEN, byte pinBLUE, int microSensitivity, int microGain, int threshold, long startBox, int analogValue, int boxStatus, double boxDB, double boxDBSPL);    

    void init();

    void white(int delay_color = 0);

    void green(int delay_color = 0);

    void ambar(int delay_color = 0);

    void red(int delay_color = 0);

    void redAP(int delay_color = 0);

    void delic(int delay_color = 0);

    void off(int delay_color = 0);

    int getInitTime();
    
    int getStatus();

    int getStatusValue();

    int getAnalog();

    double getVoltage();

    double getDB();

    double getDBSPL();
    
    int prevStatus();

    void runOFF();
    
    void wakeUP();
    
    void runUP();

    String getBoxMode(String boxModeConf);
};

#endif
