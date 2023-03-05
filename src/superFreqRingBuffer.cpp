#include "superFreq.h"

template <size_t N, typename T, typename I>
void superFreqRingBuffer<N,T,I>::insert(T value) {
  
}

template <size_t N, typename T, typename I>
bool superFreqRingBuffer<N,T,I>::isFull() {
  return (h + 1 == t);
}

template <size_t N, typename T, typename I>
bool superFreqRingBuffer<N,T,I>::isEmpty() {
  return (h == t);
}

template <size_t N, typename T, typename I>
T superFreqRingBuffer<N,T,I>::getAvg() {
  I checksum = h ^ t;
  if(checksum != chk) {
    avg = calcAvg();
    chk = checksum;
  }
  return avg;
}

template <size_t N, typename T, typename I>
T superFreqRingBuffer<N,T,I>::calcAvg() {

}

template <size_t N, typename T, typename I>
T superFreqRingBuffer<N,T,I>::read() {

}

template <size_t N, typename T, typename I>
I superFreqRingBuffer<N,T,I>::available() {

}
