#include "superFreq.h"

template <size_t N, typename I>
void superFreq<N>::update(bool state) {
  uint32_t m = micros();
  uint32_t p = m - lastHigh;
  lastHigh = m;

  switch (state) {
    case true:
      periods.insert(p);
      break;
    case false:
      highPeriods.insert(p);
      break;
  }
}

template <size_t N, typename I>
float superFreq<N>::getFreq() {
  return 1000000.0 / (float)getPeriod();

}

template <size_t N, typename I>
float superFreq<N>::getPulseWidth() {
  return getHighPeriod() / (float)getLowPeriod();

}

template <size_t N, typename I>
uint32_t superFreq<N>::getPeriod() {
  return periods.getAvg();
}

template <size_t N, typename I>
uint32_t superFreq<N>::getHighPeriod() {
  return highPeriods.getAvg();
}

template <size_t N, typename I>
uint32_t superFreq<N>::getLowPeriod() {
  return periods.getAvg() - highPeriods.getAvg();
}

template <size_t N, typename I>
I superFreq<N>::available() {

}

template <size_t N, typename I>
superFreqEdge superFreq<N>::readEdge() {

}
