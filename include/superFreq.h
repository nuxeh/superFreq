#ifndef __SUPER_FREQ__
#define __SUPER_FREQ__

#include <stdint.h>

class superFreq {
  void update(bool);
  float getFreq();
  float getPulseWidth();
  uint32_t getPeriod();
  uint32_t getHighPeriod();
  uint32_t getLowPeriod();

private:
  uint32_t lastUsHigh = 0;
  uint32_t lastUsLow = 0;
};

#endif // __SUPER_FREQ__
