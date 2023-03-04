#include "superFreq.h"

template <uint8_t N>
void superFreq<N>::update(bool state) {
  switch (state) {
    case true:
      high[highHead] = micros();
      highHead = (lowHead + 1) % N;
      break;
    case false:
      low[lowHead] = micros();
      lowHead = (lowHead + 1) % N;
      break;
  }
}


template <uint8_t N>
float superFreq<N>::getFreq() {

}

template <uint8_t N>
float superFreq<N>::getPulseWidth() {

}

template <uint8_t N>
uint32_t superFreq<N>::getPeriod() {
  if (highTail != highHead || isFull) {

  }
}

template <uint8_t N>
uint32_t superFreq<N>::getHighPeriod() {

}

template <uint8_t N>
uint32_t superFreq<N>::getLowPeriod() {

}

