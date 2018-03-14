

/*
New AC Remote
aroon25@yahoo.com
arun
19/jun/2015



*/

/*
   Thermistor resistance 9.70 K  ______ depricated_____


   use of Dallas one wire temp sensor digial DS 18S20 one wire temSensor
   1--+++++++ -  GND  
    2--+    + -  DATA
   3--+++++++ -  VDD


*/

#include <EEPROM.h>
#include <IRremote.h>
#include <Wire.h>
#include <Stepper.h>
#include <IRremoteInt.h>
#include <math.h>
#include <LedControl.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Arduino.h>
//#include <acUnit.h>


#define STEPS 100
#define tempPIN A0
#define pipePIN A1
#define acPIN 3
#define fanPIN1 4
#define fanPIN2 5
#define fanPIN3 6
#define buzzPIN 0
#define delayTime 3; // initial delaytime
#define MAX_DS1820_SENSORS 2
#define ONE_WIRE_BUS 9
#define TEMPERATURE_PRECISION 9



int receivepin = 1;
int codeType = -1;
int epMEM =10;
int irValue;

int c;
int AC;
int ODU;
//int acPIN; // ac pin same as ODU
int oduPIN;

int oduDELAY=3; // outdoor delay time
int remotePIN =1;
float tempC;
float tempR;

int fanSPEED=0; //0,1,2,3, auto =4
int setTEMP =10;
float pipeTemp=0;
float pipeRT=2.5;
int pipeFtime=9;
int pipeTime=0;
int dTIME=18;       // 10sec * 3 =180 sec or 3min
int rTIME=0;        // runTIME

int SW=1;           // SWing 0 or 1 or 2
int SWPin=10;
int PARK=0;
int PARKpin=11;    // louver park pin
int acSTATE=0;      // 0 off 1 on
int pcount=0;
int tcount=0;
int acMODE=1;      // 1=cool 2=fan 3=heat 4=dehumid 5=recirc

//int buzzPIN=13;

float vcc = 5.00;                       // only used for display purposes, if used
                                        // set to the measured Vcc.
float pad = 9700;                       // balance/pad resistor value, set this to
                                        // the measured resistance of your pad resistor
float thermr = 10000;                   // thermistor nominal resistance

/* display settings 
  pin 12 is connected to the DataIn 
  pin 11 is connected to the CLK 
  pin 10 is connected to LOAD */

  int disData=12;
  int disClock=11;
  int disCS=10;
    

  LedControl lc=LedControl(disData,disClock,disCS,1);  // (pin, pin,pin, int_no_of_display)


IRrecv irrecv(receivepin);
decode_results results;

//acUnit myAC(0,0,0,0);

// tempSensor 

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DeviceAddress rtTempArray, ptTempArray;

// sensors.getAddress(deviceAddress, index)
// DeviceAddress insideThermometer = { 0x28, 0x1D, 0x39, 0x31, 0x2, 0x0, 0x0, 0xF0 };
// DeviceAddress outsideThermometer   = { 0x28, 0x3F, 0x1C, 0x31, 0x2, 0x0, 0x0, 0x2 };



void setup() {
  // put your setup code here, to run once:

   pinMode(buzzPIN,OUTPUT);
   analogReference(INTERNAL);
   Serial.begin(9600);
   irrecv.enableIRIn(); // Start the receiver


   Wire.begin(); // join i2c bus (address optional for master)
  
//  tempSensors initialize
sensors.begin();
sensors.setResolution(rtTempArray, TEMPERATURE_PRECISION);
sensors.setResolution( ptTempArray, TEMPERATURE_PRECISION);

cli();//stop interrupts
//set timer1 interrupt at 1Hz
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCNT1  = 0;//initialize counter value to 0
  // set compare match register for 1hz increments
  OCR1A = 15624;// = (16*10^6) / (1*1024) - 1 (must be <65536) OR 15624
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS10 and CS12 bits for 1024 prescaler
  TCCR1B |= (1 << CS12) | (1 << CS10);
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);
sei();//allow interrupts




 reEP(); // ac STATE read

 acSET(acSTATE,setTEMP,fanSPEED,SW,acMODE);// set acSET


}

void loop() {
  // put your main code here, to run repeatedly:
if (irrecv.decode(&results)) {
    Serial.println(results.value, HEX);
    irValue = results.value;

    irrecv.resume(); // Receive the next value
}
}// loop


