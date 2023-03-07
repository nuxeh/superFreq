#ifndef __SUPER_FREQ__
#define __SUPER_FREQ__

#include <Arduino.h>

typedef struct {
  bool state;
  uint32_t delta;
} superFreqEdge;

struct superFreqCycle {
  uint32_t highUs;
  uint32_t lowUs;

  float getFreq() {
    return 1000000.0 / (float)getPeriod();
  }

  uint32_t getPeriod() {
    return highUs + lowUs;
  }

  float getDutyCycle() {
    return (float)highUs / (float)getPeriod();
  }

#ifdef SUPER_FREQ_DEBUG_SERIAL
  void print() {
    Serial.print("superFreqCycle { .highUs: ");
    Serial.print(highUs);
    Serial.print(", .lowUs: ");
    Serial.print(lowUs);
    Serial.print(" }\r\n");
  }
#endif

  superFreqCycle& operator=(const superFreqCycle &rhs) {
    if (this != &rhs) {
      highUs = rhs.highUs;
      lowUs = rhs.lowUs;
    }
    return *this;
  }

  superFreqCycle& operator=(const int &rhs) {
    highUs = rhs;
    lowUs = rhs;
  }

  superFreqCycle& operator+=(const superFreqCycle &rhs) {
    highUs += rhs.highUs;
    lowUs += rhs.lowUs;
    return *this;
  }

  superFreqCycle& operator/=(const uint32_t &rhs) {
    highUs /= rhs;
    lowUs /= rhs;
    return *this;
  }

  superFreqCycle& operator<<=(const uint32_t &rhs) {
    highUs <<= rhs;
    lowUs <<= rhs;
    return *this;
  }

  superFreqCycle& operator>>=(const uint32_t &rhs) {
    highUs >>= rhs;
    lowUs >>= rhs;
    return *this;
  }

  const superFreqCycle operator+(const superFreqCycle &other) const {
    return superFreqCycle(*this) += other;
  }

  const superFreqCycle operator/(uint32_t rhs) const {
    return superFreqCycle(*this) /= rhs;
  }

  const superFreqCycle operator<<(uint32_t rhs) const {
    return superFreqCycle(*this) <<= rhs;
  }

  const superFreqCycle operator>>(uint32_t rhs) const {
    return superFreqCycle(*this) >>= rhs;
  }
};

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
  T sum = 0;
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

template <size_t N, typename I>
class superFreq {
public:
  void update(bool);
  bool isFull();
  I available();
  superFreqCycle readCycle();
#ifdef SUPER_FREQ_DEBUG_SERIAL
  void print();
#endif

private:
  superFreqRingBuffer<N, superFreqCycle, I> cycles;
  uint32_t lastHigh = 0;
  bool lastState = false;
  bool locked = false;
};

template <size_t N, typename I>
void superFreq<N,I>::update(bool state) {
  if (lastState == state) {
    return;
  }

  uint32_t m = micros();
  uint32_t p = m - lastHigh;

  switch (state) {
    case true:
      cycles.insert(p);
      lastHigh = m;
      break;
    case false:
      cycles.insert(p);
      break;
  }

  lastState = state;
}


template <size_t N, typename I>
I superFreq<N,I>::available() {
  return cycles.available();
}

template <size_t N, typename I>
superFreqCycle superFreq<N,I>::readCycle() {

}

#ifdef SUPER_FREQ_DEBUG_SERIAL
template <size_t N, typename I>
void superFreq<N,I>::print() {
  cycles.print();
}
#endif

#endif // __SUPER_FREQ__
