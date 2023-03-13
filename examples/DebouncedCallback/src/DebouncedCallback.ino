/*
 * trigger user defined callbacks when the superFreq debounce has determined a
 * rising or falling edge
 */

#include <Arduino.h>
#include <TimerOne.h>
#include <superFreq.h>

#define PIN 13

superFreqDebounceCallback<4> sf;
superFreqCallbackHandler sch;

char PB[78] = {0};

void readInput() { sf.update(digitalRead(PIN)); }
void started() { Serial.println(">>>> started <<<<"); }
void stopped() { Serial.println(">>>> stopped <<<<"); }
void asserted() { Serial.println(">> asserted <<"); }
void deasserted() { Serial.println(">> deasserted <<"); }

void setup() {
  Serial.begin(115200);
  Serial.println("start");

  sch.attachCallback(CallbackEvent::Asserted, asserted);
  sch.attachCallback(CallbackEvent::Deasserted, deasserted);
  sch.attachCallback(CallbackEvent::Started, started);
  sch.attachCallback(CallbackEvent::Stopped, stopped);
  sf.attachCallbackHandler(&sch);

  pinMode(PIN, INPUT);

  Timer1.initialize(5000);
  Timer1.attachInterrupt(readInput);
}

void loop() {
  if (sf.available()) {
    superFreqCycle a = sf.getAvg();
    sprintf(PB, "T=%lu F=%.2f PW=%.2f TH=%lu TL=%lu R=%d",
            a.getPeriod(), a.getFreq(), a.getDutyCycle(),
            a.highUs, a.lowUs, sf.isRunning());
    Serial.println(PB);
  }
  delay(2000);
}

