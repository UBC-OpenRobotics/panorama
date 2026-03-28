#pragma once

#include "client/DataBuffer.hpp"
#include <string>
#include <iostream>
#include <atomic>
#include <memory>

class DataBuffer;

class CommandProcessor {
public:
    CommandProcessor(std::shared_ptr<DataBuffer> dataBuffer);
    void start(); //run command loop
    void stop();
    
private:
    std::shared_ptr<DataBuffer> dataBuffer_;
    std::string command;
    std::atomic<bool> running_{true};

    void processCommand(const std::string& command);
};

