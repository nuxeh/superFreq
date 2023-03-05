#include "superFreq.h"

template <size_t N>
void superFreq<N>::update(bool state) {
  uint32_t m = micros();
  uint32_t p = m - lastHigh;
  lastHigh = m;

  switch (state) {
    case true:
      highPeriods.insert(p);
      break;
    case false:
      lowPeriods.insert(p);
      break;
  }
}


template <size_t N>
float superFreq<N>::getFreq() {

}

template <size_t N>
float superFreq<N>::getPulseWidth() {

}

template <size_t N>
uint32_t superFreq<N>::getPeriod() {
  return highPeriods.getAvg();
}

template <size_t N>
uint32_t superFreq<N>::getHighPeriod() {

}

template <size_t N>
uint32_t superFreq<N>::getLowPeriod() {
  return lowPeriods.getAvg();
}

