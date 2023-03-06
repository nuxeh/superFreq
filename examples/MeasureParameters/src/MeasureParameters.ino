#include <Arduino.h>
#include <TimerOne.h>

//#define SUPER_FREQ_DEBUG_SERIAL
#include "superFreq.h"

superFreq<5, uint8_t> sf;

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
#ifdef SUPER_FREQ_DEBUG_SERIAL
  sf.print();
#endif
  sprintf(PB, "T=%lu F=%.2f PW=%.2f TH=%lu TL=%lu",
          sf.getPeriod(), sf.getFreq(), sf.getPulseWidth(),
          sf.getHighPeriod(), sf.getLowPeriod());
  Serial.println(PB);
  delay(101);
}
