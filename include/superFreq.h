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
#ifdef SUPER_FREQ_DEBUG
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
#ifdef SUPER_FREQ_DEBUG
  void print();
#endif

private:
  superFreqRingBuffer<N, uint32_t, I> periods;     /* buffer of periods H->H */
  superFreqRingBuffer<N, uint32_t, I> highPeriods; /* buffer of periods H->L */
  uint32_t lastHigh = 0;
};

#endif // __SUPER_FREQ__
