/*
Sampling with the Arduino
example 5 - 9600 baud
from http://sites.goggle.com/site/MeasuringStuff
*/

// the famous LED on digital pin 13
byte ledPin =  13;
// we'll use digital pin 2 to signal the start of data collection
byte letsStartPin = 2;
// our array size
int i=0;
int size=250;
unsigned long time[250];
unsigned int rawdata[250];

void setup() {
  // initialize both serial ports:
  Serial.begin(9600);
  // signal ready to start by turning on LED 13
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, HIGH);
  pinMode(letsStartPin,INPUT);
  // endless loop until letsStartPin goes high;
  do { 
  }
  while (digitalRead(letsStartPin) == LOW); 
  digitalWrite(ledPin, LOW);  
}

void loop() {
  // avoid an internal loop structure - just do it
//  for (i=0 ;i<size;i++){
//    time[i]=micros();
//    rawdata[i]=analogRead(0); 
//  }
//  for (i=0;i<size;i++) {
//    Serial.print(time[i]);
//    Serial.print(" , ");
//    Serial.println(rawdata[i]);
//  }
//  delay(100);

  time[0]=micros();
  rawdata[0]=analogRead(0); 
  Serial.print(time[0]);
  Serial.print(" , ");
  Serial.println(rawdata[0]);
  delay(5);
}
