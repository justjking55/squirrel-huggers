#include <Servo.h> //Include Servo Library

#include "Adafruit_TCS34725.h"

#include <Adafruit_TiCoServo.h>

 

Adafruit_TiCoServo servoLeft;

Adafruit_TiCoServo servoRight;

//Pins for QTI connections on board

#define lineSensor1 49

#define lineSensor2 51

#define lineSensor3 53

 

//rgb pins

#define redPin 45

#define greenPin 46

#define bluePin 44

 

Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);

 

void setup() {

  Serial.begin(9600); //start the serial monitor so we can view the output

 

  servoLeft.attach(11);

  servoRight.attach(12);

 

  pinMode(redPin, OUTPUT);

  pinMode(greenPin, OUTPUT);

  pinMode(bluePin, OUTPUT);

  set_RGB(255,255,255);

  float red, green, blue;

 

    if (tcs.begin()) {

    Serial.println("Found sensor");

  } else {

    Serial.println("No TCS34725 found ... check your connections");

    while (1); // halt!

  }

//   tcs.getRGB(&red, &green, &blue);

 

// Serial.print("R:\t"); Serial.print(int(red));

// Serial.print("\tG:\t"); Serial.print(int(green));

// Serial.print("\tB:\t"); Serial.print(int(blue));

// Serial.print("\n");

// int color = classify(int(red), int(green),int(blue));

}

 

 

  int hashCounter = 0;

 

void loop() {

  float red, green, blue;

 

  int qti1 = rcTime(lineSensor1);

  delay(10);

  //Serial.println(qti1);  

 

  int qti2 = rcTime(lineSensor2);

  delay(10);

  //Serial.println(qti2);

 

  int qti3 = rcTime(lineSensor3);

  delay(10);

  //Serial.println(qti3);

  // Serial.println(qti1);

  // Serial.println(qti2);

  // Serial.println(qti3);

  boolean sLeft;

  if (qti1 > 200){
    sLeft = false;
  }

  else{

    sLeft = true;

  }

 

    boolean sMiddle;

  if (qti2 > 1750){

    sMiddle = false;

  }

  else{

    sMiddle = true;

  }

   

  boolean sRight;

    if (qti3 > 100){

      sRight = false;

    }

    else{

      sRight = true;

    }

 

  int state = 4*sLeft + 2*sMiddle + sRight;

  //Serial.println(state);
  Serial.println(state);

 

switch (state) {

 

case 0:
Serial.println("case 0");

hashCounter++;

servoLeft.write(1500);

servoRight.write(1500);

delay(60);  // takes 50ms to read

tcs.getRGB(&red, &green, &blue);

 

Serial.print("R:\t"); Serial.print(int(red));

Serial.print("\tG:\t"); Serial.print(int(green));

Serial.print("\tB:\t"); Serial.print(int(blue));

Serial.print("\n");

int color = classify(int(red), int(green),int(blue));

delay(1000);

//change light

hashCounter = hashmark(hashCounter);

//moves forward

servoLeft.write(1600);

servoRight.write(1400);

delay(200);

 

break;

 

case 1:

servoLeft.write(1475);

servoRight.write(1400);

break;

 

case 3:

// turn left and move forward slightly until state is 0

servoLeft.write(1475);

servoRight.write(1400);

break;

 

case 4:

servoLeft.write(1570);

servoRight.write(1510);

break;

 

case 5:

 

servoLeft.write(1550);

servoRight.write(1460);

break;

 

case 6:

servoLeft.write(1585);

servoRight.write(1535);

break;

 

case 7:

servoLeft.write(1500);

servoRight.write(1500);

 

break;

 

default:

servoLeft.write(1500);

servoRight.write(1500);

break;

 

}

 

}

 

int hashmark(int num) {

  if (num == 1) { //first hashmark, etc.

    set_RGB(0, 255, 255); //red

    delay(1000);

    set_RGB(255, 255, 255); //turn light back off after one second

    return 1;

  }

  else if(num == 2) {

    set_RGB(0, 0, 255); //yellow

    delay(1000);

    set_RGB(255, 255, 255);

    return 2;

 

  }

  else if(num == 3) {

    set_RGB(255, 0, 255); //green

    delay(1000);

    set_RGB(255, 255, 255);

    return 3;

 

  }

  else if(num == 4) {

    set_RGB(255, 255, 0); //blue

    delay(1000);

    set_RGB(255, 255, 255);

    return 4;

 

  }

  else if(num == 5) {

    set_RGB(0, 255, 0); //purple

    delay(1000);

    set_RGB(255, 255, 255);

    servoLeft.write(1500);

    servoRight.write(1500);

    servoLeft.detach();

    servoRight.detach();

    return 5;

  } else if (num == 0) {

    set_RGB(255,255,255);

    return 0;

 

  }

  delay(1000);

}

 

void set_RGB(int r, int g, int b) {

  // Set RGB LED pins based on low=bright (default)

  analogWrite(redPin, r);

  analogWrite(greenPin, g);

  analogWrite(bluePin, b);

}

//classify colors and store in array

// yellow = 1

// blue = 2

// orange = 3

// red = 4

// green = 5

int colorOrder[5];

 

//returns -1 on error

int classify(int r, int g, int b){

  Serial.println("****************");

  int tol = 10;

  if((r<91+tol||r>91-tol)&&(g<106+tol||g>106-tol)&&(b<63+tol||b>63+tol)){

    Serial.println("Green");

    return 5;

  } else if((r<141+tol||r>141-tol)&&(g<74+tol||g>74-tol)&&(b<52+tol||b>52+tol)){

    Serial.println("Orange");

    return 3;

  } else if((r<80+tol||r>80-tol)&&(g<93+tol||g>93-tol)&&(b<90+tol||b>90+tol)){

    Serial.println("Blue");

    return 2;

  } else if ((r<142+tol||r>142-tol)&&(g<70+tol||g>70-tol)&&(b<58+tol||b>58+tol)){

    Serial.println("Red");

    return 4;

  } else if ((r<119+tol||r>119-tol)&&(g<94+tol||g>94-tol)&&(b<49+tol||b>49+tol)){

    Serial.println("Yellow");

    return 1;

  } else {

    return -1;

  }

  return -1;

}

 

//Defines funtion 'rcTime' to read value from QTI sensor

// From Ch. 6 Activity 2 of Robotics with the BOE Shield for Arduino

long rcTime(int pin)

{

  pinMode(pin, OUTPUT);    // Sets pin as OUTPUT
  digitalWrite(pin, HIGH); // Pin HIGH

  delay(1);                // Waits for 1 millisecond

  pinMode(pin, INPUT);     // Sets pin as INPUT

  digitalWrite(pin, LOW);  // Pin LOW

  long time = micros();    // Tracks starting time

  while(digitalRead(pin)); // Loops while voltage is high

  time = micros() - time;  // Calculate decay time

  return time;             // Return decay time

}

 
