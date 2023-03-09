#ifndef __SUPER_FREQ__
#define __SUPER_FREQ__

#include <Arduino.h>

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
  static_assert((N && (N & (N - 1)) == 0), "template parameter N must be a power of two!");
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
  T sum = 0;
  uint8_t count = 0;
  uint8_t start = t;

  while ((start % N) != h) {
#ifdef SUPER_FREQ_DEBUG_SERIAL
    Serial.print(count);
    Serial.print(' ');
    Serial.print(start);
    Serial.print(' ');
    Serial.print(start % N);
    Serial.print(" = ");
    Serial.println(buffer[start % N]);
#endif

    sum += buffer[start % N];
    count++;
    start++;
  }

#ifdef SUPER_FREQ_DEBUG_SERIAL
  Serial.println(sum);
  Serial.println(count);
#endif

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
  void high() { update(true); }
  void low() { update(false); }
  void update(bool);
  uint8_t available();
  superFreqCycle getAvg(); // TODO: rename
  void flush();
  int getPeriods(int n, uint32_t *);
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

#ifdef SUPER_FREQ_DEBUG_SERIAL
  Serial.println(p);
#endif

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
#ifdef SUPER_FREQ_DEBUG_SERIAL
  Serial.print("us: ");
  Serial.print(us);
#endif
  uint32_t highUs = highPeriods.getAvg();
#ifdef SUPER_FREQ_DEBUG_SERIAL
  Serial.print(" highUs: ");
  Serial.println(highUs);
#endif
  return superFreqCycle(highUs, us - highUs);
}

#ifdef SUPER_FREQ_DEBUG_SERIAL
template <size_t N>
void superFreq<N>::print() {
  periods.print();
}
#endif

const uint8_t MASK = 0b11000111;

template <size_t N>
struct superFreqDebounce : public superFreq<N> {
  void high() { update(true); }
  void low() { update(false); }

  void update(bool state) {
    advance(state);
    if (asserted()) { superFreq<N>::update(true); }
    if (deasserted()) { superFreq<N>::update(false); }
  }

  uint8_t asserted() {
    uint8_t asserted = 0;
    if ((history & MASK) == 0b00000111) {
      asserted = 1;
      history = 0xFF;
    }
    return asserted;
  }

  uint8_t deasserted(){
    uint8_t deasserted = 0;
    if ((history & MASK) == 0b11000000) {
      deasserted = 1;
      history = 0x00;
    }
    return deasserted;
  }

protected:
  void advance(bool state) {
    history <<= 1;
    history |= state;
  }

private:
  uint8_t history = 0;
};

typedef enum {
  Start = 0,
  Stop,
  Asserted,
  Deasserted,
} superFreqCallback;

template <size_t N>
struct superFreqDebounceCallback : public superFreqDebounce<N> {
  void high() { update(true); }
  void low() { update(false); }

  void attachCallback(superFreqCallback c, void (*fn)(void)) {
    switch (c) {
      case superFreqCallback::Asserted:
        assertedFn = fn;
        break;
      case superFreqCallback::Deasserted:
        deassertedFn = fn;
        break;
      default:
        break;
    }
  }

  void update(bool state) {
    superFreqDebounce<N>::advance(state);
    if (superFreqDebounce<N>::asserted()) {
      superFreq<N>::update(true);
      if (assertedFn != NULL) {
        assertedFn();
      }
    }
    if (superFreqDebounce<N>::deasserted()) {
      superFreq<N>::update(false);
      if (deassertedFn != NULL) {
        deassertedFn();
      }
    }
  }

private:
  void (*assertedFn)() = NULL;
  void (*deassertedFn)() = NULL;
};

template <typename T>
struct superFreqMonitor {
  superFreqMonitor(T& sf) : sf(sf) {}

  bool isRunning() { return state; }
  superFreqCycle getAvgCycle() { return avg; }

  /* perform maintenance */
  void tick() {
    uint32_t t = micros();
    uint32_t avgPeriod2 = avg.getPeriod() << 1;

    if (sf.available() > 0) {
      /* we have edges */
      avg = sf.getAvg();
      process();
      state = true;
      lastUpdate = t;
    } else if (state && (t - lastUpdate > avgPeriod2)) {
      /* two periods have elapsed without edges */
      state = false;
    }
  }

protected:
  T& sf; /* superFreq object */
  bool state = false; /* running state */

private:
  void process() {
    sf.flush();
  }

  superFreqCycle avg; /* cached average cycle */
  uint32_t lastUpdate = 0; /* time last new edges received */
};

template <typename T, size_t N>
struct superFreqMonitorCycleCache : public superFreqMonitor<T> {
  void process() {
    while (superFreqMonitor<T>::sf.available()) {
      buffer.insert(superFreqMonitor<T>::sf.read);
    }
  }

  uint8_t available() {
    return buffer.available();
  }

  superFreqCycle read() {
    return buffer.read();
  }

private:
  superFreqRingBuffer<N, superFreqCallback> buffer;
};

template <typename T>
struct superFreqMonitorCallback : public superFreqMonitor<T> {
  superFreqMonitorCallback(T& sf) : superFreqMonitor<T>::sf(sf) {}

  void attachCallback(superFreqCallback c, void (*fn)()) {
    switch (c) {
      case superFreqCallback::Start:
        startFn = fn;
        break;
      case superFreqCallback::Stop:
        stopFn = fn;
        break;
    }
  }

  void attachStateChangeCallback(void (*fn)(bool)) {
    stateChangeFn = fn;
  }

  void tick() {
    bool lastState = superFreqMonitor<T>::state;
    superFreqMonitor<T>::tick();
    bool newState = superFreqMonitor<T>::state;

    /* call callbacks when state has changed */
    if (newState != lastState) {
      if (stateChangeFn != NULL) {
        stateChangeFn(newState);
      }
      switch (newState) {
        case true:
          if (startFn != NULL) {
            startFn();
          }
          break;
        case false:
          if (stopFn != NULL) {
            stopFn();
          }
          break;
      }
    }
  }

private:
  void (*startFn)() = NULL;
  void (*stopFn)() = NULL;
  void (*stateChangeFn)(bool) = NULL;
};

//TODO: edge-cacheing class

#endif // __SUPER_FREQ__
