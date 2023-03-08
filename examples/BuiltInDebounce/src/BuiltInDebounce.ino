/*
 * Update superFreq with a sampling strategy, with software debouncing provided
 * by the library. It takes a few non-transition samples to trigger. The update
 * function must simply be called regularly enough to trade off noise tolerance
 * and response time. Here, update is called by a timer every 5 milliseconds.
 */

#include <Arduino.h>
#include <TimerOne.h>
#include <superFreq.h>

#define PIN 13
superFreqDebounce<4> sf;

char PB[78] = {0};

void update(){
  sf.update(digitalRead(PIN));
}

void setup() {
  Serial.begin(115200);
  pinMode(PIN, INPUT);
  Timer1.initialize(5000);
  Timer1.attachInterrupt(update);
}

void loop() {
  if (sf.available() > 0) {
    superFreqCycle a = sf.getAvg();
    sprintf(PB, "T=%lu F=%.2f PW=%.2f TH=%lu TL=%lu",
            a.getPeriod(), a.getFreq(), a.getDutyCycle(),
            a.highUs, a.lowUs);
    Serial.println(PB);
    sf.flush();
  }
  delay(2000);
}


