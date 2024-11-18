#include <PeakDetection.h>
#include <TMRpcm.h>
#include <SPI.h>
#define GARAGE_PIN_1 A1
#define GARAGE_PIN_2 A3
#define GARAGE_PIN_3 A5
#define PIR_1 A0
#define PIR_2 A2
#define PIR_3 A4
#define TriggerOut 4
#define CameraOut 5

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

  pinMode(PIR_1, INPUT);
  pinMode(GARAGE_PIN_1, INPUT);
  pinMode(PIR_2, INPUT);
  pinMode(GARAGE_PIN_2, INPUT);
  pinMode(PIR_3, INPUT);
  pinMode(GARAGE_PIN_3, INPUT);
  pinMode(TriggerOut, OUTPUT);
  pinMode(CameraOut, OUTPUT);
  digitalWrite(TriggerOut, LOW);
  digitalWrite(CameraOut, LOW);

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
    int rawG1 = analogRead(GARAGE_PIN_1);
    int rawG2 = analogRead(GARAGE_PIN_2);
    int rawG3 = analogRead(GARAGE_PIN_3);
    int max12 = max(rawG1, rawG2);
    int rawGarage = max(max12, rawG3);
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

        double data = (double)analogRead(PIR_1) / 512 - 1;
        peakDetection.add(data);
        int peak = peakDetection.getPeak();
        pirPeaks[arrInd] = (peak == 1) ? abs(peak) : 0;
//        Serial.println(peak);
//        Serial.println(peakDetection.getFilt());
//        Serial.println((listSum(pirPeaks, LEN_WINDOW)));

        if (listSum(pirPeaks, LEN_WINDOW) >= LEN_WINDOW * PIR_SENSITIVITY_PERC) {
          readyToDetect = false;
          digitalWrite(TriggerOut, HIGH);
          digitalWrite(CameraOut, HIGH);
          Serial.println(F("TRIGGERING"));
          delay(500);
          digitalWrite(TriggerOut, LOW);
          digitalWrite(CameraOut, LOW);

          delay(10000);

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
