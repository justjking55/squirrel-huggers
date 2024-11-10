#include <PeakDetection.h>                       // peak detection lib for pir
#include <SD.h>                      // need to include the SD library
#define SD_ChipSelectPin 4  //using digital pin 4 on arduino nano 328, can use other pins
#include <TMRpcm.h>           //  also need to include this library...
#include <SPI.h>
#define GARAGE_PIN A1
#define PIR A0

PeakDetection peakDetection;                     // create PeakDetection object
TMRpcm tmrpcm;   // create TMRpcm object (for speaker)
PeakDetection garage; //using for moving average

bool readyToDetect = true;
bool pirPOS = false;
bool garagePOS = false;
const int LEN_WINDOW = 20;
const int LEN_GARAGE = 15;
int count = 0;
int pirPeaks[20];
float garageAvg[15];


// Pin where the relay is connected
const int relayPin = 7;

// Time intervals in milliseconds
const unsigned long interval = 3000; // 5 seconds
unsigned long startPIR = millis();

  
void setup() {
  Serial.begin(9600);                            // set the data rate for the Serial communication
  Serial.println(F("hello"));
  if (!SD.begin(SD_ChipSelectPin)) {  // see if the card is present and can be initialized:
    Serial.println(F("SD fail"));  
    return;   // don't do anything more if not
  }
  
  pinMode(A0, INPUT);                            // analog pin used to connect the sensor
  tmrpcm.speakerPin = 9; //5,6,11 or 46 on Mega, 9 on Uno, Nano, etc
  pinMode(A1, INPUT); //garage door sensor pin
  Serial.println(F("test"));
  pinMode(relayPin, OUTPUT); // Initialize the relay pin as an output
  Serial.println(F("woah"));
  digitalWrite(relayPin, LOW); // Set relay to LOW initially to keep it off
  tmrpcm.setVolume(4);
  peakDetection.begin(48, 2, 0.6);               // sets the lag, threshold and influence
  Serial.println(F("hilo"));
  for (int i = 0; i < LEN_WINDOW; i++) {
    pirPeaks[i] = 0;
  }
  for (int i = 0; i < LEN_GARAGE; i++) {
    garageAvg[i] = 100;
  }

}

void loop() {
    count = count + 1;
    
    if(readyToDetect) {
      //Serial.println(F("ready"));

      int rawGarage = analogRead(A1);
      float mVolts = rawGarage * (5.0/1023.0) * (1000.0);

      int garInd = count % LEN_GARAGE;
      garageAvg[garInd] = mVolts;
      //scaledGarage is in mV - should be ~100 mV if intact, < 1 mV (~.1mV) if broken
//      Serial.println(listSumDouble(garageAvg, LEN_GARAGE)/LEN_GARAGE);
//      Serial.println(mVolts);

      //logic: always check garage. if garage broken (garagePOS) then enter a loop to start checking PIR for x seconds
      if((listSumDouble(garageAvg, LEN_GARAGE)/LEN_GARAGE) < 1.0) {
        garagePOS = true;
        
      }
      else {
        garagePOS = false;
      }

      //if PIR doesn't return anything, then set garagePOS back to false. loop restarts (readyToDetect = true)
      //if PIR does detect anything, (if(listSum...)...), trigger all deterrence

      if(garagePOS) {
        startPIR = millis();
        
        while(millis() - startPIR < 5000) {
          count = count + 1;
          
          //this is PIR stuff
          int arrInd = count % LEN_WINDOW;
          
          double data = (double)analogRead(A0)/512-1;  // converts the sensor value to a range between -1 and 1
          peakDetection.add(data);                     // adds a new data point
          int peak = peakDetection.getPeak();          // 0, 1 or -1
          if(peak == 1) {
            pirPeaks[arrInd] = abs(peak);
          }
          else {
            pirPeaks[arrInd] = 0;
          }
          
          double filtered = peakDetection.getFilt();   // moving average
    
          if(listSum(pirPeaks,LEN_WINDOW) >= LEN_WINDOW * 0.3) {
            //ALL DETERRENCE TRIGGERS HERE
            readyToDetect = false;
            Serial.println("TEST");
            //start speaker
            tmrpcm.setVolume(4);
            tmrpcm.play("dog1.WAV");
//            Serial.println("music");
            
            digitalWrite(relayPin, HIGH);
            delay(100); // Flash duration (adjust if needed)
            digitalWrite(relayPin, LOW); // Turn off the relay
            delay(4900); //4900 + 100 = roughly 5 seconds of music
            tmrpcm.pause();

            //triggerScarecrow(); //motion deterrence

            //resetting pirPeaks list
            for (int i = 0; i < LEN_WINDOW; i++) {
              pirPeaks[i] = 0;
            }
            for (int i = 0; i < LEN_GARAGE; i++) {
              garageAvg[i] = 100;
            }
            readyToDetect = true;
          }
        }

        garagePOS = false;
  
      }
  
    }
    
}


int listSum(int pirArr[],int LEN_WINDOW) {
  int sum = 0;
  for (int i = 0; i < LEN_WINDOW; i++) {
    sum += pirArr[i];
  }
  return sum;
}
int listSumDouble(float pirArr[],int LEN_WINDOW) {
  int sum = 0;
  for (int i = 0; i < LEN_WINDOW; i++) {
    sum += pirArr[i];
  }
  return sum;
}
