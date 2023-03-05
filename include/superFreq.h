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
  void getAvg();

private:
  void advance();

  T buffer[N] = {0};
  I head = 0;
  I tail = 0;
};

template <size_t N = 5>
class superFreq {
public:
  void update(bool);
  float getFreq();
  float getPulseWidth();
  uint32_t getPeriod();
  uint32_t getHighPeriod();
  uint32_t getLowPeriod();
  int8_t getPeriods(uint32_t *, uint8_t);
  int8_t getHighPeriods(uint32_t *, uint8_t);
  int8_t getLowPeriods(uint32_t *, uint8_t);
  /* get an array of edges, delta relative to first edge */
  int8_t getEdges(superFreqEdge *, uint8_t);
  /* get an array of edges, delta absolute */
  int8_t getEdgesRaw(superFreqEdge *, uint8_t);

private:
  superFreqRingBuffer<N, uint32_t, uint8_t> highPeriods;
  superFreqRingBuffer<N, uint32_t, uint8_t> lowPeriods;
  uint32_t lastHigh = 0;
};

#endif // __SUPER_FREQ__
