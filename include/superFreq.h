#ifndef __SUPER_FREQ__
#define __SUPER_FREQ__

#include <Arduino.h>

#include "callback.h"
#include "cycle.h"
#include "ringbuf.h"

#define SPP_SHIFT 1

template <size_t N>
struct superFreq : public superFreqCallback {
  void high() { update(true); }
  void low() { update(false); }
  bool isRunning() { return running; }
  int getPeriods(int n, uint32_t *);
  bool isFull();

  void update(bool state) {
    /* state has changed since last sample */
    if (lastState != state) {
      //Serial.print("num samples=");
      //Serial.println(numSamples);
      uint32_t m = micros();
      uint32_t p = m - lastHigh;

      switch (state) {
        /* high */
        case true:
          periods.insert(p);
          lastHigh = m;
          timeoutSamples = numSamples << SPP_SHIFT;
          if (!running) runCallback(CallbackEvent::Started);
          running = true;
          numSamples = 0;
          break;
        /* low */
        case false:
          highPeriods.insert(p);
          break;
      }
    }
    registerSample();
    lastState = state;
  }


  uint8_t available() {
    // highPeriods... TODO
    return periods.available();
  }

  void flush() {
    periods.flush();
    highPeriods.flush();
  }

  superFreqCycle getAvg() { // Rename to getAvgCycle() TODO
    uint32_t us = periods.getAvg();
    uint32_t highUs = highPeriods.getAvg();
    return superFreqCycle(highUs, us - highUs);
  }

#ifdef SUPER_FREQ_DEBUG_SERIAL
  void print() {
    periods.print();
    Serial.print(" | ");
    highPeriods.print();
    Serial.print("\r\n");
  }
#endif

protected:
  void registerSample() {
    numSamples++;
    if (running && numSamples > timeoutSamples) {
      running = false;
      runCallback(CallbackEvent::Stopped);
    }
  }

private:
  superFreqRingBuffer<N, uint32_t> periods;     /* buffer of periods H->H */
  superFreqRingBuffer<N, uint32_t> highPeriods; /* buffer of periods H->L */
  uint32_t lastHigh = 0;       /* us elapsed since last high */
  bool lastState = false;      /* signal state on last processed sample */
  uint16_t numSamples = 0;     /* sample counter */
  uint16_t timeoutSamples = 0; /* sample count to time out at */
  bool running = false;        /* true indicates signal presence has been detected */
};

const uint8_t MASK = 0b11000111;

template <size_t N>
struct superFreqDebounce : public superFreq<N> {
  void high() { update(true); }
  void low() { update(false); }

  void update(bool state) {
    advance(state);
    if (asserted()) {
      superFreq<N>::update(true);
      superFreq<N>::runCallback(CallbackEvent::Asserted);
    }
    else if (deasserted()) {
      superFreq<N>::update(false);
      superFreq<N>::runCallback(CallbackEvent::Deasserted);
    }
    /* we must still count each sample for presence detection */
    else { superFreq<N>::registerSample(); }
  }

protected:
  uint8_t asserted() {
    uint8_t asserted = 0;
    if ((history & MASK) == 0b00000111) {
      asserted = 1;
      history = 0xFF;
    }
    return asserted;
  }

  uint8_t deasserted(){
    uint8_t deasserted = 0;
    if ((history & MASK) == 0b11000000) {
      deasserted = 1;
      history = 0x00;
    }
    return deasserted;
  }

  void advance(bool state) {
    history <<= 1;
    history |= state;
  }

private:
  uint8_t history = 0;
};

#endif // __SUPER_FREQ__
