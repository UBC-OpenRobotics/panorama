#ifndef ULTRASONIC_SENSOR_H
#define ULTRASONIC_SENSOR_H

#include <Arduino.h>

/********************************************************************
 * sets up the ultrasonic sensor by configuring the trigger and echo pins. Must be called in setup() before reading distances.
 * @param triggerPin: GPIO pin number connected to the sensor's trigger pin
 * @param echoPin: GPIO pin number connected to the sensor's echo pin
********************************************************************/
void setupUltrasonic(uint8_t triggerPin, uint8_t echoPin);

/********************************************************************
 * reads the distance from the ultrasonic sensor in centimeters. Returns -1.0 if no echo is received within the specified timeout.
 * @param triggerPin: GPIO pin number connected to the sensor's trigger pin
 * @param echoPin: GPIO pin number connected to the sensor's echo pin
 * @param timeoutUs: Maximum time to wait for an echo in microseconds (default is 25000 us, which corresponds to ~4.3 meters)
 * @return Distance in centimeters, or -1.0 if no echo is received within the timeout
********************************************************************/
float readUltrasonicCm(uint8_t triggerPin, uint8_t echoPin, unsigned long timeoutUs = 25000);

#endif
