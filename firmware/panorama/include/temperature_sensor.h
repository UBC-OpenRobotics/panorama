#ifndef TEMPERATURE_SENSOR_H
#define TEMPERATURE_SENSOR_H

#include <Arduino.h>

// This module manages a Dallas/Maxim 1-Wire temperature sensor (e.g., DS18B20) connected to GPIO4. It provides functions to initialize the sensor and read temperature values in Celsius or Fahrenheit. The sensor is accessed using the DallasTemperature library, which handles the 1-Wire protocol and temperature conversion. The readTemperature function requests a new temperature measurement from the sensor and returns the value in the desired unit, while also printing it to the serial console for debugging purposes.

/***************************************************************************
 * Initializes the 1-Wire temperature sensor. Must be called in setup() before reading temperatures.
***************************************************************************/
void setupTemperatureSensor();

/***************************************************************************
 * reads the temperature from the sensor. If celsius is true, returns the temperature in Celsius; otherwise, returns it in Fahrenheit. Also prints the read temperature to the serial console.
 * @param celsius: If true, returns temperature in Celsius; if false, returns in Fahrenheit. Default is true (Celsius).
 * @return The temperature value read from the sensor in the specified unit.
***************************************************************************/
float readTemperature(bool celsius = true);

#endif
