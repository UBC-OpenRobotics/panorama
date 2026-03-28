#pragma once

#include <mutex>
#include <iostream>
#include <list>

class PostProcessing {
public:
    PostProcessing();
    float processData(float data);
    void reset();
    void addOffset(float offset);
    void addScaling(float scaleFactor);
    void updateDataBase();

private:
    float currentOffset = 0.0;
    float currentScaleFactor = 1.0;
    std::mutex mutex_;
    
};