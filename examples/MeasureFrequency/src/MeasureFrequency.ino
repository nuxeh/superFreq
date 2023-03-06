#include <Arduino.h>
#include <TimerOne.h>

//#define SUPER_FREQ_DEBUG_SERIAL
#include "superFreq.h"

superFreq<5, uint8_t> sf;

volatile bool state = false;

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
  Serial.println(sf.getFreq());
  delay(100);
}
