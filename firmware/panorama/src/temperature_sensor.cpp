#include "temperature_sensor.h"

#include <DallasTemperature.h>
#include <OneWire.h>

namespace {
const int ONE_WIRE_BUS = 4;
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
}

void setupTemperatureSensor() {
    sensors.begin();
    Serial.println("Temperature sensor initialized");
}

float readTemperature(bool celsius) {
    sensors.requestTemperatures();
    if (celsius) {
        float tempC = sensors.getTempCByIndex(0);
        Serial.printf("Temperature: %.2f C\n", tempC);
        return tempC;
    }

    float tempF = sensors.getTempFByIndex(0);
    Serial.printf("Temperature: %.2f F\n", tempF);
    return tempF;
}