void acSET(int _state, int _setTEMP, int _fanspeed, int _swing, int _acMode ) {




    dTIME=18; //set Delay time to 3 min

    wrEP(); // write in eeprom
 //

}
void reEP(){
    fanSPEED=EEPROM.read(epMEM);
    setTEMP=EEPROM.read(epMEM+1);
    SW=EEPROM.read(epMEM+2);
    acSTATE=EEPROM.read(epMEM+3);
}

void wrEP(){
    EEPROM.write(epMEM,fanSPEED);
    EEPROM.write((epMEM+1),setTEMP);
    EEPROM.write((epMEM+2),SW);
    EEPROM.write((epMEM+3),acSTATE);

}
void bz(){digitalWrite(buzzPIN,HIGH);delay(250);digitalWrite(buzzPIN,LOW);} // buzzer


void getTEMP(){
              tempR=int(Thermistor(analogRead(tempPIN)));
              // get pipesensor TEMP
              pipeTemp=Thermistor(analogRead(pipePIN));
              if(pipeTemp<=pipeRT) acSTATE=5;
              if(pipeTemp>9 && pipeTime >9 ) {pipeTime =0; acSTATE=1;}


return;
}
void proCODE(){
                switch(results.value){
                  //temp30
                  case 0x9070000E: { acSTATE=1; setTEMP=30;fanSPEED=4; SW=2; acMODE=1; break;}
                  case 0x9870000E: { acSTATE=1; setTEMP=30;fanSPEED=1; SW=2; acMODE=1; break;}
                  case 0x9470000E: { acSTATE=1; setTEMP=30;fanSPEED=2; SW=2; acMODE=1; break;}
                  case 0x9C70000E: { acSTATE=1; setTEMP=30;fanSPEED=3; SW=2; acMODE=1; break;}
                  case 0x8070000E: { acSTATE=0; setTEMP=30;fanSPEED=3; SW=2; acMODE=1; break;} // ac off

                   //temp29
                  case 0x90B0000E: { acSTATE=1; setTEMP=29;fanSPEED=4; SW=2; acMODE=1; break;} // ac on
                  case 0x98B0000E: { acSTATE=1; setTEMP=29;fanSPEED=1; SW=2; acMODE=1; break;}
                  case 0x94B0000E: { acSTATE=1; setTEMP=29;fanSPEED=2; SW=2; acMODE=1; break;}
                  case 0x9CB0000E: { acSTATE=1; setTEMP=29;fanSPEED=3; SW=2; acMODE=1; break;}
                  case 0x80B0000E: { acSTATE=0; setTEMP=29;fanSPEED=3; SW=2; acMODE=1; break;} // ac off

                  //temp28
                  case 0x9030000E: { acSTATE=1; setTEMP=28;fanSPEED=4; SW=2; acMODE=1; break;} // ac on
                  case 0x9830000E: { acSTATE=1; setTEMP=28;fanSPEED=1; SW=2; acMODE=1; break;}
                  case 0x9430000E: { acSTATE=1; setTEMP=28;fanSPEED=2; SW=2; acMODE=1; break;}
                  case 0x9C30000E: { acSTATE=1; setTEMP=28;fanSPEED=3; SW=2; acMODE=1; break;}
                  case 0x8030000E: { acSTATE=0; setTEMP=28;fanSPEED=3; SW=2; acMODE=1; break;} // ac off

                  //temp27
                  case 0x90D0000E: { acSTATE=1; setTEMP=27;fanSPEED=4; SW=2; acMODE=1; break;} // ac on
                  case 0x98D0000E: { acSTATE=1; setTEMP=27;fanSPEED=1; SW=2; acMODE=1; break;}
                  case 0x94D0000E: { acSTATE=1; setTEMP=27;fanSPEED=2; SW=2; acMODE=1; break;}
                  case 0x9CD0000E: { acSTATE=1; setTEMP=27;fanSPEED=3; SW=2; acMODE=1; break;}
                  case 0x80D0000E: { acSTATE=0; setTEMP=27;fanSPEED=3; SW=2; acMODE=1; break;} // ac off

                  //temp26
                  case 0x9050000E: { acSTATE=1; setTEMP=26;fanSPEED=4; SW=2; acMODE=1; break;} // ac on
                  case 0x9850000E: { acSTATE=1; setTEMP=26;fanSPEED=1; SW=2; acMODE=1; break;}
                  case 0x9450000E: { acSTATE=1; setTEMP=26;fanSPEED=2; SW=2; acMODE=1; break;}
                  case 0x9C50000E: { acSTATE=1; setTEMP=26;fanSPEED=3; SW=2; acMODE=1; break;}
                  case 0x8050000E: { acSTATE=0; setTEMP=26;fanSPEED=3; SW=2; acMODE=1; break;} // ac off

                  //temp25
                  case 0x9090000E: { acSTATE=1; setTEMP=25;fanSPEED=4; SW=2; acMODE=1; break;} // ac on
                  case 0x9890000E: { acSTATE=1; setTEMP=25;fanSPEED=1; SW=2; acMODE=1; break;}
                  case 0x9490000E: { acSTATE=1; setTEMP=25;fanSPEED=2; SW=2; acMODE=1; break;}
                  case 0x9C90000E: { acSTATE=1; setTEMP=25;fanSPEED=3; SW=2; acMODE=1; break;}
                  case 0x8090000E: { acSTATE=0; setTEMP=25;fanSPEED=3; SW=2; acMODE=1; break;} // ac off

                  //temp24
                  case 0x9010000E: { acSTATE=1; setTEMP=24;fanSPEED=4; SW=2; acMODE=1; break;} // ac on
                  case 0x9810000E: { acSTATE=1; setTEMP=24;fanSPEED=1; SW=2; acMODE=1; break;}
                  case 0x9410000E: { acSTATE=1; setTEMP=24;fanSPEED=2; SW=2; acMODE=1; break;}
                  case 0x9C10000E: { acSTATE=1; setTEMP=24;fanSPEED=3; SW=2; acMODE=1; break;}
                  case 0x8010000E: { acSTATE=0; setTEMP=24;fanSPEED=3; SW=2; acMODE=1; break;} // ac off

                   //temp23
                  case 0x90E0000E: { acSTATE=1; setTEMP=23;fanSPEED=4; SW=2; acMODE=1; break;} // ac on
                  case 0x98E0000E: { acSTATE=1; setTEMP=23;fanSPEED=1; SW=2; acMODE=1; break;}
                  case 0x94E0000E: { acSTATE=1; setTEMP=23;fanSPEED=2; SW=2; acMODE=1; break;}
                  case 0x9CE0000E: { acSTATE=1; setTEMP=23;fanSPEED=3; SW=2; acMODE=1; break;}
                  case 0x80E0000E: { acSTATE=0; setTEMP=23;fanSPEED=3; SW=2; acMODE=1; break;} // ac off

                   //temp22
                  case 0x9060000E: { acSTATE=1; setTEMP=22;fanSPEED=4; SW=2; acMODE=1; break;} // ac on
                  case 0x9860000E: { acSTATE=1; setTEMP=22;fanSPEED=1; SW=2; acMODE=1; break;}
                  case 0x9460000E: { acSTATE=1; setTEMP=22;fanSPEED=2; SW=2; acMODE=1; break;}
                  case 0x9C60000E: { acSTATE=1; setTEMP=22;fanSPEED=3; SW=2; acMODE=1; break;}
                  case 0x8060000E: { acSTATE=0; setTEMP=22;fanSPEED=3; SW=2; acMODE=1; break;} // ac off

                  //temp21
                  case 0x90A0000E: { acSTATE=1; setTEMP=21;fanSPEED=4; SW=2; acMODE=1; break;} // ac on
                  case 0x98A0000E: { acSTATE=1; setTEMP=21;fanSPEED=1; SW=2; acMODE=1; break;}
                  case 0x94A0000E: { acSTATE=1; setTEMP=21;fanSPEED=2; SW=2; acMODE=1; break;}
                  case 0x9CA0000E: { acSTATE=1; setTEMP=21;fanSPEED=3; SW=2; acMODE=1; break;}
                  case 0x80A0000E: { acSTATE=0; setTEMP=21;fanSPEED=3; SW=2; acMODE=1; break;} // ac off

                  //temp20
                  case 0x9020000E: { acSTATE=1; setTEMP=20;fanSPEED=4; SW=2; acMODE=1; break;} // ac on
                  case 0x9820000E: { acSTATE=1; setTEMP=20;fanSPEED=1; SW=2; acMODE=1; break;}
                  case 0x9420000E: { acSTATE=1; setTEMP=20;fanSPEED=2; SW=2; acMODE=1; break;}
                  case 0x9C20000E: { acSTATE=1; setTEMP=20;fanSPEED=3; SW=2; acMODE=1; break;}
                  case 0x8020000E: { acSTATE=0; setTEMP=20;fanSPEED=3; SW=2; acMODE=1; break;} // ac off

                  //temp19
                  case 0x90C0000E: { acSTATE=1; setTEMP=19;fanSPEED=4; SW=2; acMODE=1; break;} // ac on
                  case 0x98C0000E: { acSTATE=1; setTEMP=19;fanSPEED=1; SW=2; acMODE=1; break;}
                  case 0x94C0000E: { acSTATE=1; setTEMP=19;fanSPEED=2; SW=2; acMODE=1; break;}
                  case 0x9CC0000E: { acSTATE=1; setTEMP=19;fanSPEED=3; SW=2; acMODE=1; break;}
                  case 0x80C0000E: { acSTATE=0; setTEMP=19;fanSPEED=3; SW=2; acMODE=1; break;} // ac off

                  //temp18
                  case 0x9040000E: { acSTATE=1; setTEMP=18;fanSPEED=4; SW=2; acMODE=1; break;} // ac on
                  case 0x9840000E: { acSTATE=1; setTEMP=18;fanSPEED=1; SW=2; acMODE=1; break;}
                  case 0x9440000E: { acSTATE=1; setTEMP=18;fanSPEED=2; SW=2; acMODE=1; break;}
                  case 0x9C40000E: { acSTATE=1; setTEMP=18;fanSPEED=3; SW=2; acMODE=1; break;}
                  case 0x8040000E: { acSTATE=0; setTEMP=18;fanSPEED=3; SW=2; acMODE=1; break;} // ac off

                  //temp17
                  case 0x9080000E: { acSTATE=1; setTEMP=17;fanSPEED=4; SW=2; acMODE=1; break;} // ac on
                  case 0x9880000E: { acSTATE=1; setTEMP=17;fanSPEED=1; SW=2; acMODE=1; break;}
                  case 0x9480000E: { acSTATE=1; setTEMP=17;fanSPEED=2; SW=2; acMODE=1; break;}
                  case 0x9C80000E: { acSTATE=1; setTEMP=17;fanSPEED=3; SW=2; acMODE=1; break;}
                  case 0x8080000E: { acSTATE=0; setTEMP=17;fanSPEED=3; SW=2; acMODE=1; break;} // ac off


                  //temp16
                  case 0x9000000E: { acSTATE=1; setTEMP=16;fanSPEED=4; SW=2; acMODE=1; break;} // ac on
                  case 0x9800000E: { acSTATE=1; setTEMP=16;fanSPEED=1; SW=2; acMODE=1; break;}
                  case 0x9400000E: { acSTATE=1; setTEMP=16;fanSPEED=2; SW=2; acMODE=1; break;}
                  case 0x9C00000E: { acSTATE=1; setTEMP=16;fanSPEED=3; SW=2; acMODE=1; break;}
                  case 0x8000000E: { acSTATE=0; setTEMP=16;fanSPEED=3; SW=2; acMODE=1; break;} // ac off


/////////////////////////////////////// mode // 1=cool 2=fan 3=heat 4=dehumid 5=recirc
                  // fan mode
                  case 0xD810000E: {acSTATE=0; setTEMP=0; fanSPEED=1; SW=2; acMODE=2; break; }
                  case 0xD410000E: {acSTATE=0; setTEMP=0; fanSPEED=2; SW=2; acMODE=2; break; }
                  case 0xDC10000E: {acSTATE=0; setTEMP=0; fanSPEED=3; SW=2; acMODE=2; break; }
                  case 0xC810000E: {acSTATE=0; setTEMP=0; fanSPEED=3; SW=2; acMODE=0; break; }
                 /// de humid
                  case 0x5810000E: {acSTATE=0; setTEMP=0; fanSPEED=1; SW=2; acMODE=4; break; }
                  case 0x4810000E: {acSTATE=0; setTEMP=0; fanSPEED=1; SW=2; acMODE=0; break; }


                }// switch


                  acSET(acSTATE, setTEMP, fanSPEED,SW,acMODE);
                  bz();
                  Serial.print("AC State");Serial.println(acSTATE);
                  Serial.print("Set TEMP");Serial.println(setTEMP);
                  Serial.print("SWing");   Serial.println(fanSPEED);
                  Serial.print("AC MODE"); Serial.println(SW);
                  Serial.print("AC State");Serial.println(acMODE);


}

