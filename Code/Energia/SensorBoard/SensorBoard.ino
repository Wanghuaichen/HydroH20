
#include <altSPI.h>
#include <math.h>
#include <string.h>

//Pin 2
int read_cond = A11;
//Pin 6
int pulse = PE_5;
//Pin 7 
int read_turb = A10;
//Pin 19
int led_out = PB_2;
//pin 18
int read_temp = A3;
//pin 17
int refVolt = PF_0;

//Holds current temperature values
int tempVal = 0;
//Holds current turbidity values
float turbVal = 0;
//Holds current conductivity values
int condVal = 0;

String tempString = "Temperature Value is: ";
String actTempFString = "Temperature (F) is: ";
String actTempCString = "Temperature (C) is: ";
String condString = "Conductivity Value is: ";
String turbADCString = "Turbidity Value (ADC) is: ";
String turbNTUString = "Turbidity Value (NTU) is: ";

struct conf_t {
  //Hold calibration values
  float x0, x1, x2, x3, x4,x5,x6,x7,x8,x9,x10;
  float y0, m0, b0;
  float y1, m1, b1;
  float y2, m2, b2;
  float y3, m3, b3;
  float y4, m4, b4;
  float y5, m5, b5;
  float y6, m6, b6;
  float y7, m7, b7;
  float y8, m8, b8;
  float y9, m9, b9;
  float y10, m10, b10;
}
conf;
void printStruct() {
  char structBuff[50];
  sprintf(structBuff,"%f NTU: y0: %f, m0: %f, b0: %f", conf.x0, conf.y0, conf.m0, conf.b0);
  Serial.println(structBuff);
  sprintf(structBuff,"%f NTU: y1: %f, m1: %f, b1: %f", conf.x1, conf.y1, conf.m1, conf.b1);
  Serial.println(structBuff);
  sprintf(structBuff,"%f NTU: y2: %f, m2: %f, b2: %f", conf.x2, conf.y2, conf.m2, conf.b2);
  Serial.println(structBuff);
  sprintf(structBuff,"%f NTU: y3: %f, m3: %f, b3: %f", conf.x3, conf.y3, conf.m3, conf.b3);
  Serial.println(structBuff);
  sprintf(structBuff,"%f NTU: y4: %f, m4: %f, b4: %f", conf.x4, conf.y4, conf.m4, conf.b4);
  Serial.println(structBuff);
  sprintf(structBuff,"%f NTU: y5: %f, m5: %f, b5: %f", conf.x5, conf.y5, conf.m5, conf.b5);
  Serial.println(structBuff);
  sprintf(structBuff,"%f NTU: y6: %f, m6: %f, b6: %f", conf.x6, conf.y6, conf.m6, conf.b6);
  Serial.println(structBuff);
  sprintf(structBuff,"%f NTU: y7: %f, m7: %f, b7: %f", conf.x7, conf.y7, conf.m7, conf.b7);
  Serial.println(structBuff);
  sprintf(structBuff,"%f NTU: y8: %f, m8: %f, b8: %f", conf.x8, conf.y8, conf.m8, conf.b8);
  Serial.println(structBuff);
  sprintf(structBuff,"%f NTU: y9: %f, m9: %f, b9: %f", conf.x9, conf.y9, conf.m9, conf.b9);
  Serial.println(structBuff);
  sprintf(structBuff,"%f NTU: y10: %f, m10: %f, b10: %f", conf.x10, conf.y10, conf.m10, conf.b10);
  Serial.println(structBuff);
  sprintf(structBuff,"Finished printing calibration data");
  Serial.println(structBuff);
}

double Thermistor(int RawADC) {
  double Temp;
  Temp = log(10000.0*((4096.0/RawADC-1))); 
  Temp = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * Temp * Temp))* Temp);
  Temp = Temp - 273.15;            // Convert Kelvin to Celcius
  return Temp;
}

void promptUser() {
  if(Serial.available() > 0) {
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
      readTurbNTU();
      break;
    case 4:
      caliTurb();
      Serial.println("Turbidity configuration:");
      printStruct();
      break;
    default:
      int a = 1;
      a++;
      //break;
    }
  }
}

void readTemp() {
  tempVal=0;
  for(int i = 0; i < 1000; i++) {
    tempVal += analogRead(read_temp);
  }
  tempVal = tempVal / 1000;
  String tempOut = tempString + tempVal;
  //Serial.println(tempOut);
  //Print celcius/Fahrenheit conversions
  double actTemp = Thermistor(tempVal);
  Serial.println(actTempCString + actTemp);
  Serial.println(actTempFString + ((actTemp * (9.0/5.0)) + 32.0)); 
}

