## superFreq

[![PlatformIO Registry](https://badges.registry.platformio.org/packages/nuxeh/library/superFreq.svg)](https://registry.platformio.org/libraries/nuxeh/superFreq)

A small Arduino library for measuring frequencies, and getting other parameters
from a repetetive signal.

The micros() Arduino API call is used to get a determination of time passed.
This uses e.g. Timer1, on 8-bit Atmega microcontrollers. This method doesn't
require the use of a dedicated timer capture compare input pin.

It's targetted at slower frequency signals, originally conceived to analyse
signals sent to a pulsed solenoid liquid pump in the 0-10 Hz range. There is
some likelihood however, that it could be used for faster signals, though this
hasn't been tested. It is, however, designed with computational efficiency,
compactness, optimisability, and minimising interrupt latencies in mind.