void controlTemp(){

      if(acSTATE==1){
      tempC=Thermistor(analogRead(tempPIN));
      if(tempC>=setTEMP){ODU=1;digitalWrite(oduPIN,1);}
      if(tempC<setTEMP) {ODU=0; digitalWrite(oduPIN,0);}

      }
}
ISR(TIMER1_COMPA_vect){//timer1 interrupt 1Hz toggles pin 13 (LED)
//generates pulse wave of frequency 1Hz/2 = 0.5kHz (takes two cycles for full wave- toggle high then toggle low)
  c++;
  if(c>9){ c=0; tenSEC();}

}

void fan(int fS){
  switch(fS){   /// order of switching important
     case'1': { digitalWrite(fanPIN2,LOW);
                digitalWrite(fanPIN3,LOW);
                digitalWrite(fanPIN1,HIGH);
              }
     case'2': {
                digitalWrite(fanPIN1,LOW);
                digitalWrite(fanPIN3,LOW);
                digitalWrite(fanPIN2,HIGH);
              }
     case'3': {
                digitalWrite(fanPIN1,LOW);
                digitalWrite(fanPIN2,LOW);
                digitalWrite(fanPIN3,HIGH);
              }
    break;
  }


}
void tenSEC(){
  int sec ;
  pipeTemp=Thermistor(analogRead(pipePIN));
  if(pipeTemp<=pipeRT) acSTATE=5;
  if(pipeTemp>5 && pipeTime >9 ) {pipeTime =0; acSTATE=1;}

  sec++;
  if(sec>5) {sec=0;minuteCount();}
  }

