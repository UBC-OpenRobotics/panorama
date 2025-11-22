#include "DataBuffer.hpp"

DataBuffer::DataBuffer() {}
DataBuffer::~DataBuffer() {}

void DataBuffer::writeData(const std::string& jsonChunk) {
    // TODO: append jsonChunk to buffer_
}

void DataBuffer::setData(const std::string& jsonData) {
    // TODO: replace buffer_ entirely
}

std::string DataBuffer::readAll() const {
    // TODO: return buffer_ as-is
    return {};
}

std::string DataBuffer::consume() {
    // TODO: return buffer_ and clear it
    return {};
}

bool DataBuffer::hasCompleteJson() const {
    // TODO: call findJsonBoundary() and check if != npos
    return false;
}

size_t DataBuffer::size() const {
    // TODO: return buffer_.size()
    return 0;
}

void DataBuffer::clear() {
    // TODO: buffer_.clear()
}

std::string DataBuffer::extractNextJson() {
    // TODO:
    // 1. Use findJsonBoundary() to locate a complete JSON object
    // 2. Extract it from buffer_
    // 3. Remove extracted substring from buffer_
    return {};
}

size_t DataBuffer::findJsonBoundary() const {
    // TODO:
    // Implement brace matching OR delimiter search
    // Return index of end of JSON, or npos if incomplete
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
