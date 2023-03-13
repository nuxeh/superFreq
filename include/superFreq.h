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
#if 0
    Serial.println("assignment called");
    print();
    Serial.println();
    rhs.print();
    Serial.println();
#endif
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
#ifdef SUPER_FREQ_RB_DEBUG_SERIAL
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
#ifdef SUPER_FREQ_DEBUG_SERIAL
  print();
#endif
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

#ifdef SUPER_FREQ_RB_DEBUG_SERIAL
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
  //Serial.print("\r\n");
}
#endif

#define SPP_SHIFT 2

template <size_t N>
struct superFreq {
  void high() { update(true); }
  void low() { update(false); }
  bool isRunning() { return running; }
  int getPeriods(int n, uint32_t *);
  bool isFull();

  void update(bool state) {
    /* state has changed since last sample */
    if (lastState != state) {
      lastState = state;
      Serial.print("num samples=");
      Serial.println(numSamples);
      uint32_t m = micros();
      uint32_t p = m - lastHigh;

      switch (state) {
        /* high */
        case true:
          Serial.println("HIGH");
          periods.insert(p);
          lastHigh = m;
          timeoutSamples = numSamples << SPP_SHIFT;
          numSamples = 0;
          running = true;
          break;
        /* low */
        case false:
          Serial.println("LOW");
          highPeriods.insert(p);
          break;
      }
    }
    if (numSamples > timeoutSamples) {
      running = false;
    }
    numSamples++;
  }


  uint8_t available() {
    // highPeriods... TODO
    return periods.available();
  }

  void flush() {
    periods.flush();
    highPeriods.flush();
  }

  superFreqCycle getAvg() { // Rename to getAvgCycle() TODO
    uint32_t us = periods.getAvg();
    uint32_t highUs = highPeriods.getAvg();
    return superFreqCycle(highUs, us - highUs);
  }

#ifdef SUPER_FREQ_DEBUG_SERIAL
  void print() {
    periods.print();
    Serial.print(" | ");
    highPeriods.print();
    Serial.print("\r\n");
  }
#endif

private:
  superFreqRingBuffer<N, uint32_t> periods;     /* buffer of periods H->H */
  superFreqRingBuffer<N, uint32_t> highPeriods; /* buffer of periods H->L */
  uint32_t lastHigh = 0;       /* us elapsed since last high */
  bool lastState = false;      /* signal state on last processed sample */
  uint16_t numSamples = 0;     /* sample counter */
  uint16_t timeoutSamples = 0; /* sample count to time out at */
  bool running = false;        /* true indicates signal presence has been detected */
};

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

protected:
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

  void advance(bool state) {
    history <<= 1;
    history |= state;
  }

private:
  uint8_t history = 0;
};

// samples per period

typedef enum {
  Started = 0,
  Stopped,
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

// callbackls here
// caching only ..? if even -- cache at source?
// use debounce-like stop determination based on n samples

#if 0
template <typename T>
struct superFreqMonitorCallback : public superFreqMonitor<T> {
  superFreqMonitorCallback(T& sf) : superFreqMonitor<T>(sf) {}

  void attachCallback(superFreqCallback c, void (*fn)()) {
    switch (c) {
      case superFreqCallback::Started:
        startFn = fn;
        break;
      case superFreqCallback::Stopped:
        stopFn = fn;
        break;
      default:
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
#endif

#endif // __SUPER_FREQ__
