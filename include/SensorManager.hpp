#ifndef SENSOR_MANAGER_H
#define SENSOR_MANAGER_H

#include "sensor/ParameterSensor.hpp"

#define MAX_SENSORS 8

class SensorManager {
public:
    // Function to get the singleton instance
    static SensorManager& getInstance() {
        static SensorManager instance;
        return instance;
    }

    // Function to add a sensor to the manager
    void addSensor(ParameterType parameter, int recurrence, AbstractSensor* sensor);

    ParameterType getSupportedParameter(bool reset = false);

    // Function to set recurrence for a sensor with a given index
    void setRecurrenceWithIndex(ParameterType index, int recurrence);

    // Function to begin reading for all sensors
    void initAll();
    void testAll();
    void readAll();

    // sensor manager loop function
    void loop();

    // Function to get the parameter type by index
    float getValue(int index);

    bool validateIndex(int index);

private:
    SensorManager(); // Private constructor to prevent instantiation
    SensorData sensors[MAX_SENSORS]; // Array of SensorData
    int currentSize; // Current size of the array
};

enum class ManagingStates {
    INITIALIZATION,
    TESTING,
    READING,
    WAITING,
    NUM_STATES // Just to keep track of the number of states
};

#endif
