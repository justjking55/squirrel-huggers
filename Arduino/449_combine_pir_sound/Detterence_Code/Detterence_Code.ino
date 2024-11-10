#include <PeakDetection.h>  // Peak detection lib for PIR (if needed in the future)
#include <SD.h>             // SD library
#include <TMRpcm.h>         // TMRpcm library for audio
#include <SPI.h>            // SPI library for general functionality

#define SD_ChipSelectPin 4  // SD card select pin on Arduino Nano 328 (can use other pins)
#define triggerPin 2        // Input pin for triggering the relay and audio
#define relayPin 7          // Output pin for the relay

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
  digitalWrite(relayPin, LOW);                   // Keep relay off initially

  Serial.println(F("Setup Complete"));
}

void loop() {
  int count=0;
  if(count % 5 == 0){
  //if (digitalRead(triggerPin) == HIGH) {         // Check if trigger pin is HIGH
    Serial.println(F("Trigger detected"));

    // Activate relay and play sound
    tmrpcm.play("dog1.WAV");
    digitalWrite(relayPin, HIGH);                // Turn relay ON
    delay(100);                                  // Flash duration (adjust if needed)
    digitalWrite(relayPin, LOW);                 // Turn relay OFF
    delay(4900);                                 // Delay to allow sound to play

    tmrpcm.pause();                              // Pause audio after playback
    Serial.println(F("Deterrence Triggered"));
  }
  delay(10000);
  count++;
}
