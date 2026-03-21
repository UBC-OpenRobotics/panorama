#include "client/json_reader.hpp"
#include <rapidjson/error/en.h> // for GetParseError_En
#include <cstdio>      // for FILE*, fopen, fclose
#include <iostream>
#include <filesystem>
#include <sstream>
#include <vector>
#include <string>
#include <ctime>
#include "common/panorama_utils.hpp"

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
    std::cout << json.c_str() << std::endl;
    if (!ok) {
        std::cerr << "JSON parse error at offset " << ok.Offset()
                  << ": " << rapidjson::GetParseError_En(ok.Code()) << std::endl;
        //std::cout << json;
        return ret;  // return default-initialized struct
    } 

    if (!doc.IsObject()) {
        std::cerr << "JSON is not an object!\n";
        return ret;
    }

    // if (!doc.HasMember("sensor") || !doc["sensor"].IsString() ||
    //     !doc.HasMember("unit") || !doc["unit"].IsString() ||
    //     !doc.HasMember("value") || !doc["value"].IsNumber()) {
    //     return ret;
    // }

    // ret.datatype = std::string(doc["sensor"].GetString(), doc["sensor"].GetStringLength());
    // ret.data = doc["value"].GetDouble();
    // ret.dataunit = std::string(doc["unit"].GetString(), doc["unit"].GetStringLength());
    // ret.timestamp = std::time(&ret.timestamp);
    ret.sensor = "";
    ret.datatype = "";
    ret.dataunit = "";
    ret.data = 0;
    ret.timestamp = NULL;

    if (doc.HasMember("sensor")) {
        std::string sensorTypeString (
            doc["sensor"].GetString(),
            doc["sensor"].GetStringLength()
        );
        ret.sensor = sensorTypeString.c_str();
    }
    if (doc.HasMember("dataunit")) {
        std::string sensorUnitString (
            doc["dataunit"].GetString(),
            doc["dataunit"].GetStringLength()
        );
        ret.dataunit = sensorUnitString.c_str();
    }
    if (doc.HasMember("data")) {
        double sensorValue = doc["data"].GetDouble();
        ret.data = sensorValue;
    }
    if (doc.HasMember("datatype")) {
        std::string sensorUnitString (
            doc["datatype"].GetString(),
            doc["datatype"].GetStringLength()
        );
        ret.datatype = sensorUnitString.c_str();
    }
    if (doc.HasMember("timestamp")) {
        ret.timestamp = (long) doc["timestamp"].GetInt();
    }

    
    
    
    
    //ret.timestamp = std::time(nullptr);

    return ret;

}