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
    Document doc = getDocumentFromData(data);

    // open/create document with name of sensorID
    FILE* fp = fopen((std::to_string(data.sensorID)).c_str(), "wb");
    if (!fp) { 
        // file couldnt open - return false
        return false;
    }
    
    char writeBuffer[65536];
    FileWriteStream os(fp, writeBuffer, sizeof(writeBuffer));

    //PrettyWriter<FileWriteStream> writer(os); // write with indentation

    Writer<FileWriteStream> writer(os); // write without indentation but more compact

    doc.Accept(writer);
    
    fclose(fp);
    
    return true; // stub
}

Document JsonWriter::getDocumentFromData(buffer_data_t data) {
    Document doc;
    doc.SetObject();
    Document::AllocatorType& allocator = doc.GetAllocator();

    doc.AddMember("data", data.data, allocator);
    doc.AddMember("timestamp", data.timestamp, allocator);
    doc.AddMember("dataunit", data.dataunit, allocator);
    doc.AddMember("datatype", data.datatype, allocator);
    doc.AddMember("sensor", data.sensor, allocator);
    doc.AddMember("sensorID", data.sensorID, allocator);

    return doc;
}