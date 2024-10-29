#include "Adafruit_TCS34725.h"

Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);

#define redpin 45
#define greenpin 46
#define bluepin 44


void setup() {
  Serial.begin(9600);
  Serial.println("Sensor Test");


  if (tcs.begin()) {
    Serial.println("Found sensor");
  } else {
    Serial.println("No TCS34725 found ... check your connections");
    while (1); // halt!
  }

  pinMode(redpin, OUTPUT);
  pinMode(greenpin, OUTPUT);
  pinMode(bluepin, OUTPUT);

  // Start with light red
  set_RGBi(255, 0, 0);
  delay(100);
}

void loop() {
  float red, green, blue;

  delay(60);  // takes 50ms to read
  tcs.getRGB(&red, &green, &blue);

  Serial.print("R:\t"); Serial.print(int(red)); 
  Serial.print("\tG:\t"); Serial.print(int(green)); 
  Serial.print("\tB:\t"); Serial.print(int(blue));
  Serial.print("\n");

  set_RGBi(int(red), int(green), int(blue));
}


void set_RGB(int r, int g, int b){
  // Set RGB LED pins based on low=bright (default)
  analogWrite(redpin, r);
  analogWrite(greenpin, g);
  analogWrite(bluepin, b);
}


void set_RGBi(int r, int g, int b){
  // Set RGB LED pins based on high=bright 
  set_RGB(255-r, 255-g, 255-b);
}
