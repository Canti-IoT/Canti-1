#ifndef DEBUG_MACROS_H
#define DEBUG_MACROS_H

#include <Arduino.h>

#if DEBUGENABLE == 1
    #define DEBUGINIT(rate) USBSerial.begin(rate)
#endif

#if DEBUGENABLE == 1
    #define DEBUG(...) USBSerial.printf(__VA_ARGS__)
#endif

#endif /* DEBUG_MACROS_H */
