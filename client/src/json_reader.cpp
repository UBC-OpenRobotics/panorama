#include "client/json_reader.hpp"
#include <cstdio>      // for FILE*, fopen, fclose
#include <iostream>
#include <filesystem>

JsonReader::JsonReader(const std::string& filename)
    : filename_(filename) {}

bool JsonReader::parse() {
    FILE* fp = fopen("example.json", "r");
    if (!fp) {
        std::cerr << "Could not open file: " << filename_ << std::endl;
        std::cerr << "Current working directory: " 
                  << std::filesystem::current_path() << std::endl;
        return false;
    }

    char readBuffer[65536];
    rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));

    document_.ParseStream(is);
    fclose(fp);

    if (document_.HasParseError()) {
        std::cerr << "Error parsing JSON file: " << filename_ << std::endl;
        return false;
    }

    return true;
}

const rapidjson::Document& JsonReader::getDocument() const {
    return document_;
}

buffer_data_t JsonReader::exportToBuffer() {
    
}