float readTurbADC() {
  //Read and print turbidity
  digitalWrite(led_out,HIGH);
  int highVal = 0;
  int lowVal = 1000000;
  //Clear up noise, add values of 750000 scans (approx 5sec) and average
  turbVal = 0;
  for (int a = 0; a < 750000; a++){
    float readVal = analogRead(read_turb);
    if(readVal > highVal) {
      highVal = turbVal;
    } 
    if(turbVal < lowVal) {
      lowVal = readVal;
    }
    turbVal += readVal;
  }
  //Remove outliers
  turbVal -= (highVal + lowVal);
  //Get average value
  turbVal = turbVal / 749998;
  String turbOut = turbADCString + turbVal;
  Serial.println(turbOut);
  //digitalWrite(led_out,LOW);


  return turbVal;
}

float readTurbNTU() {
  float ntuVal = -9999;
  float reading = readTurbADC();
  char structBuff[50];
  sprintf(structBuff,"Raw reading is: %f", reading);
  Serial.println(structBuff);
  //Convert to NTU
  if(reading > conf.y1)       {
    ntuVal = reading * conf.m1 + conf.b1;
  }   
  else if(reading > conf.y2)  {
    ntuVal = reading * conf.m2 + conf.b2;
  }
  else if(reading > conf.y3)  {
    ntuVal = reading * conf.m3 + conf.b3;
  }
  else if(reading > conf.y4)  {
    ntuVal = reading * conf.m4 + conf.b4;
  }  
  else if(reading > conf.y5)  {
    ntuVal = reading * conf.m5 + conf.b5;
  }  
  else if(reading > conf.y6)  {
    ntuVal = reading * conf.m6 + conf.b6;
  }  
  else if(reading > conf.y7)  {
    ntuVal = reading * conf.m7 + conf.b7;
  }  
  else if(reading > conf.y8)  {
    ntuVal = reading * conf.m8 + conf.b8;
  }  
  else if(reading > conf.y9)  {
    ntuVal = reading * conf.m9 + conf.b9;
  }  
  else if(reading > conf.y10)  {
    ntuVal = reading * conf.m10 + conf.b10;
  }  
  else                            {
    ntuVal = reading * conf.m0 + conf.b0;
  }

  String turbNTU = turbNTUString + ntuVal;
  Serial.println(turbNTU);

  return ntuVal;
}

void readCond() {
  double condValLow = 0;
  double condValHigh = 0;

  //Read and print conductivity
  /**
   * delay of 5x2 = 10 microseconds, x 100 = 1 second delay to read.
   */
  for(int i = 0; i < 16; i++) {
    digitalWrite(pulse,HIGH);
    condValHigh += analogRead(read_cond);
    delay(1);
    digitalWrite(pulse,LOW);
    delay(1);
    condValHigh -= analogRead(read_cond);
  }
/*
  condValHigh = condValHigh/1000;
  condValLow = condValLow/1000;

  String condValLowStr = "Low conductivity Value is: ";
  String condValHighStr = "High conductivity Value is: ";
  */
  String condOut = condString + condValHigh;

 /* String condLowOut = condValLowStr + condValLow;
    String condHighOut = condValHighStr + condValHigh;

  Serial.println(condLowOut);
  Serial.println(condHighOut); */
  Serial.println(condOut);
}

