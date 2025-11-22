#pragma once

#include <rapidjson/document.h>
#include <rapidjson/filereadstream.h>
#include <string>

class JsonReader {
public:
    // Constructor takes the filename of the JSON file
    JsonReader(const std::string& filename);

    // Parse the JSON file
    bool parse();

    // Accessors for JSON data
    const rapidjson::Document& getDocument() const;

private:
    std::string filename_;
    rapidjson::Document document_;
};