#pragma once

#include <rapidjson/document.h>
#include <rapidjson/filereadstream.h>
#include <string>
#include "panorama_defines.hpp"

class JsonReader {
public:
    // Constructor takes the filename of the JSON file
    JsonReader(const std::string& filename);

    // Parse the JSON file
    bool parse();

    // Accessors for JSON data
    const rapidjson::Document& getDocument() const;

    buffer_data_t exportToBuffer(); 

private:
    std::string filename_;
    rapidjson::Document document_;
};