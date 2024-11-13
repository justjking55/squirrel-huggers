#include <PeakDetection.h>
#include <TMRpcm.h>
#include <SPI.h>
#define GARAGE_PIN A1
#define PIR A0
#define TriggerOut 4

PeakDetection peakDetection;
PeakDetection garage;

bool readyToDetect = true;
bool pirPOS = false;
bool garagePOS = false;
const int LEN_WINDOW = 20;
const int LEN_GARAGE = 40;
int count = 0;
int zeroCount = 0;
int pirPeaks[LEN_WINDOW];
//float garageAvg[LEN_GARAGE];
float lastMVolts = -1.0;

float PIR_SENSITIVITY_PERC = 0.5;
int GARAGE_THRESH = 50;

const int relayPin = 7;
const unsigned long interval = 3000;
unsigned long startPIR = millis();

void setup() {
  Serial.begin(9600);
//  Serial.println(F("hello"));

  pinMode(PIR, INPUT);
  pinMode(GARAGE_PIN, INPUT);
  pinMode(TriggerOut, OUTPUT);
  digitalWrite(TriggerOut, LOW);

  peakDetection.begin(48, 2, 0.6);
//  Serial.println(F("hilo"));
  
  for (int i = 0; i < LEN_WINDOW; i++) {
    pirPeaks[i] = 0;
  }
//  for (int i = 0; i < LEN_GARAGE; i++) {
//    garageAvg[i] = 100;
//  }
  
}

void loop() {
  count++;

  if (readyToDetect) {
    int rawGarage = analogRead(GARAGE_PIN);
    float mVolts = rawGarage * (5.0 / 1023.0) * 1000.0;
    int garInd = count % LEN_GARAGE;

    if(lastMVolts == 0.00) {
      if(mVolts == 0.00) {
        zeroCount++;
      }
      else {
        zeroCount = 0;
      }
    }
    else {
      if(mVolts == 0.00) {
        zeroCount = 1;
      }
    }
    lastMVolts = mVolts;
//    garageAvg[garInd] = mVolts;
    
//    Serial.println(listSumDouble(garageAvg, LEN_GARAGE)/LEN_GARAGE);

//    if ((listSumDouble(garageAvg, LEN_GARAGE) / LEN_GARAGE) < 1.0) {
//      garagePOS = true;
//    } else {
//      garagePOS = false;
//    }
    if (zeroCount > GARAGE_THRESH) {
      garagePOS = true;
      zeroCount = 0;
    } else {
      garagePOS = false;
    }

    if (garagePOS) {
      Serial.println(F("Garage door is broken"));
      startPIR = millis();

      while (millis() - startPIR < 2000) {
        count++;
        int arrInd = count % LEN_WINDOW;

        double data = (double)analogRead(PIR) / 512 - 1;
        peakDetection.add(data);
        int peak = peakDetection.getPeak();
        pirPeaks[arrInd] = (peak == 1) ? abs(peak) : 0;
//        Serial.println(peak);
//        Serial.println(peakDetection.getFilt());
//        Serial.println((listSum(pirPeaks, LEN_WINDOW)));

        if (listSum(pirPeaks, LEN_WINDOW) >= LEN_WINDOW * PIR_SENSITIVITY_PERC) {
          readyToDetect = false;
          digitalWrite(TriggerOut, HIGH);
          Serial.println(F("TRIGGERING"));
          delay(500);
          digitalWrite(TriggerOut, LOW);

          for (int i = 0; i < LEN_WINDOW; i++) {
            pirPeaks[i] = 0;
          }
//          for (int i = 0; i < LEN_GARAGE; i++) {
//            garageAvg[i] = 100;
//          }
          readyToDetect = true;
          startPIR = startPIR - 2000;
        }
      }

      garagePOS = false;
    }
  }
}

int listSum(int pirArr[], int LEN_WINDOW) {
  int sum = 0;
  for (int i = 0; i < LEN_WINDOW; i++) {
    sum += pirArr[i];
  }
  return sum;
}

float listSumDouble(float pirArr[], int LEN_GARAGE) {
  float sum = 0;
  for (int i = 0; i < LEN_GARAGE; i++) {
    sum += pirArr[i];
  }
  return sum;
}
