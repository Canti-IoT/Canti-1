#ifndef DEBUG_MACROS_H
#define DEBUG_MACROS_H

#include <Arduino.h>
#include <RTCSingleton.hpp>

#if DEBUGENABLE == 1
    #define DEBUGINIT(rate) USBSerial.begin(rate)
#endif

#if DEBUGENABLE == 1
    #define DEBUG(...) USBSerial.printf(__VA_ARGS__)
#endif

#if DEBUGENABLE == 1
    #define TIMESTAMP() DEBUG("%d:%d:%d:%d - ", RTCSingleton::rtc.getHour(), RTCSingleton::rtc.getMinute(), RTCSingleton::rtc.getSecond(), RTCSingleton::rtc.getMillis())
#endif
  

#endif /* DEBUG_MACROS_H */
