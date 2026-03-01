#pragma once
#include <ctime>

typedef struct {
    float data; // actual value
    std::time_t timestamp; // date recorded
    std::string dataunit; // e.g. "kPa", "mL"
    std::string datatype; // e.g. "temperature", "sound"
    /*
    std::string sensor;
    std::string sensorID;
    */

} buffer_data_t;   