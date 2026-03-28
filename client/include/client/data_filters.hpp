#pragma once

#include <iostream>
#include <vector>
#include <deque>

class DataFilters {
public:
    DataFilters();

    int KalmanFilter(double input);
    double MovingAverageFilter(double input);

private:
    size_t MAX_KALMAN_SIZE = 10;    
    std::vector<double> kalmanList; 

    size_t MAX_MOVINGAVERAGE = 10;
    std::deque<double> movingAverageList;
   
};