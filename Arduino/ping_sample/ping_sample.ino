/*
  Ping))) Sensor - higher res

  created 3 Nov 2008
  by David A. Mellis
  modified 30 Aug 2011
  by Tom Igoe
  modified 5 Sep 2022
  by Michael Gustafson

  This example code is in the public domain.

  original: https://www.arduino.cc/en/Tutorial/BuiltInExamples/Ping
*/
#include <Servo.h>  
const int pingPin = 35;
Servo servoLeft;
Servo servoRight;

void setup() {
  Serial.begin(9600);
  pinMode(37, OUTPUT);
  pinMode(39, OUTPUT);
  digitalWrite(37, HIGH);
  digitalWrite(39, LOW);
  
  servoLeft.attach(12);
  servoRight.attach(11);
}

void loop() {
  long duration;
  float inches, cm;
  // short LOW pulse to ensure a clean HIGH pulse:
  pinMode(pingPin, OUTPUT);
  digitalWrite(pingPin, LOW);
  // 2 ms HIGH pulse
  delayMicroseconds(2);
  digitalWrite(pingPin, HIGH);
  // back to LOW
  delayMicroseconds(5);
  digitalWrite(pingPin, LOW);

  // The same pin is used to read the signal from the PING))): 
  // a HIGH pulse whose duration is the time (in microseconds) 
  // from the sending of the ping to the reception of 
  // its echo off of an object.
  pinMode(pingPin, INPUT);
  duration = pulseIn(pingPin, HIGH);

  // convert the time into a distance
  inches = microsecondsToInches(duration);
  cm = microsecondsToCentimeters(duration);

  if(inches < 6) {
    servoLeft.write(1700);
    servoRight.write(1300);
    Serial.println("too close");
  }
  else if(inches < 10) {
    servoLeft.write(1500);
    servoRight.write(1500);
    Serial.println("just right");
  }
  else {
    servoLeft.write(1300);
    servoRight.write(1700);
    Serial.println("too far");
  }
  
  Serial.print(inches);
  Serial.print("in, ");
  Serial.print(cm);
  Serial.print("cm");
  Serial.println();
  delay(200);
}

float microsecondsToInches(long microseconds) {
  // The speed of sound is about 1125 ft/s
  // Sound takes about 74.074 us to travel 1 in
  return microseconds / 74.074 / 2;
}

float microsecondsToCentimeters(long microseconds) {
  // The speed of sound is about 343 m/s
  // Sound takes about 29.155 us to travel 1 cm
  return microseconds / 29.155 / 2.0;
}
