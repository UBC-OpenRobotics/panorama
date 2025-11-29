#pragma once
#include <string>
#include <vector>
#include <mutex>
#include "common/panorama_defines.hpp"

class DataBuffer {
public:
    DataBuffer();
    ~DataBuffer();

    // ---------------------------
    //       BUFFER HANDLING
    // ---------------------------

    // Append new raw JSON data received from server
    void writeData(const std::string& jsonChunk);
    
    // Replace buffer entirely with fresh raw JSON
    void setData(const std::string& jsonData);

    // Return the entire buffer content (without clearing)
    std::string readAll() const;

    // Return and clear the buffer (consume)
    std::string consume();

    // Does the buffer currently contain a complete JSON object?
    bool hasCompleteJson() const;

    // Return buffer size in bytes
    size_t size() const;

    // Clear the buffer
    void clear();

    // Extract the next complete JSON object from buffer
    // Removes extracted portion from buffer
    std::string extractNextJson();


    // ---------------------------
    //        JSON PARSING
    // ---------------------------

    // Parse the next complete JSON object in the buffer.
    // Returns success/failure depending on whether parsing succeeded.
    // The parsed result can be returned as a variant, struct, or any user-defined type.
    bool parseNextJson(/* ParsedData &out */);

    // Parse *all* complete JSON objects currently in the buffer.
    // Useful if the buffer contains multiple messages.
    void parseAll(/* std::vector<ParsedData> &out */);

private:
    // Raw buffer storing incoming data
    std::string buffer_;

    // ---------------------------
    //      JSON HELPER LOGIC
    // ---------------------------

    // Find end of next valid JSON object inside buffer
    // E.g., matching braces or newline delimiter depending on protocol
    size_t findJsonBoundary() const;

    // Validate that a substring is a valid JSON object
    // Useful to avoid partial or corrupted parsing attempts
    bool isValidJson(const std::string& jsonStr) const;

    // Convert a JSON string into structured data (using a library of your choice)
    // Example: convert jsonStr → ParsedData type
    // This keeps parsing logic isolated from buffer logic
    bool decodeJson(const std::string& jsonStr /*, ParsedData &out */);
};