void minuteCount(){

  //fan();
  tempC=int(Thermistor(analogRead(tempPIN)));
  if(fanSPEED==4){
  if( tempC <=  setTEMP)     { fanSPEED=1;}
  if( tempC > ( setTEMP -2 )){ fanSPEED=2;}
  if( tempC > ( setTEMP -3 )){ fanSPEED=3;}
  }//fanSPEED auto
  fan(fanSPEED);


  if(acSTATE==5){pipeTime++;}
  tcount++;
  if(tcount>=dTIME) {tcount==0; controlTemp();}

  rTIME++;
  if(rTIME >=6) dTIME=6; // on minute delay


}


/// send data to display

void slave(int _sT, int _acSTATE, int _swing, int _time, int _fanSPEED ){

      Wire.write(_acSTATE);
      Wire.write(_swing);
      Wire.write(_time);
      Wire.write(_fanSPEED);

      Wire.endTransmission();    // stop transmitting

  }// slave



float Thermistor(int RawADC) {
  long Resistance;
  float Temp;  // Dual-Purpose variable to save space.

  Resistance=pad*((1024.0 / RawADC) - 1);
  Temp = log(Resistance); // Saving the Log(resistance) so not to calculate  it 4 times later
  Temp = 1 / (0.001129148 + (0.000234125 * Temp) + (0.0000000876741 * Temp * Temp * Temp));
  Temp = Temp - 273.15;  // Convert Kelvin to Celsius
  return Temp;                                      // Return the Temperature
}

// function to print a device address
void printAddress(DeviceAddress deviceAddress)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    // zero pad the address if necessary
    if (deviceAddress[i] < 16) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
  }
}

// function to print the temperature for a device
void printTemperature(DeviceAddress deviceAddress)
{
  float tempC = sensors.getTempC(deviceAddress);
  Serial.print("Temp C: ");
  Serial.print(tempC);
  Serial.print(" Temp F: ");
  Serial.print(DallasTemperature::toFahrenheit(tempC));
}

// function to print a device's resolution
void printResolution(DeviceAddress deviceAddress)
{
  Serial.print("Resolution: ");
  Serial.print(sensors.getResolution(deviceAddress));
  Serial.println();
}

// main function to print information about a device
void printData(DeviceAddress deviceAddress)
{
  Serial.print("Device Address: ");
  printAddress(deviceAddress);
  Serial.print(" ");
  printTemperature(deviceAddress);
  Serial.println();
}

//printData(insideThermometer);
//printData(outsideThermometer);