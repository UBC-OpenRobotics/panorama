#include "client/post_processing.hpp"

PostProcessing::PostProcessing() {
    
}

float PostProcessing::processData(float data) {
    //std::cout << "currentOffset: " << currentOffset << ", currentScaleFactor: " << currentScaleFactor << std::endl; // Debug output
    std::lock_guard<std::mutex> lock(mutex_);
    return data*currentScaleFactor + currentOffset;
}

void PostProcessing::addOffset(float offset) {
    std::lock_guard<std::mutex> lock(mutex_);
    currentOffset = offset; //record the offset value
    std::cout << "Offset set to: " << offset << std::endl;
}

void PostProcessing::addScaling(float scaleFactor) {
    std::lock_guard<std::mutex> lock(mutex_);
    currentScaleFactor = scaleFactor; //record the scale factor value
    std::cout << "Scale factor set to: " << scaleFactor << std::endl;
}

void PostProcessing::reset() {
    std::lock_guard<std::mutex> lock(mutex_);
    //reset all post processing parameters to default values
    addOffset(0); //reset offset to 0;
    addScaling(1.0); //reset scale factor to 1;
}

void PostProcessing::updateDataBase() {
    //TODO: update the database accordingly

    
}

