#pragma once

#include "client/DataBuffer.hpp"
#include "client/post_processing.hpp"

#include <string>
#include <iostream>
#include <atomic>
#include <memory>

class CommandProcessor {
public:
    CommandProcessor(std::shared_ptr<DataBuffer> dataBuffer, std::shared_ptr<PostProcessing> postProcessor);
    void start(); //run command loop
    void stop();
    
private:
    std::shared_ptr<DataBuffer> dataBuffer_;
    std::shared_ptr<PostProcessing> postProcessor_;
    std::string command;
    std::atomic<bool> running_{true};

    void processCommand(const std::string& command);
};

