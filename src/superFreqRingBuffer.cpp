#include "superFreq.h"

template <size_t N, typename T, typename I>
bool superFreqRingBuffer<N,T,I>::insert(T value) {
  
}

template <size_t N, typename T, typename I>
bool superFreqRingBuffer<N,T,I>::isFull() {
  return (head + 1 == tail);
}

template <size_t N, typename T, typename I>
bool superFreqRingBuffer<N,T,I>::isEmpty() {

}

template <size_t N, typename T, typename I>
T superFreqRingBuffer<N,T,I>::getAvg() {
  I checksum = head ^ tail;
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
I superFreqRingBuffer<N,T,I>::read() {

}

template <size_t N, typename T, typename I>
I superFreqRingBuffer<N,T,I>::available() {

}
