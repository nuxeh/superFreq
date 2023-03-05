#include "superFreq.h"

template <size_t N, typename T, typename I>
void superFreqRingBuffer<N,T,I>::insert(T value) {
  buffer[h] = value;
  advance();
}

template <size_t N, typename T, typename I>
T superFreqRingBuffer<N,T,I>::getAvg() {
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
T superFreqRingBuffer<N,T,I>::calcAvg() {
  uint32_t sum = 0;
  I count = 0;
  I start = (I)(((int32_t)h - (int32_t)t) % (int32_t)N);

  while ((start++ % N) != h) {
    sum += buffer[(start + count++) % N];
  }

  return ((sum << 3) / count) >> 3;
}

template <size_t N, typename T, typename I>
T superFreqRingBuffer<N,T,I>::read() {

}

template <size_t N, typename T, typename I>
I superFreqRingBuffer<N,T,I>::available() {
  int32_t delta = ((uint32_t)h - (uint32_t)t) % N;
  return 

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

#ifdef SUPER_FREQ_DEBUG
template <size_t N, typename T, typename I>
void superFreqRingBuffer<N,T,I>::print() {
  for (I i=0; i<N; i++) {
    if (i == t) Serial.print("T(");
    if (i == h) Serial.print("H(");
    if (i == r) Serial.print("R(");
    Serial.print(buffer[i]);
    Serial.print(' ');
    if (i == t || i == h || i == r) Serial.print(")");
  }
}
#endif
