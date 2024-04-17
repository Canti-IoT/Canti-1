#ifndef PARAMETER_SENSOR_H
#define PARAMETER_SENSOR_H

#include "ParameterTypes.hpp"
#include "AbstractSensor.hpp"

struct SensorData {
    ParameterType parameter;
    int recurrence;
    AbstractSensor *sensor;
    float value;
};

#endif
