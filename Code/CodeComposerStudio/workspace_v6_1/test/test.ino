//Pin 2
int read_cond = A11;
//Pin 5 is SDO

//Pin 6
int pulse = PE_5;
//Pin 7
int read_turb = A10;

//Pin 8 is SDI
//Pin 11 is SerialClock
//Pin 12 is SlaveSelect (CS)

//Pin 19
int led_out = PB_2;
//pin 17
int refVolt = PF_0;
//pin 18
int read_temp = A3;

/**
 * XBEE Pins
 **/
 //Pin 31 - DTR

 //Pin 32 - Data In

 //Pin 33 - Data out


//Holds current temperature values
int tempVal = 0;
//Holds current turbidity values
int turbVal = 0;
//Holds current conductivity values
int condVal = 0;

String tempString = "Temperature Value is: ";
String condString = "Conductivity Value is: ";
String turbString = "Turbidity Value is: ";

double getTemp(float RawADC) {
 double temp = 0.0;
 temp = log10(((1024000/RawADC)-1000));
 temp = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * temp * temp ))* temp );
 temp = temp - 273.15;            // Convert Kelvin to Celcius
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

 //Set reference voltage to be on ~half time
 analogWrite(refVolt,128);

 Serial.begin(9600);
}

void loop()
{
  //Read and print temperature
  tempVal = analogRead(read_temp);
  String tempOut = tempString + tempVal;
  Serial.println(tempOut);
  
  //Read and print turbidity
  digitalWrite(led_out, HIGH);

  //Reduce noise a bit
  turbVal = 0;
  for (int a = 0; a<32; a++){
  turbVal += analogRead(read_turb);
  }
  turbVal = turbVal/32;

  String turbOut = turbString + turbVal;
  Serial.println(turbOut);
  Serial.println("");

  //Delay 1 second before reprinting
  delay(1000);
}
