/*
 * Update superFreq with a sampling strategy, with software debouncing
 */

#include <Arduino.h>
#include <TimerOne.h>

//#define SUPER_FREQ_DEBUG_SERIAL
#define SUPER_FREQ_CYCLE_DEBUG_SERIAL
#include <superFreq.h>

#define PIN 13
#define MASK 0b11000111

superFreq<4> sf;

char PB[78] = {0};
uint8_t history = 0;

uint8_t buttonPressed(){
  uint8_t asserted = 0;
  if ((history & MASK) == 0b00000111) {
    asserted = 1;
    history = 0b11111111;
  }
  return asserted;
}

uint8_t buttonReleased(){
  uint8_t released = 0;
  if ((history & MASK) == 0b11000000) {
    released = 1;
    history = 0b00000000;
  }
  return released;
}

void update(){
  history = history << 1;
  history |= digitalRead(PIN);
  if (buttonPressed()) sf.high();
  if (buttonReleased()) sf.low();
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

