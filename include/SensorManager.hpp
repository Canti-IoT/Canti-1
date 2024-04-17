#ifndef SENSOR_MANAGER_H
#define SENSOR_MANAGER_H

#include "sensor/ParameterSensor.hpp"

class SensorManager {
public:
    SensorManager(); // Constructor

    // Function to add a sensor to the manager
    void addSensor(ParameterType parameter, int recurrence, AbstractSensor* sensor);

    // Function to get the sensor data for a given parameter
    SensorData* getSensor(ParameterType parameter);

    // Function to set recurrence for a sensor with a given index
    void setRecurrenceWithIndex(ParameterType index, int recurrence);

    // Function to begin reading for all sensors
    void beginReadingAll();

    // Function to get the parameter type by index
    float getValue(int index);

    bool validateIndex(int index);

private:
    static const int MAX_SENSORS = 8; // Maximum number of sensors
    SensorData sensors[MAX_SENSORS]; // Array of SensorData
    int currentSize; // Current size of the array
};

#endif
