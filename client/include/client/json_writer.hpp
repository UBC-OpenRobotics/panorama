#pragma once

#include "common/panorama_defines.hpp"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/filewritestream.h" // For writing directly to a FILE* stream
#include <iostream>
#include <fstream>
#include <cstdio> // For fopen, fclose

#include "client/DataBuffer.hpp"
#include <memory>
#include <atomic>
#include <thread>
#include <chrono>


class DataBuffer;

class JsonWriter {
public: 
    JsonWriter(std::shared_ptr<DataBuffer> dataBuffer, const std::string& exportPath); // constructor

    void start();
    void stop();

    // returns boolean if has been written to 
    bool writeToJson(buffer_data_t data); 

    Document getDocumentFromData(buffer_data_t data);

private:
    std::time_t previousTimestamp = 0;
    std::atomic<bool> running_{true};
    std::shared_ptr<DataBuffer> dataBuffer_;
    std::string exportPath;
};