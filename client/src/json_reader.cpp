#include "client/json_reader.hpp"
#include <rapidjson/error/en.h> // for GetParseError_En
#include <cstdio>      // for FILE*, fopen, fclose
#include <iostream>
#include <filesystem>
#include <sstream>
#include <vector>
#include <string>

JsonReader::JsonReader() {}

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

buffer_data_t JsonReader::exportToBuffer(std::string json) {
    buffer_data_t ret;
    
    rapidjson::Document doc;
    rapidjson::ParseResult ok = doc.Parse(json.c_str());
    std::cout << json;
    if (!ok) {
        std::cerr << "JSON parse error at offset " << ok.Offset()
                  << ": " << rapidjson::GetParseError_En(ok.Code()) << std::endl;
        return ret;  // return default-initialized struct
    }

    if (!doc.IsObject()) {
        std::cerr << "JSON is not an object!\n";
        return ret;
    }

    if (doc["sensor"].GetString() == "temperature") {
        ret.a_data = doc["value"].GetFloat();
    } else if (doc["sensor"].GetString() == "pressure") {
        ret.b_data = doc["value"].GetFloat();
        std::cout << ret.b_data << "B DATA";
    }
    return ret;

}