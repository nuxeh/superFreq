#include "superFreq.h"

template <uint8_t N>
void superFreq<N>::update(bool state) {
  switch (state) {
    case true:
      high[highIndex] = micros();
      highIndex = (lowIndex + 1) % N;
      break;
    case false:
      low[lowIndex] = micros();
      lowIndex = (lowIndex + 1) % N;
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

}

template <uint8_t N>
uint32_t superFreq<N>::getHighPeriod() {

}

template <uint8_t N>
uint32_t superFreq<N>::getLowPeriod() {

}

