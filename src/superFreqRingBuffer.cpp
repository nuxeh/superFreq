#include "superFreq.h"

template <size_t N, typename T, typename I>
void superFreqRingBuffer<N,T,I>::insert(T value) {
  buffer[h] = value;
  advance();
}

template <size_t N, typename T, typename I>
T superFreqRingBuffer<N,T,I>::getAvg(I n) {
  I checksum = h ^ t;
  if(checksum != chk) {
    avg = calcAvg(n);
    chk = checksum;
  }
  return avg;
}

/* calculate an average of last n period values
 *
 * start from oldest sample to avoid data changing in interrupt as much as
 * possible
 */
template <size_t N, typename T, typename I>
T superFreqRingBuffer<N,T,I>::calcAvg(I n) {

}

template <size_t N, typename T, typename I>
T superFreqRingBuffer<N,T,I>::read() {

}

template <size_t N, typename T, typename I>
I superFreqRingBuffer<N,T,I>::available() {

}

template <size_t N, typename T, typename I>
bool superFreqRingBuffer<N,T,I>::isFull() {
  return ((h + 1) % N == t);
}

template <size_t N, typename T, typename I>
bool superFreqRingBuffer<N,T,I>::isEmpty() {
  return (h == t);
}

template <size_t N, typename T, typename I>
void superFreqRingBuffer<N,T,I>::advance() {
  if(isFull()) {
		t = (t + 1) % N; /* advance tail if full */
    if (t > r) {
		  r = (r + 1) % N; /* advance read if tail has caught up */
    }
	}
	h = (h + 1) % N; /* advance head */
}
