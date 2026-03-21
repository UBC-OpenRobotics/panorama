#include "client/json_writer.hpp"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/filewritestream.h" // For writing directly to a FILE* stream
#include <iostream>
#include <fstream>
#include <cstdio> // For fopen, fclose
using namespace rapidjson;

JsonWriter::JsonWriter(std::shared_ptr<DataBuffer> dataBuffer, const std::string& exportPath) 
    : dataBuffer_(dataBuffer), exportPath(exportPath) {}

void JsonWriter::start() {
    // This function will run in a separate thread and continuously check for new data in the DataBuffer.
    while (running_) {
        //sleep for 5 milliseconds to avoid busy waiting
        std::this_thread::sleep_for(std::chrono::milliseconds(5));

        if(dataBuffer_->readAll().size() > 0) {
            buffer_data_t latestData = dataBuffer_->readAll().back(); // Get the most recent data entry
            int written = 0;

            if (latestData.timestamp > previousTimestamp) { // Check if it's new data
                written = writeToJson(latestData);
                if(written){
                    previousTimestamp = latestData.timestamp; // Update the last written timestamp
                } else {
                    std::cout << "Failed to write data to JSON for sensorID: " << latestData.sensorID << std::endl;
                }
            }

        }

    }
}

void JsonWriter::stop() {
    running_ = false;
}

bool JsonWriter::writeToJson(buffer_data_t data) {
    rapidjson::Document doc = getDocumentFromData(data);

    // open/create document in rundir with name of sensorID
    std::string path = exportPath + "/" + std::to_string(data.sensorID);

    FILE* fp = fopen(path.c_str(), "ab");
    if (!fp) { 
        // file couldnt open - return false
        return false;
    }
    
    char writeBuffer[65536];
    FileWriteStream os(fp, writeBuffer, sizeof(writeBuffer));

    Writer<FileWriteStream> writer(os); // write without indentation but more compact

    doc.Accept(writer);
    fputc('\n', fp);
    
    fclose(fp);
    
    return true; // stub
}

rapidjson::Document JsonWriter::getDocumentFromData(buffer_data_t data) {
    rapidjson::Document doc;
    doc.SetObject();
    rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();

    doc.AddMember("data", static_cast<double>(data.data), allocator);

    doc.AddMember("timestamp", static_cast<int64_t>(data.timestamp), allocator);

    rapidjson::Value unit;
    unit.SetString(data.dataunit.c_str(), static_cast<rapidjson::SizeType>(data.dataunit.length()), allocator);
    doc.AddMember("dataunit", unit, allocator);

    rapidjson::Value type;
    type.SetString(data.datatype.c_str(), static_cast<rapidjson::SizeType>(data.datatype.length()), allocator);
    doc.AddMember("datatype", type, allocator);

    rapidjson::Value sensor;
    sensor.SetString(data.sensor.c_str(), static_cast<rapidjson::SizeType>(data.sensor.length()), allocator);
    doc.AddMember("sensor", sensor, allocator);

    doc.AddMember("sensorID", data.sensorID, allocator);

    return doc;
}