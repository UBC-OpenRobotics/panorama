#include "client/DataBuffer.hpp"

DataBuffer::DataBuffer() {
    // TODO: any initialization if needed
}
DataBuffer::~DataBuffer() {
    // TODO: clean things up if needed 
}

void DataBuffer::writeData(buffer_data_t jsonChunk) {
    // Append the new chunk of raw JSON data to the buffer. This functions only job is to store raw
    // inbound data in a way that doesn't lose anything later the parser will call extractNextJson()
    // parseNextJson()
    write(jsonChunk);
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

std::string DataBuffer::toString(const buffer_data_t& buffer_item) {
    //convert one struct of buffer_ into string
    std::string temp = "{";
    temp = temp + "\"" + buffer_item.a + "\": " + std::to_string(buffer_item.a_data) + ", ";
    temp = temp + "\"" + buffer_item.b + "\": " + std::to_string(buffer_item.b_data);
    temp += "}";
    return temp;
}

std::string DataBuffer::toStringAll() {
    //print all buffer_ as string
    //buffer_ is an array of buffer_data_t
    std::string res = "";

    for (buffer_data_t buffer_item : buffer_) {
        res += toString(buffer_item) + ",\n";
    }
    return res;
}
