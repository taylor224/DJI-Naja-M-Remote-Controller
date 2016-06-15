// Taylor Starfield  -  http://github.com/taylor224
//
// DJI NAZA-M Controller Control System
// License : MIT
//
//
// Serial Input  >1111:1111:1111:1111:1/
// >AILE:ELEV:THRO:RUDD:MODE
//
// Description about this code - http://drone.festasquare.com/devinfo/146


#include <Servo.h>

#define SIGNAL_NUMBER 8

int signalPins[SIGNAL_NUMBER] = {2, 3, 4, 5, 6, 7, 8, 9};

int scount = 0;
int icount = 0;
int sproc[100];
char sinput[300];

boolean first = true;

int startcount = 0;

float AILE = 90;
float ELEV = 90;
float THRO = 45; // 45~140 // 45~92.5~140 (Total 95 step)
float RUDD = 90;
float MODE = 127; // 62 - 94 - 127
float AUX1 = 127;
float AUX2 = 90;

int led = 13;

bool isInputted = false;
bool isFullInputted = false;

Servo PIN[SIGNAL_NUMBER];

void setup()
{
  Serial.begin(115200);
  pinMode(led, OUTPUT);
  digitalWrite(led, LOW);
}

void loop()
{
 Reader();
}

float SignalCalc(int input) // Signal Value Calculator  INPUT = "1111:1111:1111:1111:1"
{
  float inp = input;
  float output = (((inp/1000.0)*(140.0-45.0))+45.0);
  return output;
}

void Reader()
{
 char *p;
 char ch = Serial.read();
 
 if(ch == '>' && startcount < 1)
 {
   for(int i=0; i<SIGNAL_NUMBER; i++)
     PIN[i].attach(signalPins[i]);
     
   startcount++;
   digitalWrite(led, HIGH);
 }
   
 if((int)ch >= 47) // if ch != -1
 {
   if (first) // prevent error when system send null data to serial
   {
     if (ch != '>')
     {
       return;
     }
     else if (ch == '>')
     {
       first = false; 
     }
   }
   
   if(ch != '/' && ch != '>') // inputting, checking eof char '/'
   {
     sinput[scount] = ch;
     scount++;
   }
   else if(ch == '/') // make inputted
   {
     sinput[scount] = 0;
     isInputted = true;
     ch = '-1'; // For safe make -1
   }
 }
 
 if(isInputted == true)
 {
   p = strtok(sinput, ":");
   while(p!=NULL)
   {
     sproc[icount] = atoi(p);
     p = strtok(NULL, ":");
     icount++;
   }
   
   if(p == NULL)
   {
     isFullInputted = true;
     isInputted = false;
     scount = 0;
     icount = 0;
     first = true;
   }
 }

 if(isFullInputted == true)
 {
   int modeint;

   AILE = SignalCalc(sproc[0]);
   ELEV = SignalCalc(sproc[1]);
   THRO = SignalCalc(sproc[2]);
   RUDD = SignalCalc(sproc[3]);
   
   modeint = sproc[4];
   if(modeint == 1) // GPS
     MODE = 127;
   else if(modeint == 2) // Attitude
     MODE = 94;
   else if(modeint == 3) // Manual
     MODE = 62;
   
   sinput[0] = '\0'; // input string reset
   for(int i=0; i<6; i++)
   {
     sproc[i] = 0; 
   }
   scount = 0; // input counter reset
   isFullInputted = false; // make system to read input again
 }
 
   PIN[0].write(AILE);
   PIN[1].write(ELEV);
   PIN[2].write(THRO);
   PIN[3].write(RUDD);
   PIN[4].write(MODE);
   PIN[5].write(AUX1);
   PIN[6].write(AUX2);
 
}
