#pragma once

#include <rapidjson/document.h>
#include <rapidjson/filereadstream.h>
#include <string>
#include <vector>
#include "common/panorama_defines.hpp"

class JsonReader {
public:
    // Constructor takes the filename of the JSON file
    JsonReader();

    // Parse the JSON file
    bool parse();

    // Accessors for JSON data
    const rapidjson::Document& getDocument() const;

    buffer_data_t exportToBuffer(std::string json);

    // Parse all newline-delimited JSON objects from a TCP chunk.
    // Stores any trailing incomplete data in recvBuffer_ for the next call.
    std::vector<buffer_data_t> exportAllToBuffer(const std::string& chunk);

private:
    std::string filename_;
    rapidjson::Document document_;
    std::string recvBuffer_;  // accumulates partial JSON across recv() calls
};