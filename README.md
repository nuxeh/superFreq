## superFreq

[![PlatformIO Registry](https://badges.registry.platformio.org/packages/nuxeh/library/superFreq.svg)](https://registry.platformio.org/libraries/nuxeh/superFreq)

A small Arduino library for measuring frequencies, and getting other parameters
from a repetetive signal.

The micros() Arduino API call is used to get a determination of time passed.
This uses e.g. Timer1, on 8-bit Atmega microcontrollers. This method doesn't
require the use of a dedicated timer capture compare input.
