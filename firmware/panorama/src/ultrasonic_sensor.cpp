#include "ultrasonic_sensor.h"

void setupUltrasonic(uint8_t triggerPin, uint8_t echoPin) {
    pinMode(triggerPin, OUTPUT);
    pinMode(echoPin, INPUT);
    digitalWrite(triggerPin, LOW);
}

float readUltrasonicCm(uint8_t triggerPin, uint8_t echoPin, unsigned long timeoutUs) {
    digitalWrite(triggerPin, LOW);
    delayMicroseconds(2);
    digitalWrite(triggerPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(triggerPin, LOW);

    unsigned long durationUs = pulseIn(echoPin, HIGH, timeoutUs);
    if (durationUs == 0) {
        return -1.0f;
    }

    return (durationUs * 0.0343f) / 2.0f;
}
