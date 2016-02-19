#include "Energia.h"

double getTemp(float RawADC);
void setup();
void loop();

#line 1 "C:/Users/Chris/workspace_v6_1/test/test.ino"

int read_cond = A11;



int pulse = PE_5;

int read_turb = A10;






int led_out = PB_2;

int refVolt = PF_0;

int read_temp = A3;




 

 

 



int tempVal = 0;

int turbVal = 0;

int condVal = 0;

String tempString = "Temperature Value is: ";
String condString = "Conductivity Value is: ";
String turbString = "Turbidity Value is: ";

double getTemp(float RawADC) {
 double temp = 0.0;
 temp = log10(((1024000/RawADC)-1000));
 temp = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * temp * temp ))* temp );
 temp = temp - 273.15;            
 return temp;
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

void loop()
{
  
  tempVal = analogRead(read_temp);
  String tempOut = tempString + tempVal;
  Serial.println(tempOut);
  
  
  digitalWrite(led_out, HIGH);

  
  turbVal = 0;
  for (int a = 0; a<32; a++){
  turbVal += analogRead(read_turb);
  }
  turbVal = turbVal/32;

  String turbOut = turbString + turbVal;
  Serial.println(turbOut);
  Serial.println("");

  
  delay(1000);
}



