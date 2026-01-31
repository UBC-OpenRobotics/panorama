#pragma once
#include <ctime>

typedef struct {
    float data;
    char datatype;
    std::time_t timestamp;
    char dataunit;

} buffer_data_t;   