#pragma once

#include "common/panorama_defines.hpp"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/filewritestream.h" // For writing directly to a FILE* stream
#include <iostream>
#include <fstream>
#include <cstdio> // For fopen, fclose

class JsonWriter {
public: 
    JsonWriter(); // constructor


    // returns boolean if has been written to 
    bool writeToJson(int sensorID, buffer_data_t data); 

//private:


};