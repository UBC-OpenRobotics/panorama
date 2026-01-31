#pragma once
#include <ctime>

typedef struct {
    float data; // actual value
    std::time_t timestamp; // date recorded
    const char * dataunit; // e.g. "kPa", "mL"
    const char * datatype; // e.g. "temperature", "sound"

} buffer_data_t;   