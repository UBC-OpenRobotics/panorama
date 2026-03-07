#include "client/json_writer.hpp"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/filewritestream.h" // For writing directly to a FILE* stream
#include <iostream>
#include <fstream>
#include <cstdio> // For fopen, fclose

JsonWriter::JsonWriter() {}

bool writeToJson(int sensorID, buffer_data_t data) {
    return false; // stub
}