/*
 * trigger user defined callbacks when the superFreq debounce has determined a
 * rising or falling edge
 */

#include <Arduino.h>
#include <TimerOne.h>
#include <superFreq.h>

#define PIN 13

superFreqDebounceCallback<4> sf;

char PB[78] = {0};

void readInput() {
  sf.update(digitalRead(PIN));
}

void asserted() {
  Serial.println(">> asserted");
}

void deasserted() {
  Serial.println(">> deasserted");
}

void setup() {
  Serial.begin(115200);
  Serial.println("start");

  sf.attachCallback(superFreqCallback::Asserted, asserted); 
  sf.attachCallback(superFreqCallback::Deasserted, deasserted); 

  pinMode(PIN, INPUT);
  Timer1.initialize(5000);
  Timer1.attachInterrupt(readInput);

  delay(1000);
}

void loop() {
  if (sf.available()) {
    superFreqCycle a = sf.getAvg();
    sprintf(PB, "T=%lu F=%.2f PW=%.2f TH=%lu TL=%lu",
            a.getPeriod(), a.getFreq(), a.getDutyCycle(),
            a.highUs, a.lowUs);
    Serial.println(PB);
  }
  delay(2000);
}

