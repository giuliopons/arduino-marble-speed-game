/*

    marble speed game

*/


/*  display  */
#include "TM1637.h"
#define CLK 2
#define DIO 3
TM1637 tm1637(CLK, DIO);

/*  pins variables */
int sensorPin1=A0;
int sensorPin2=A1;
int buttonPin = 5;
int ledPin = 7;

/*  timer variables  */
unsigned long t0 = 0;
unsigned long t1 = 0;

int thresholdLight1; 
int thresholdLight2; 
int status = 0; // 0 begib,   1 start measuring,     2 stop timer and calculate,   3 show measure
int buttonState;

/* read the avarage value of a LDR lightened sensor */
int readAvarageLDR(int pinSensor){
  unsigned long t = millis() + 1000;
  unsigned long c = 0;
  unsigned long avarage = 0;
  while(millis() < t) {
    avarage+=analogRead(pinSensor);
    c++;
  }
  avarage = avarage/c;
  return avarage;
}

/* set the thresholds value of the LDR to detect the ball passing */
void setThresholds() {
    digitalWrite(ledPin,HIGH);
    delay(500);
    thresholdLight1 = readAvarageLDR(sensorPin1) - 35;
    thresholdLight2 = readAvarageLDR(sensorPin2) - 25;
    Serial.println("");
    Serial.println("");
    Serial.print("Level LDR 1: ");
    Serial.println(thresholdLight1);
    Serial.print("Level LDR 2: ");
    Serial.println(thresholdLight2);   
    Serial.println("---- ready to measure ----");
    printNumber(0);
}
void setup() {
  tm1637.init();
  tm1637.set(BRIGHT_TYPICAL);

  pinMode(sensorPin1,INPUT);
  pinMode(sensorPin2,INPUT);
  pinMode(buttonPin,INPUT);
  pinMode(ledPin, OUTPUT);

  Serial.begin(230400);

  digitalWrite(ledPin,LOW);
  status= 0;
  setThresholds();
}

/* print the number in the 4 digits 7 segmnents display */
void printNumber(int number) {
  int8_t digitArr[4];
  for (int i = 3; i >= 0; --i) {
    digitArr[i] = number % 10;
    number /= 10;
  }
  for (int i = 0; i < 4; ++i) {
    tm1637.display(i, digitArr[i]);
  }
}

/* print values */
void pr(int v1, int v2, unsigned long t0, unsigned long t1, unsigned long r, float v, int status) {
  int vel = v*100;
  Serial.println("LDR1:" + (String)v1 + " LDR2:" + (String)v2 + " " + (String)t0+ "msec " + (String)t1 + "msec " + (String)v + "m/s " + (String)vel + "cm/s " + " STATUS:"+(String)status); 
}



/* thwe game! */
void loop() {
  int v1 = analogRead(sensorPin1);
  int v2 = analogRead(sensorPin2);

  if(status==0 && v1 < thresholdLight1) {
    // timer starts!
    status = 1;
    t0 = millis();
    pr(v1,v2,t0,0,0,0,status);
    
  }

  if(status==1 && v2 < thresholdLight2) {
    // stopping timer
    status = 2;
    t1 = millis();
    pr(v1,v2,t0,t1,0,0,status);
  }

  if(status == 1) {
    // show the time while the marble is running in the tube */
    unsigned long t = (millis() - t0);
    printNumber(t);
  }
  
  if(status == 2) {
    // calculate speed and show the result
   int r = t1 - t0;
   status = 3;
   digitalWrite(ledPin,LOW);
   float v = ( 1000.0/ r ) ;   // m / sec   x 100
   pr(v1,v2,t0,t1,r,v,status);    
   v = v*100;
   printNumber((int) v);
  }

  buttonState = digitalRead(buttonPin); // Read the state of the button
  if (buttonState == LOW) {
    // button press => start again!
    tm1637.clearDisplay();
    status= 0;
    setThresholds();
   }

}
