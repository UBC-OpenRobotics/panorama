#include "client/sensor.hpp"

Sensor::Sensor(const std::string& sensorName, const std::string& sensorUnit)
    : sensorName_(sensorName), sensorUnit_(sensorUnit), sensorValue_(0.0), lastUpdated_(wxDateTime::Now()), enabled_(true) {
}

// Getter Functions
const std::string& Sensor::GetName() const {
    return sensorName_;
}

const std::string& Sensor::GetUnit() const {
    return sensorUnit_;
}

double Sensor::GetValue() const {
    return sensorValue_;
}

const wxDateTime& Sensor::GetLastUpdated() const {
    return lastUpdated_;
}

bool Sensor::checkEnabled() const {
    return enabled_;
}

// Setter Functions
void Sensor::SetValue(double value) {
    sensorValue_ = value;
    lastUpdated_ = wxDateTime::Now();
}

void Sensor::SetUnit(const std::string& unit) {
    sensorUnit_ = unit;
}

void Sensor::SetEnabled(bool enabled) {
    enabled_ = enabled;
}