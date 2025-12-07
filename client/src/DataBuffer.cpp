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
    buffer_.push_back(jsonChunk);
}

void DataBuffer::setData(buffer_data_t jsonData) {
    // Overwrite the entire buffer with new raw JSON data
    buffer_.clear();
    buffer_.push_front(jsonData);
}

std::list<buffer_data_t> DataBuffer::readAll() const {
    // TODO: return raw buffer as is
    return buffer_;
}

std::list<buffer_data_t> DataBuffer::consume() {
    // TODO: copy the raw buffer then clear the buffer and return copied content
    std::list<buffer_data_t> temp = buffer_;
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
    //buffer_ is a list of buffer_data_t
    std::string res = "";

    for (auto it = buffer_.begin(); it != buffer_.end(); it++) {
        buffer_data_t item = *it;
        res += toString(item);

        //add a comma and a new line after every element
        auto next = std::next(it);
        if (next != buffer_.end()) {
            res += ",\n";
        }
    }

    return res;
}

void DataBuffer::exportBuffer() {
    //Export the entire buffer (make a local JSON file under client/src/)
    FILE* fp = fopen("../src/exported_buffer.json", "w");
    if (!fp) {
		std::cerr << "Could not open file for writing exported buffer." << std::endl;
        return;
    }
    
    std::string json_content = "[\n" + toStringAll() + "\n]";
    fputs(json_content, fp);
    fclose(fp);
    return;
}

