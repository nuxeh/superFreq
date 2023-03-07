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

  superFreqCycle() : highUs(0), lowUs(0) {}
  superFreqCycle(uint32_t highUs, uint32_t lowUs) : highUs(highUs), lowUs(lowUs) {}

  float getFreq() {
    return 1000000.0 / (float)getPeriod();
  }

  uint32_t getPeriod() {
    return highUs + lowUs;
  }

  float getDutyCycle() {
    return (float)highUs / (float)getPeriod();
  }

#ifdef SUPER_FREQ_CYCLE_DEBUG_SERIAL
  void print() {
    Serial.print("superFreqCycle { .highUs: ");
    Serial.print(highUs);
    Serial.print(", .lowUs: ");
    Serial.print(lowUs);
    Serial.print(" }");
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
    return *this;
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

template <size_t N, typename T>
struct superFreqRingBuffer {
  void insert(T);
  bool isFull();
  bool isEmpty();
  T getAvg();
  void flush();
  T read();
  uint8_t available();
#ifdef SUPER_FREQ_DEBUG_SERIAL
  void print();
#endif

private:
  T calcAvg();
  void advance();

  T buffer[N] = {0};
  uint8_t h = 0; /* head pointer */
  uint8_t t = 0; /* tail pointer */
  uint8_t r = 0; /* read pointer */
  T avg; /* cache for averaged value */
  uint8_t chk = 0; /* checksum at the point the cache was generated */
};


template <size_t N, typename T>
void superFreqRingBuffer<N,T>::insert(T value) {
  buffer[h] = value;
  advance();
}

template <size_t N, typename T>
T superFreqRingBuffer<N,T>::getAvg() {
  uint8_t checksum = h ^ t;
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
template <size_t N, typename T>
T superFreqRingBuffer<N,T>::calcAvg() {
  T sum;
  uint8_t count = 0;
  uint8_t start = t;

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

template <size_t N, typename T>
T superFreqRingBuffer<N,T>::read() {
  T ret = buffer[r];
  r = (r + 1) % N;
  return ret;
}

template <size_t N, typename T>
uint8_t superFreqRingBuffer<N,T>::available() {
  return (h >= r)
         ? h - r
         : N - r + h;
}

template <size_t N, typename T>
bool superFreqRingBuffer<N,T>::isFull() {
  return ((h + 1) % N == t);
}

template <size_t N, typename T>
bool superFreqRingBuffer<N,T>::isEmpty() {
  return (h == t);
}

template <size_t N, typename T>
void superFreqRingBuffer<N,T>::flush() {
  r = h;
}

template <size_t N, typename T>
void superFreqRingBuffer<N,T>::advance() {
  if(isFull()) {
    if (t + 1 > r) {
      r = (r + 1) % N; /* advance read if tail has caught up */
    }
    t = (t + 1) % N; /* advance tail if full */
  }
  h = (h + 1) % N; /* advance head */
}

#ifdef SUPER_FREQ_DEBUG_SERIAL
template <size_t N, typename T>
void superFreqRingBuffer<N,T>::print() {
  for (uint8_t i=0; i<N; i++) {
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

template <size_t N>
struct superFreq {
  void update(bool);
  uint8_t available();
  void flush();
  superFreqCycle getAvg();
  int getPeriods(uint32_t *);
  bool isFull();
#ifdef SUPER_FREQ_DEBUG_SERIAL
  void print();
#endif

private:
  superFreqRingBuffer<N, uint32_t> periods;     /* buffer of periods H->H */
  superFreqRingBuffer<N, uint32_t> highPeriods; /* buffer of periods H->L */
  uint32_t lastHigh = 0;
  bool lastState = false;
  bool locked = false;
};

template <size_t N>
void superFreq<N>::update(bool state) {
  if (lastState == state) {
    return;
  }

  uint32_t m = micros();
  uint32_t p = m - lastHigh;

  switch (state) {
    /* high */
    case true:
      periods.insert(p);
      lastHigh = m;
      break;
    /* low */
    case false:
      highPeriods.insert(p);
      break;
  }

  lastState = state;
}


template <size_t N>
uint8_t superFreq<N>::available() {
  return periods.available();
}

template <size_t N>
void superFreq<N>::flush() {
  periods.flush();
  highPeriods.flush();
}

template <size_t N>
superFreqCycle superFreq<N>::getAvg() {
  uint32_t us = periods.getAvg();
  Serial.print("us: ");
  Serial.print(us);
  uint32_t highUs = highPeriods.getAvg();
  Serial.print(" highUs: ");
  Serial.println(highUs);
  return superFreqCycle(highUs, us - highUs);
}

#ifdef SUPER_FREQ_DEBUG_SERIAL
template <size_t N>
void superFreq<N>::print() {
  periods.print();
}
#endif

#endif // __SUPER_FREQ__
