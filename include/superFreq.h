#ifndef __SUPER_FREQ__
#define __SUPER_FREQ__

#include <Arduino.h>

typedef struct {
  bool state;
  uint32_t delta;
} superFreqEdge;

template <size_t N, typename T, typename I>
class superFreqRingBuffer {
public:
  void insert(T);
  bool isFull();
  bool isEmpty();
  T getAvg();
  T read();
  I available();
#ifdef SUPER_FREQ_DEBUG_SERIAL
  void print();
#endif

private:
  T calcAvg();
  void advance();

  T buffer[N] = {0};
  I h = 0; /* head pointer */
  I t = 0; /* tail pointer */
  I r = 0; /* read pointer */
  T avg = 0; /* cache for averaged value */
  I chk = 0; /* checksum at the point the cache was generated */
};

template <size_t N, typename I>
class superFreq {
public:
  void update(bool);
  bool isFull();
  float getFreq();
  float getPulseWidth();
  uint32_t getPeriod();
  uint32_t getHighPeriod();
  uint32_t getLowPeriod();
  I available();
  superFreqEdge readEdge();
#ifdef SUPER_FREQ_DEBUG_SERIAL
  void print();
#endif

private:
  superFreqRingBuffer<N, uint32_t, I> periods;     /* buffer of periods H->H */
  superFreqRingBuffer<N, uint32_t, I> highPeriods; /* buffer of periods H->L */
  uint32_t lastHigh = 0;
};

template <size_t N, typename I>
void superFreq<N,I>::update(bool state) {
  uint32_t m = micros();
  uint32_t p = m - lastHigh;

  switch (state) {
    case true:
      periods.insert(p);
      lastHigh = m;
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
  return (float)getHighPeriod() / (float)getPeriod();

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

#ifdef SUPER_FREQ_DEBUG_SERIAL
template <size_t N, typename I>
void superFreq<N,I>::print() {
  periods.print();
  //highPeriods.print();
}
#endif

template <size_t N, typename T, typename I>
void superFreqRingBuffer<N,T,I>::insert(T value) {
  buffer[h] = value;
  advance();
}

template <size_t N, typename T, typename I>
T superFreqRingBuffer<N,T,I>::getAvg() {
  I checksum = h ^ t;
  if(checksum != chk) {
#ifdef SUPER_FREQ_DEBUG_SERIAL
    Serial.println("Recalculating...");
#endif
    avg = calcAvg();
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
  I start = t;

  while ((start % N) != h) {
#ifdef SUPER_FREQ_DEBUG_SERIAL
    Serial.print(count);
    Serial.print(' ');
    Serial.print(start);
    Serial.print(' ');
    Serial.println(start % N);
#endif

    sum += buffer[start % N];
    count++;
    start++;
  }

  return ((sum << 3) / count) >> 3;
}

template <size_t N, typename T, typename I>
T superFreqRingBuffer<N,T,I>::read() {

}

template <size_t N, typename T, typename I>
I superFreqRingBuffer<N,T,I>::available() {
  return (I)(((int32_t)h - (int32_t)t) % (int32_t)N);
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
    if (t + 1 > r) {
      r = (r + 1) % N; /* advance read if tail has caught up */
    }
    t = (t + 1) % N; /* advance tail if full */
  }
  h = (h + 1) % N; /* advance head */
}

#ifdef SUPER_FREQ_DEBUG_SERIAL
template <size_t N, typename T, typename I>
void superFreqRingBuffer<N,T,I>::print() {
  for (I i=0; i<N; i++) {
    if (i == t) Serial.print("T(");
    if (i == h) Serial.print("H(");
    if (i == r) Serial.print("R(");
    Serial.print(buffer[i]);
    if (i == t || i == h || i == r) Serial.print(")");
    Serial.print(' ');
  }
  Serial.print("\r\n");
}
#endif

#endif // __SUPER_FREQ__
