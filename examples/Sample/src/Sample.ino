/*
 * Update superFreq with a sampling strategy. This avoids noise and bounce if
 * set at a slow enough sample rate - or updating superFreq at a carefully
 * chosen rate to avoid bounce.
 */

#include <Arduino.h>
#include <TimerOne.h>

#define SUPER_FREQ_CYCLE_DEBUG_SERIAL
#include <superFreq.h>

#define PIN 13
#define MASK 0b11000111

superFreq<4> sf;

char PB[78] = {0};

void readInput(){ sf.update(digitalRead(PIN)); }

void setup() {
  Serial.begin(115200);
  pinMode(PIN, INPUT);
  Timer1.initialize(15000);
  Timer1.attachInterrupt(readInput);
}

void loop() {
  if (sf.available() > 0) {
    superFreqCycle a = sf.getAvg();
    a.print();
    Serial.println();
    sprintf(PB, "T=%lu F=%.2f PW=%.2f TH=%lu TL=%lu R=%d",
            a.getPeriod(), a.getFreq(), a.getDutyCycle(),
            a.highUs, a.lowUs, sf.isRunning());
    Serial.println(PB);
    sf.flush();
  }
  delay(2000);
}


