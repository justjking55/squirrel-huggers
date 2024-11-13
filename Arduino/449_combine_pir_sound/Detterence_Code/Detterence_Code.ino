#include <PeakDetection.h>  // Peak detection lib for PIR (if needed in the future)
#include <SD.h>             // SD library
#include <TMRpcm.h>         // TMRpcm library for audio
#include <SPI.h>            // SPI library for general functionality

#define SD_ChipSelectPin 4  // SD card select pin on Arduino Nano 328 (can use other pins)
#define triggerPin 2        // Input pin for triggering the relay and audio
#define relayPin 7          // Output pin for the relay
#define sPinUp 6            // Output pin for scarecrow spooling up
#define sPinDown 5          // Output pin for scarecrow spooling down

TMRpcm tmrpcm;              // Create TMRpcm object for speaker control

void setup() {
  Serial.begin(9600);                            // Set data rate for Serial communication
  Serial.println(F("System Initialized"));

  if (!SD.begin(SD_ChipSelectPin)) {             // Initialize SD card
    Serial.println(F("SD Card Initialization Failed"));
    return;
  }

  tmrpcm.speakerPin = 9;                         // Specify speaker pin
  tmrpcm.setVolume(4);                           // Set audio volume

  pinMode(triggerPin, INPUT);                    // Set trigger pin as input
  pinMode(relayPin, OUTPUT);                     // Set relay pin as output
  pinMode(sPinUp, OUTPUT);
  pinMode(sPinDown, OUTPUT);
  digitalWrite(relayPin, LOW);                   // Keep relay off initially
  digitalWrite(sPinUp, LOW);
  digitalWrite(sPinDown,LOW);

  Serial.println(F("Setup Complete"));
}
int count=0;

void loop() {
  Serial.println(digitalRead(triggerPin));
//  if(count % 5 == 0){
  if (digitalRead(triggerPin) == HIGH) {         // Check if trigger pin is HIGH
    Serial.println(F("Trigger detected"));

    // Activate relay and play sound
    digitalWrite(sPinUp, HIGH);
    tmrpcm.play("dog1.WAV");
    digitalWrite(relayPin, HIGH);                // Turn relay ON
    delay(1500);                                  // Flash duration (adjust if needed)
    digitalWrite(relayPin, LOW);                 // Turn relay OFF
    digitalWrite(sPinUp, LOW);
    delay(4400);                                 // Delay to allow sound to play
    digitalWrite(sPinDown,HIGH);
    delay(1500);
    digitalWrite(sPinDown,LOW);

    tmrpcm.pause();                              // Pause audio after playback
    Serial.println(F("Deterrence Triggered"));
  }
//  delay(10000);
  count++;
}
