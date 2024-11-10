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
const int LEN_GARAGE = 15;
int count = 0;
int pirPeaks[LEN_WINDOW];
float garageAvg[LEN_GARAGE];

const int relayPin = 7;
const unsigned long interval = 3000;
unsigned long startPIR = millis();

void setup() {
  Serial.begin(9600);
  Serial.println(F("hello"));

  pinMode(PIR, INPUT);
  pinMode(GARAGE_PIN, INPUT);
  pinMode(TriggerOut, OUTPUT);
  digitalWrite(TriggerOut, LOW);

  peakDetection.begin(48, 2, 0.6);
  Serial.println(F("hilo"));
  
  for (int i = 0; i < LEN_WINDOW; i++) {
    pirPeaks[i] = 0;
  }
  for (int i = 0; i < LEN_GARAGE; i++) {
    garageAvg[i] = 100;
  }
}

void loop() {
  count++;

  if (readyToDetect) {
    int rawGarage = analogRead(GARAGE_PIN);
    float mVolts = rawGarage * (5.0 / 1023.0) * 1000.0;
    int garInd = count % LEN_GARAGE;
    garageAvg[garInd] = mVolts;

    if ((listSumDouble(garageAvg, LEN_GARAGE) / LEN_GARAGE) < 1.0) {
      garagePOS = true;
    } else {
      garagePOS = false;
    }

    if (garagePOS) {
      Serial.println(F("Garage door is broken"));
      startPIR = millis();

      while (millis() - startPIR < 5000) {
        count++;
        int arrInd = count % LEN_WINDOW;

        double data = (double)analogRead(PIR) / 512 - 1;
        peakDetection.add(data);
        int peak = peakDetection.getPeak();
        pirPeaks[arrInd] = (peak == 1) ? abs(peak) : 0;

        if (listSum(pirPeaks, LEN_WINDOW) >= LEN_WINDOW * 0.3) {
          readyToDetect = false;
          digitalWrite(TriggerOut, HIGH);
          Serial.println(F("TRIGGERING"));
          delay(100);
          digitalWrite(TriggerOut, LOW);

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
