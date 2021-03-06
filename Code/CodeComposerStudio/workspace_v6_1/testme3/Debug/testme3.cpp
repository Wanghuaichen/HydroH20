#include "Energia.h"

double Thermistor(int RawADC);
void promptUser();
void readTemp();
int readTurb();
void readCond();
int caliTurb();
void setup();
void loop();

#line 1 "C:/Users/Chris/workspace_v6_1/testme3/testme3.ino"

#include <altSPI.h>
#include <math.h>
#include <string.h>


int read_cond = A11;

int pulse = PE_5;

int read_turb = A10;

int led_out = PB_2;

int read_temp = A3;

int refVolt = PF_0;


int tempVal = 0;

int turbVal = 0;

int condVal = 0;

String tempString = "Temperature Value is: ";
String actTempFString = "Temperature (F) is: ";
String actTempCString = "Temperature (C) is: ";
String condString = "Conductivity Value is: ";
String turbString = "Turbidity Value (ADC) is: ";

struct config_t {
  
  float y0, m0, b0;
  float y1, m1, b1;
  float y2, m2, b2;
  float y3, m3, b3;
  float y4, m4, b4;
}
config;

double Thermistor(int RawADC) {
  double Temp;
  Temp = log(10000.0*((4096.0/RawADC-1)));
  Temp = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * Temp * Temp))* Temp);
  Temp = Temp - 273.15;            
  return Temp;
}

void promptUser() {
  Serial.print("--------------\n\rH20Spy v.0.0.1\n\r--------------\n\r");
  Serial.print("What would you like to do?\n\r\r\n1. Read Temperature\n\r2. Read Conductivity\n\r3. Read Turbidity\n\r4. Calibrate Turbidity\n\r");
  int mode = Serial.parseInt();
  switch (mode) {
  case 1:
    readTemp();
    break;
  case 2:
    readCond();
    break;
  case 3:
    readTurb();
    break;
  case 4:
    caliTurb();
    break;
  default:
    break;
  }
}

void readTemp() {
  tempVal=0;
  for(int i = 0; i < 1000; i++) {
    tempVal += analogRead(read_temp);
  }
  tempVal = tempVal / 1000;
  String tempOut = tempString + tempVal;
  
  
  double actTemp = Thermistor(tempVal);
  Serial.println(actTempCString + actTemp);
  Serial.println(actTempFString + ((actTemp * (9.0/5.0)) + 32.0));
}

int readTurb() {
  
  digitalWrite(led_out,HIGH);

  
  turbVal = 0;
  for (int a = 0; a < 750000; a++){
    turbVal += analogRead(read_turb);
  }
  
  turbVal = turbVal / 750000;
  String turbOut = turbString + turbVal;
  Serial.println(turbOut);
  

  return turbVal;
}

void readCond() {
  double condValLow = 0;
  double condValHigh = 0;

  
  


  for(int i = 0; i < 100; i++) {
    digitalWrite(pulse,HIGH);
    condValHigh += analogRead(read_cond);
    delay(5);
    digitalWrite(pulse,LOW);
    delay(5);
    condValLow += analogRead(read_cond);
  }

  condValHigh = condValHigh/1000;
  condValLow = condValLow/1000;

  String condValLowStr = "Low conductivity Value is: ";
  String condValHighStr = "High conductivity Value is: ";
  String condOut = condString + condVal;

  String condLowOut = condValLowStr + condValLow;
  String condHighOut = condValHighStr + condValHigh;

  Serial.println(condOut);
  Serial.println(condLowOut);
  Serial.println(condHighOut);
}

int caliTurb() {
  int stdArry[] = {
    10, 20, 100, 800  };
  int y[4];
  int m[5];
  char respArry[3];
  respArry[2]='\0';

  for(int i = 0; i < 4; i++) {
    char promptBuff[65];
    sprintf(promptBuff,"Please place %d NTU Standard into holder then type \"ok\": ", stdArry[i]);
    Serial.println(promptBuff);
    do {
      Serial.readBytesUntil(10,respArry, 2);
      respArry[2]='\0';
    }
    while (strcmp(respArry, "ok"));

    if(!strcmp(respArry,"ok")) {
      Serial.println("Calibrating...");
      y[i] = readTurb();
      

    }
    else {
      Serial.println("Please enter \"ok\"");
    }
    Serial.flush();
    memset(&respArry,0,3);
  }

  for(int l = 0; l < 4; l++) {
    char promptBuff3[20];
    sprintf(promptBuff3,"y[%d] = %d ", l, y[l]);
    Serial.println(promptBuff3);
  }
  m[0] = (y[1] - y[0]) / (stdArry[1] - stdArry[0]);
  for(int i = 2; i < 4; i = i+2){
    y[i] = m[i/2-1]*(stdArry[i]-stdArry[i-1]) + y[i-1];
    m[i/2] = (y[i] - y[i-1]) / (stdArry[i] - stdArry[i-1]);
  }
  for(int j = 0; j < 4; j++) {
    char promptBuff2[20];
    sprintf(promptBuff2,"m[%d] = %d ", j, m[j]);
    Serial.println(promptBuff2);
  }
  for(int k = 0; k < 4; k++) {
    char promptBuff3[20];
    sprintf(promptBuff3,"y[%d] = %d ", k, y[k]);
    Serial.println(promptBuff3);
  }

  return 0;
}
void setup()
{
  pinMode(read_cond, INPUT);
  pinMode(pulse, OUTPUT);
  pinMode(read_turb, INPUT);
  pinMode(led_out, OUTPUT);
  pinMode(read_temp, INPUT);
  pinMode(refVolt, OUTPUT);

  
  
  
  analogWrite(refVolt,128);

  Serial.begin(9600);
}

void loop() {
  if(Serial.available() > 0) {
    promptUser();
  }
}



