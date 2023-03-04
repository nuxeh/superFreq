#include "superFreq.h"

template <class N>
void superFreq<N>::update(bool state) {
template <class N>
  switch (state) {
    case true:
      lastUsHigh = micros();
      break;
    case false:
      lastUsLow = micros();
      break;
  }
}


template <class N>
float superFreq<N>::getFreq() {

}

template <class N>
float superFreq<N>::getPulseWidth() {

}

template <class N>
uint32_t superFreq<N>::getPeriod() {

}

template <class N>
uint32_t superFreq<N>::getHighPeriod() {

}

template <class N>
uint32_t superFreq<N>::getLowPeriod() {

}

