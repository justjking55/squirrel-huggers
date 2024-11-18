#include <PeakDetection.h>  // Peak detection lib for PIR (if needed in the future)
#include <SD.h>             // SD library
#include <TMRpcm.h>         // TMRpcm library for audio
#include <SPI.h>            // SPI library for general functionality

#define SD_ChipSelectPin 4  // SD card select pin on Arduino Nano 328 (can use other pins)
#define triggerPin 8       // Input pin for triggering the relay and audio
#define chargePin 3         // Output pin for the charge relay
#define flashPin 2 
#define mutePin 7
#define sPinUp 6            // Output pin for scarecrow spooling up
#define sPinDown 5          // Output pin for scarecrow spooling down

TMRpcm tmrpcm;              // Create TMRpcm object for speaker control

unsigned long chargeStartTime = 0;
unsigned long lastChargeCycle = 0;
const unsigned long chargeDuration = 7000;        // 15 seconds charge duration
const unsigned long chargeInterval = 10 * 60 * 1000; // 10 minutes charge interval

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
  pinMode(chargePin, OUTPUT);                    // Set charge relay pin as output
  pinMode(flashPin, OUTPUT);                     // Set flash relay pin as output
  pinMode(sPinUp, OUTPUT);
  pinMode(sPinDown, OUTPUT);
  pinMode(mutePin, OUTPUT);
  digitalWrite(chargePin, LOW);                  // Keep relay off initially
  digitalWrite(flashPin, LOW);                   // Keep relay off initially
  digitalWrite(sPinUp, LOW);
  digitalWrite(sPinDown, LOW);
  digitalWrite(mutePin, LOW);

  Serial.println(F("Setup Complete"));
}

void loop() {
  unsigned long currentTime = millis();
  
  // Start a new charge cycle every 10 minutes if not already charging
  if (currentTime - lastChargeCycle >= chargeInterval or lastChargeCycle == 0) {
    digitalWrite(chargePin, HIGH);               // Start charging
    chargeStartTime = currentTime;               // Record the start time
    lastChargeCycle = currentTime;               // Update last charge cycle time
    Serial.println(F("Charging relay activated"));
  }
  
  // Stop charging after 15 seconds
  if (chargeStartTime > 0 and currentTime - chargeStartTime >= chargeDuration) {
    digitalWrite(chargePin, LOW);                // Stop charging
    chargeStartTime = 0;                         // Reset charge start time
    Serial.println(F("Charging relay deactivated"));
    lastChargeCycle = currentTime;
  }

  // Check if trigger pin is HIGH
  if (digitalRead(triggerPin) == HIGH) {
    Serial.println(F("Trigger detected"));

    digitalWrite(mutePin, HIGH);
    // Activate scarecrow and play sound 
    digitalWrite(flashPin, HIGH);                // Flash trigger
    digitalWrite(sPinUp, HIGH);
    tmrpcm.play("dog1.WAV");
    delay(1500);                                 // Flash duration
    digitalWrite(flashPin, LOW);                 // Turn flash off
    digitalWrite(sPinUp, LOW);
    delay(4400);                                 // Delay for sound playback
    digitalWrite(sPinDown, HIGH);
    delay(1500);
    digitalWrite(sPinDown, LOW);
    digitalWrite(mutePin, LOW);

    tmrpcm.pause();                              // Pause audio after playback
    Serial.println(F("Deterrence Triggered"));

    // Restart charge cycle on trigger
    lastChargeCycle = currentTime;               // Reset charge cycle timer
  }
}
