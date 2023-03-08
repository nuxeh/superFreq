#include <Arduino.h>
#include <TimerOne.h>
#include <superFreq.h>

superFreqMonitor<superFreq<4>> sf;

volatile bool state = false;
char PB[78] = {0};

void update() {
  sf.update(state);
  state = !state;
}

void setup() {
  Serial.begin(115200);
  Timer1.initialize(150000);
  Timer1.attachInterrupt(update);
}

void loop() {
  sf.tick();
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
