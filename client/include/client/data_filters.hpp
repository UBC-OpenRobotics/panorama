#pragma once

#include <iostream>
#include <list>

class DataFilters {
public:
    DataFilters();

    int kalmanFilter(int input);

private:
    std::list<int> kalmanList;
    int MAX_KALMAN_SIZE;
};