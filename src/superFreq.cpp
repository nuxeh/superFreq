#include "superFreq.h"

template <size_t N, typename I>
void superFreq<N,I>::update(bool state) {
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
float superFreq<N,I>::getFreq() {
  return 1000000.0 / (float)getPeriod();

}

template <size_t N, typename I>
float superFreq<N,I>::getPulseWidth() {
  return (float)getHighPeriod() / (float)getLowPeriod();

}

template <size_t N, typename I>
uint32_t superFreq<N,I>::getPeriod() {
  return periods.getAvg();
}

template <size_t N, typename I>
uint32_t superFreq<N,I>::getHighPeriod() {
  return highPeriods.getAvg();
}

template <size_t N, typename I>
uint32_t superFreq<N,I>::getLowPeriod() {
  return periods.getAvg() - highPeriods.getAvg();
}

template <size_t N, typename I>
I superFreq<N,I>::available() {

}

template <size_t N, typename I>
superFreqEdge superFreq<N,I>::readEdge() {

}

#ifdef SUPER_FREQ_DEBUG
template <size_t N, typename I>
void superFreq<N,I>::print() {
  periods.print();
  highPeriods.print();
}
#endif
