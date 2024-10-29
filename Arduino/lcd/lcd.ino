/****************************************************************************** 
MLX90614_Serial_Demo.ino 
Serial output example for the MLX90614 Infrared Thermometer

This example reads from the MLX90614 and prints out ambient and object 
temperatures every half-second or so. Open the serial monitor and set the
baud rate to 9600.

Hardware Hookup (if you're not using the eval board):
MLX90614 ------------- Arduino
  VDD ------------------ 3.3V
  VSS ------------------ GND
  SDA ------------------ SDA (A4 on older boards) (pin 20)
  SCL ------------------ SCL (A5 on older boards) (pin 21)
  
An LED can be attached to pin 8 to monitor for any read errors.

Jim Lindblom @ SparkFun Electronics
October 23, 2015
https://github.com/sparkfun/SparkFun_MLX90614_Arduino_Library

Development environment specifics:
Arduino 1.6.5
SparkFun IR Thermometer Evaluation Board - MLX90614
******************************************************************************/

#include <Wire.h> // I2C library, required for MLX90614
#include <SparkFunMLX90614.h> //Click here to get the library: http://librarymanager/All#Qwiic_IR_Thermometer by SparkFun
#include <SoftwareSerial.h>

#define TxPin 14
#define num 17

SoftwareSerial mySerial = SoftwareSerial(255, TxPin);

IRTherm therm; // Create an IRTherm object to interact with throughout

int durs[num]  = {211, 211, 211, 210, 210, 
                211, 211, 211, 211, 211, 211, 211, 210, 210, 211, 211, 212
};
int octs[num]  = {215, 215, 215, 215, 215, 
                215, 215, 215, 215, 215, 215, 215, 215, 215, 215, 215, 215
};
int notes[num] = {220, 220, 220, 220, 220,
                220, 225, 232, 220, 220, 220, 220, 220, 220, 220, 225, 232
};

void setup() 
{
  Serial.begin(9600); // Initialize Serial to log output
  mySerial.begin(9600);
  delay(100);
  mySerial.write(12); // clear
  delay(10);
  mySerial.write(22); // no cursor no blink
  delay(10);
  mySerial.write(17); // backlight
  delay(10);
  Wire.begin(); //Joing I2C bus
  
  if (therm.begin() == false){ // Initialize thermal IR sensor
    Serial.println("Qwiic IR thermometer did not acknowledge! Freezing!");
    while(1);
  }
  Serial.println("Qwiic IR Thermometer did acknowledge.");
  
  therm.setUnit(TEMP_F); // Set the library's units to Farenheit (sic)
  // Alternatively, TEMP_F can be replaced with TEMP_C for Celsius or
  // TEMP_K for Kelvin.
  
  pinMode(LED_BUILTIN, OUTPUT); // LED pin as output
  
  
}
void iceicebaby(){
  for(long k=0; k<num; k++){
    
    mySerial.write(durs[k]); mySerial.write(octs[k]); mySerial.write(notes[k]);
    int len = 214 - durs[k];
    float del = 2000 / pow(2, len);
    delay(int(del*1.1));
  }
}
void loop() {
  digitalWrite(LED_BUILTIN, HIGH);
    
  // Call therm.read() to read object and ambient temperatures from the sensor.
  if (therm.read()) // On success, read() will return 1, on fail 0.
  {
    // Use the object() and ambient() functions to grab the object and ambient
  // temperatures.
  // They'll be floats, calculated out to the unit you set with setUnit().
    Serial.print("Object: " + String(therm.object(), 2));
    Serial.println("F");
    Serial.print("Ambient: " + String(therm.ambient(), 2));
    Serial.println("F");
    Serial.println();
    if (therm.object() < 50){
    iceicebaby();
  } 
    delay(10000);
  }
  digitalWrite(LED_BUILTIN, LOW);
  delay(1000);
}
