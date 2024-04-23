#include "AlarmManager.hpp"
#include <SensorManager.hpp>
#include <debug.h>

SensorManager *sensorManager4 = nullptr;

// Array to store alarms
RTC_DATA_ATTR Alarm alarms[MAX_ALARMS][MAX_PARAMETERS_PER_ALARM];
RTC_DATA_ATTR uint8_t triggered[MAX_ALARMS] = {};
RTC_DATA_ATTR bool enabled[MAX_ALARMS] = {};

bool new_data = false;

Buzzer *b = nullptr;

AlarmManager::AlarmManager()
{
    sensorManager4 = &SensorManager::getInstance();
    sensorManager4->setAlarmCallback(std::bind(&AlarmManager::onSensorRead, this));
    // Initialize alarms
    for (int i = 0; i < MAX_ALARMS; ++i)
    {
        for (int j = 0; j < MAX_PARAMETERS_PER_ALARM; ++j)
        {
            alarms[i][j].parameterIndex = ParameterType::NONE;
        }
        enabled[i] = false; // Initially, all alarms are disabled
        triggered[i] = 0;   // Initially, all alarms are not triggered
    }
    new_data = false;
}

void AlarmManager::setBuzzer(Buzzer *_b)
{
    b = _b;
}

void AlarmManager::onSensorRead()
{
    new_data = true;
    DEBUG("On sensor read\n");
}

void AlarmManager::setAlarm(uint8_t alarmIndex, uint8_t parameterIndex, IntervalType intervalType, float lowerValue, float upperValue)
{
    // Find the first available slot for the parameter index
    int availableSlot = -1;
    for (int i = 0; i < MAX_PARAMETERS_PER_ALARM; ++i)
    {
        if (alarms[alarmIndex][i].parameterIndex == ParameterType::NONE)
        {
            availableSlot = i;
            break;
        }
    }

    // If no available slot was found, return without setting the alarm
    if (availableSlot == -1)
    {
        return;
    }

    // Populate the found slot with the alarm configuration
    alarms[alarmIndex][availableSlot].parameterIndex = static_cast<ParameterType>(parameterIndex);
    alarms[alarmIndex][availableSlot].intervalType = intervalType;
    alarms[alarmIndex][availableSlot].lowerValue = lowerValue;
    alarms[alarmIndex][availableSlot].upperValue = upperValue;
}

void AlarmManager::enableAlarm(uint8_t alarmIndex)
{
    if (alarmIndex < MAX_ALARMS)
    {
        enabled[alarmIndex] = true;
    }
}

void AlarmManager::disableAlarm(uint8_t alarmIndex)
{
    if (alarmIndex < MAX_ALARMS)
    {
        enabled[alarmIndex] = false;
    }
}

void AlarmManager::deleteAlarm(uint8_t alarmIndex)
{
    if (alarmIndex < MAX_ALARMS)
    {
        // Reset alarm configuration
        for (int i = 0; i < MAX_PARAMETERS_PER_ALARM; ++i)
        {
            alarms[alarmIndex][i].parameterIndex = ParameterType::NONE;
            alarms[alarmIndex][i].intervalType = IntervalType::DISABLED_INTERVAL;
            alarms[alarmIndex][i].lowerValue = 0.0f;
            alarms[alarmIndex][i].upperValue = 0.0f;
        }
        enabled[alarmIndex] = false;
        triggered[alarmIndex] = 0;
    }
}

void AlarmManager::loop()
{
    if (new_data)
    {
        // Check if any alarm is triggered
        for (int i = 0; i < MAX_ALARMS; ++i)
        {
            if (enabled[i] == false)
            {
                continue;
            }
            triggered[i] = ALARM_TRIGGER_CYCLES;
            for (int j = 0; j < MAX_PARAMETERS_PER_ALARM; ++j)
            {
                if (alarms[i][j].parameterIndex != ParameterType::NONE)
                {
                    float sensorValue = sensorManager4->getValue(static_cast<uint8_t>(alarms[i][j].parameterIndex));
                    if (alarms[i][j].intervalType == INSIDE && sensorValue >= alarms[i][j].lowerValue && sensorValue <= alarms[i][j].upperValue)
                    {
                        // alarms[i][j].triggered = true;
                    }
                    else if (alarms[i][j].intervalType == OUTSIDE && (sensorValue < alarms[i][j].lowerValue || sensorValue > alarms[i][j].upperValue))
                    {
                        // alarms[i][j].triggered = true;
                    }
                    else
                    {
                        // If any parameter is not triggered, set the flag to false
                        triggered[i] = 0;
                    }
                }
            }
            new_data = false;
        }
    }

    // Check if any alarm is triggered
    for (int i = 0; i < MAX_ALARMS; ++i)
    {
        if (enabled[i] == false)
        {
            continue;
        }
        // If all parameters are triggered, emit the alarm
        if (triggered[i] > 0)
        {
            new_data = true;
            b->playTune1();
            triggered[i] -= 1;
            DEBUG("Alarm %d trigers remainig %d\n", i, triggered[i]);
        }
    }
}
