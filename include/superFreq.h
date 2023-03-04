#ifndef __SUPER_FREQ__
#define __SUPER_FREQ__

#include <Arduino.h>

typedef struct {
  bool state;
  uint32_t delta;
} superFreqEdge;

template <uint8_t N = 4>
class superFreq {
public:
  void update(bool);
  float getFreq();
  float getPulseWidth();
  uint32_t getPeriod();
  uint32_t getPeriodAvg();
  uint32_t getHighPeriod();
  uint32_t getHighPeriodAvg();
  uint32_t getLowPeriod();
  uint32_t getLowPeriodAvg();
  int8_t getPeriods(uint32_t *, uint8_t);
  int8_t getHighPeriods(uint32_t *, uint8_t);
  int8_t getLowPeriods(uint32_t *, uint8_t);
  /* get an array of edges, delta relative to first edge */
  int8_t getEdges(superFreqEdge *, uint8_t);
  /* get an array of edges, delta absolute */
  int8_t getEdgesRaw(superFreqEdge *, uint8_t);

private:
  uint32_t high[N] = {0};
  uint32_t low[N] = {0};
  uint8_t highIndex, lowIndex;
};

#endif // __SUPER_FREQ__
