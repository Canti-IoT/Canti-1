#ifndef ALARMMANAGER_H
#define ALARMMANAGER_H
#include <stdint.h>
#include "sensor/ParameterTypes.hpp"
#include "buzzer.hpp"

#define MAX_ALARMS 3
#define MAX_PARAMETERS_PER_ALARM 8
#define ALARM_TRIGGER_CYCLES 5

// Enum for interval type
enum IntervalType
{
    DISABLED_INTERVAL,
    INSIDE,
    OUTSIDE
};

typedef struct Alarm
{
    ParameterType parameterIndex;
    IntervalType intervalType;
    float lowerValue;
    float upperValue;
}Alarm;

class AlarmManager
{
public:
    AlarmManager(Buzzer *_b);

    // Method to set an alarm
    void setAlarm(uint8_t alarmIndex, uint8_t parameterIndex, IntervalType intervalType, float lowerValue = 0.0f, float upperValue = 0.0f);

    // Method to enable an alarm
    void enableAlarm(uint8_t alarmIndex);

    // Method to disable an alarm
    void disableAlarm(uint8_t alarmIndex);

    // Method to delete an alarm
    void deleteAlarm(uint8_t alarmIndex);

    // Alarm loop, emits the alarm if alarm was triggered
    void loop();

    void onSensorRead();
};

#endif