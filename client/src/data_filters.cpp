#include "client/data_filters.hpp";


DataFilters::DataFilters() {
}


int DataFilters::KalmanFilter(double input) {

    // QUEUE DATA TYPE, FIRST IN FIRST OUT
    return 0.0;
}

double DataFilters::MovingAverageFilter(double input) {
    movingAverageList.push_front(input);
    if (MAX_MOVINGAVERAGE <= movingAverageList.size()) {
        movingAverageList.pop_back();
    }
    
    double sum = 0;
    int i = 0;
    while (i < movingAverageList.size()) {
        sum += movingAverageList[i];
        i++;
    }

    return sum / i;
}