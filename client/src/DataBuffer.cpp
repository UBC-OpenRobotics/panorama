#include "client/DataBuffer.hpp"
#include <iostream>

DataBuffer::DataBuffer(const std::string& logFilePath) 
    : logFilePath_(logFilePath) {
   // TODO: initialize buffer if needed
}

DataBuffer::~DataBuffer() {
    // TODO: clean things up if needed 
}

void DataBuffer::writeData(buffer_data_t jsonChunk) {
    // Append the new chunk of raw JSON data to the buffer. This functions only job is to store raw
    // inbound data in a way that doesn't lose anything later the parser will call extractNextJson()
    // parseNextJson()
   // std::cout << "[DataBuffer] Writing data to buffer:" << std::endl;
   // std::cout << "[DataBuffer]   a: '" << jsonChunk.a << "' a_data: " << jsonChunk.a_data << std::endl;
   // std::cout << "[DataBuffer]   b: '" << jsonChunk.b << "' b_data: " << jsonChunk.b_data << std::endl;
   // std::cout << "[DataBuffer]   Buffer size before write: " << size() << std::endl;
   
   //get the runtime directory path from DataLogger to export buffer if it exceeds threshold

   //DataLogger logger;
   //std::string exportPath = logger.getLogFilePath();

    write(jsonChunk);

    if ((int)size() > FLUSH_THRESHOLD * MAX_BUFFER_SIZE / 100) {
        popFront();
        exportBuffer(logFilePath_);
    }
    // std::cout << "[DataBuffer]   Buffer size after write: " << size() << std::endl;
    // std::cout << buffer_.size();
    // std::cout << "[DataBuffer] : " << toStringAll() << std::endl;
}

void DataBuffer::setData(buffer_data_t jsonData) {
    // Overwrite the entire buffer with new raw JSON data
    BufferBase<buffer_data_t>::setData(jsonData);
}

std::list<buffer_data_t> DataBuffer::readAll() const {
    // TODO: return raw buffer as is
    return BufferBase<buffer_data_t>::readAll();
}

std::list<buffer_data_t> DataBuffer::consume() {
    // TODO: copy the raw buffer then clear the buffer and return copied content
    return BufferBase<buffer_data_t>::consume();
}

size_t DataBuffer::size() const {
    return BufferBase<buffer_data_t>::size();
}

void DataBuffer::clear() {
    BufferBase<buffer_data_t>::clear();
}

bool DataBuffer::hasCompleteJson() const {
    // For now, just check if buffer is not empty
    // In a more sophisticated implementation, this would check for complete JSON objects
    return !buffer_.empty();
}

std::string DataBuffer::extractNextJson() {
    // TODO:
    // 1. Use findJsonBoundary() to locate a complete JSON object
    // 2. Extract it from buffer_
    // 3. Remove extracted substring from buffer_
    return toString(extractNextBuffer());
}

size_t DataBuffer::findJsonBoundary() const {
    // TODO: for struct based packets, boundary = sizeof(buffer_data_t) retunr npos if incomplete 
    return 0;
}

bool DataBuffer::isValidJson(const std::string& jsonStr) const {
    // TODO:
    // Attempt decoding using your JSON library
    // Return true if valid, false otherwise
    return false;
}

bool DataBuffer::decodeJson(const std::string& jsonStr /*, ParsedData &out */) {
    // TODO:
    // Use chosen parser (e.g., nlohmann/json → json::parse(jsonStr))
    // and fill the ParsedData struct/class
    //
    // Return true on success, false on parsing error
    return false;
}

std::string DataBuffer::parseNextJson(/* ParsedData &out */) {
    // TODO:
    // 1. Use extractNextJson() to get next complete object
    // 2. Validate using isValidJson()
    // 3. Decode using decodeJson()
    // 4. Return true if parsed successfully
    return "";
}

void DataBuffer::parseAll(/* std::vector<ParsedData> &out */) {
    // TODO:
    // Loop:
    //   while (hasCompleteJson())
    //     parseNextJson(...)
}


std::string DataBuffer::toString(const buffer_data_t& buffer_item) {
    //convert one struct of buffer_ into string

    std::string temp = "{";

    temp = temp + "\"datatype\": \"" + buffer_item.datatype + "\", \"data\": " + std::to_string(buffer_item.data) + ", ";
    temp = temp + "\"dataunit\": \"" + buffer_item.dataunit + "\", ";
    temp = temp + "\"timestamp\": " + std::to_string(buffer_item.timestamp);

    temp += "}";

    return temp;
}

std::string DataBuffer::toStringAll() {
    //print all buffer_ as string
    //buffer_ is an array of buffer_data_t
    std::string res = "";
    int c = 0;
    for (buffer_data_t buffer_item : readAll()) {
        if (c == (int)size() - 1) {
            res += toString(buffer_item) + "\n";
        } else {
            res += toString(buffer_item) + ",\n";
        }
        
        c++;
    }

    return res;
}

void DataBuffer::exportBuffer(std::string exportPath) {
    //Export the entire buffer (make a local JSON file under client/src/)
    
    std::string exportFile = exportPath + "/exported_buffer.json";

    FILE* fp = fopen(exportFile.c_str(), "w");
    if (!fp) {
		std::cerr << "Could not open file for writing exported buffer." << std::endl;
        std::cerr << "logFilePath: " << exportPath << std::endl;
        return;
    }
    
    std::string json_content = "[\n" + toStringAll() + "]";
    //std::cout << "JSON CONTENT: " << json_content << std::endl;
    fputs(json_content.c_str(), fp);
    fclose(fp);
    return;
}