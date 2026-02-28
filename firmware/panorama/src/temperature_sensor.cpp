#include "temperature_sensor.h"

namespace {
uint8_t gAdcPin = 34;
float gVRef = 3.3f;
uint8_t gSampleCount = 8;
}

void setupTemperatureSensor(uint8_t adcPin, float vRef, uint8_t sampleCount) {
    gAdcPin = adcPin;
    gVRef = vRef;
    gSampleCount = sampleCount == 0 ? 1 : sampleCount;

    pinMode(gAdcPin, INPUT);
#if defined(ESP32)
    analogReadResolution(12);
    analogSetPinAttenuation(gAdcPin, ADC_11db);
#endif
    Serial.printf("LM335 ADC sensor initialized on GPIO %u (vRef=%.2fV, samples=%u)\n",
                  (unsigned int)gAdcPin,
                  gVRef,
                  (unsigned int)gSampleCount);
}

float readTemperature(bool celsius) {
    uint32_t totalRaw = 0;
    for (uint8_t i = 0; i < gSampleCount; i++) {
        totalRaw += (uint32_t)analogRead(gAdcPin);
    }

    float raw = (float)totalRaw / (float)gSampleCount;
    float voltage = (raw / 4095.0f) * gVRef;

    // LM335 output is approximately 10mV per Kelvin.
    float tempK = voltage * 100.0f;
    float tempC = tempK - 273.15f;

    if (celsius) {
        return tempC;
    }
    return (tempC * 9.0f / 5.0f) + 32.0f;
}
