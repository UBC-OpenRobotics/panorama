#include "client/DataBuffer.hpp"

DataBuffer::DataBuffer() {
    // TODO: any initialization if needed
}
DataBuffer::~DataBuffer() {
    // TODO: clean things up if needed 
}

void DataBuffer::writeData(const std::string& jsonChunk) {
    // Append the new chunk of raw JSON data to the buffer. This functions only job is to store raw 
    // inbound data in a way that doesn't lose anything later the parser will call extractNextJson()
    // parseNextJson()
    buffer_ += jsonChunk;
}

void DataBuffer::setData(const std::string& jsonData) {
    // Overwrite the entire buffer with new raw JSON data
    buffer_ = jsonData;
}

std::string DataBuffer::readAll() const {
    // TODO: return raw buffer as is
    return buffer_;
}

std::string DataBuffer::consume() {
    // TODO: copy the raw buffer then clear the buffer and return copied content
    std::string temp = buffer_;
    buffer_.clear();
    return temp;
}

bool DataBuffer::hasCompleteJson() const {
    // return true if buffer_ contains sizeof(buffer_data_t) bytes
    return buffer_.size() >= sizeof(buffer_data_t);
}

size_t DataBuffer::size() const {
    // return size of internal buffer
    return buffer_.size();
}

void DataBuffer::clear() {
    // Clear the internal buffer
    buffer_.clear();
}

std::string DataBuffer::extractNextJson() {
    // TODO:
    // 1. Use findJsonBoundary() to locate a complete JSON object
    // 2. Extract it from buffer_
    // 3. Remove extracted substring from buffer_
    return {};
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

bool DataBuffer::parseNextJson(/* ParsedData &out */) {
    // TODO:
    // 1. Use extractNextJson() to get next complete object
    // 2. Validate using isValidJson()
    // 3. Decode using decodeJson()
    // 4. Return true if parsed successfully
    return false;
}

void DataBuffer::parseAll(/* std::vector<ParsedData> &out */) {
    // TODO:
    // Loop:
    //   while (hasCompleteJson())
    //     parseNextJson(...)
}