int caliTurb() {
  float x[] = {0.51, 10, 13, 16.8, 18, 20, 23, 26,30,34.8,59,81,89,97.6,99,100,101,102,250,800, 1200}; //NTU standards (every 2x element) with additional values for interpolation
  //float x[] = {0.52,10,15,20,25,29,35,50.6,58,66.9,81,100,105,109,109.5,110,118,126,500,800,1200};
  int sampleSize = sizeof(x)/sizeof(float);
  float y[sampleSize]; // Holds Y value
  int slopeSize = ((ceil(sampleSize/2)) + 1);
  float m[slopeSize]; // Holds slopes
  char respArry[3];
  respArry[2]='\0'; //Add null char to end of array

  //Buffer to hold prompt output
  char promptBuff[65];

  sprintf(promptBuff,"Please place %.2f NTU Standard into holder then type \"ok\": ", x[0]);
  Serial.println(promptBuff);
  //Wait for user to type in OK
  do {
    Serial.readBytesUntil(10,respArry, 2);
    respArry[2]='\0';
  } 
  while (strcmp(respArry, "ok"));

  //Retrieve first value
  if(!strcmp(respArry,"ok")) {
    Serial.println("Calibrating...");
    y[0] = readTurbADC();
  } 
  
  //Clear buffer
  Serial.flush();
  memset(&respArry,0,3); 

  int iOld = 0;
  //Prompt-Collect rest of valuese
  for(int i = 1; i < sampleSize ; i=i+2) {
    if(i != 1) {
      iOld = i-1;  
    }
    char promptBuff[65];
    sprintf(promptBuff,"Please place %.2f NTU Standard into holder then type \"ok\": ", x[i]);
    Serial.println(promptBuff);
    do {
      Serial.readBytesUntil(10,respArry, 2);
      respArry[2]='\0';
    } 
    while (strcmp(respArry, "ok"));

    if(!strcmp(respArry,"ok")) {
      Serial.println("Calibrating...");
      do {
        y[i] = readTurbADC();
        Serial.println("verifying...");
        sprintf(promptBuff,"y[%d] is %f, y[%d] is %f", i, y[(i-2)], (i-2), y[i]);
        Serial.println(promptBuff);
      } while(y[(i-2)] < y[i]);
    } 
    else {
      Serial.println("Please enter \"ok\"");
    }
    Serial.flush();
    memset(&respArry,0,3); 
  }
  for(int i = 0; i < 20; i++) {
    sprintf(promptBuff,"y[%d] = %f",i, y[i]);
    Serial.println(promptBuff);
  }
  
  sprintf(promptBuff,"y[0] = %f\nm[0] = %f ", y[0], m[0]);
  Serial.println(promptBuff);
  m[0] = (y[1] - y[0]) / (x[1] - x[0]);
  //Print initial values too
  for(int i = 2; i <= 20; i=i+2) {
    y[i] = m[i/2-1]*(x[i]-x[i-1]) + y[i-1];
    m[i/2] = (y[i] - y[i-1]) / (x[i] - x[i-1]);
    char promptBuff[65];
    sprintf(promptBuff,"y[%d] = %f\nm[%d] = %f ", i, y[i], i/2, m[i/2]);
    Serial.println(promptBuff);
  }
  
  conf.x0 = x[0]; 
  conf.y0 = y[0];                                  
  conf.m0 = 1 / m[0];                              
  conf.b0 = x[0] - y[0] / m[0];                              

  conf.x1 = x[1];
  conf.y1 = y[2];                                  
  conf.m1 = 1 / m[1];                              
  conf.b1 = x[2] - y[2] / m[1];
  
  conf.x2 = x[3];
  conf.y2 = y[4];                                  
  conf.m2 = 1 / m[2];                              
  conf.b2 = x[4] - y[4] / m[2];

  conf.x3 = x[5];
  conf.y3 = y[6];                                  
  conf.m3 = 1 / m[3];                              
  conf.b3 = x[6] - y[6] / m[3];

  conf.x4 = x[7];
  conf.y4 = y[8];                                  
  conf.m4 = 1 / m[4];                              
  conf.b4 = x[8] - y[8] / m[4];
  
  conf.x5 = x[9];
  conf.y5 = y[10];                                  
  conf.m5 = 1 / m[5];                              
  conf.b5 = x[10] - y[10] / m[5];
  
  conf.x6 = x[11];
  conf.y6 = y[12];                                  
  conf.m6 = 1 / m[6];                              
  conf.b6 = x[12] - y[12] / m[6];
  
  conf.x7 = x[13];
  conf.y7 = y[14];                                  
  conf.m7 = 1 / m[7];                              
  conf.b7 = x[14] - y[14] / m[7];
  
  conf.x8 = x[15];
  conf.y8 = y[16];                                  
  conf.m8 = 1 / m[8];                              
  conf.b8 = x[16] - y[16] / m[8];
  
  conf.x9 = x[17];
  conf.y9 = y[18];                                  
  conf.m9 = 1 / m[9];                              
  conf.b9 = x[18] - y[18] / m[9];
  
  conf.x10 = x[19];
  conf.y10 = y[20];                                  
  conf.m10 = 1 / m[10];                              
  conf.b10 = x[20] - y[20] / m[10];
  

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
  //Set up potentiometer
  setupDigiPot();
  //Set reference voltage to be on half-time
  analogWrite(refVolt,128);

  Serial.begin(9600);
}

int RAN_ONCE = 0;
void loop() {
  
    if(!RAN_ONCE) {
      RAN_ONCE = 1;
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
        readTurbNTU();
        break;
      case 4:
        caliTurb();
        Serial.println("Turbidity configuration:");
        printStruct();
        break;
      default:
        break;
      }
    }
    promptUser(); 
}


