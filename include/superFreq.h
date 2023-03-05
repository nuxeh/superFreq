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

private:
  T calcAvg();
  void advance();

  T buffer[N] = {0};
  I head = 0;
  I tail = 0;
  I read = 0;

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
  bool readEdge();

private:
  superFreqRingBuffer<N, uint32_t, I> highPeriods;
  superFreqRingBuffer<N, uint32_t, I> lowPeriods;
  uint32_t lastHigh = 0;
};

#endif // __SUPER_FREQ__
