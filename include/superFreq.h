#ifndef __SUPER_FREQ__
#define __SUPER_FREQ__

#include <stdint.h>

template <class N>
class superFreq {
  void update(bool);
  float getFreq();
  float getPulseWidth();
  uint32_t getPeriod();
  uint32_t getHighPeriod();
  uint32_t getLowPeriod();

private:
  uint32_t high[N] = {0};
  uint32_t low[N] = {0};
  uint8_t highIndex, lowIndex;
};

#endif // __SUPER_FREQ__
