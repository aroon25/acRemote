
#ifndef acUnit_h
#define acUnit_h
#include"Arduino.h"
#include"LedControl.h"

class acUnit{
  public :
   
    acUnit(int acState, int acsetTemp, int acFanspeed, int acSwing, int acacMode);

  private:
    float _roomT =40.0;
    float _pipeT;
    int _fanS=1;
    
    
    void setState(int _state, int _setTEMP, int _fanspeed, int _swing, int _acMode );
    void getState();  
    void readRTemp();
    void readPTemp();
    void setTemp(int _sT);
    void writeEprom();
    void readEprom();
    void setFan(int _FS);
    void setDisplay();

};
#endif