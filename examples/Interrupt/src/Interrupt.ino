/*
 * Update superFreq with pin change interrupts. Instant response, but
 * susceptible to noise and switch bounce.
 */

#include <Arduino.h>
#include <PinChangeInterrupt.h>
#include <superFreq.h>

#define PIN 13

superFreq<5> sf;

char PB[78] = {0};

void updateLow() { sf.high(); }
void updateHigh() { sf.low(); }

void setup() {
  Serial.begin(115200);
  attachPCINT(digitalPinToPCINT(PIN), updateHigh, RISING);
  attachPCINT(digitalPinToPCINT(PIN), updateLow, FALLING);
  pinMode(PIN, INPUT);
}

void loop() {
  if (sf.available() > 0) {
    superFreqCycle a = sf.getAvg();
    sf.flush();
    sprintf(PB, "T=%lu F=%.2f PW=%.2f TH=%lu TL=%lu",
            a.getPeriod(), a.getFreq(), a.getDutyCycle(),
            a.highUs, a.lowUs);
    Serial.println(PB);
  }
  delay(2000);
}

