#ifndef TEMPERATURE_SENSOR_H
#define TEMPERATURE_SENSOR_H

#include <Arduino.h>

void setupTemperatureSensor(uint8_t adcPin = 34, float vRef = 3.3f, uint8_t sampleCount = 8);
float readTemperature(bool celsius = true);

#endif
