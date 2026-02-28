#ifndef SENSOR_DATA_H
#define SENSOR_DATA_H

#include <Arduino.h>

struct SensorData {
    const char *sensorName;
    uint16_t sensorId;
    unsigned long timestampMs;
    const char units;
    float value;
};

String sensorDataToJson(const SensorData &data);

#endif
