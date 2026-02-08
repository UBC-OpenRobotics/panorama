#pragma once
#include <ctime>
#include <string>

typedef struct {
    float data; // actual value
    std::time_t timestamp; // date recorded
    std::string dataunit; // e.g. "kPa", "mL"
    std::string datatype; // e.g. "temperature", "sound"

} buffer_data_t;   