#pragma once
#include <string>
#include <vector>
#include <mutex>   // optional, if buffer must be thread-safe
#include "client/panorama_defines.hpp"

class DataBuffer {
public:
    DataBuffer();
    ~DataBuffer();

    // ---- Writing ----
    // Append raw JSON data received from server into buffer
    void writeData(const std::string& jsonChunk);

    // Optionally overwrite buffer with fresh data
    void setData(const std::string& jsonData);

    // ---- Reading ----
    // Retrieve all raw data currently stored
    std::string readAll() const;

    // Retrieve and clear buffer (useful for message-by-message processing)
    std::string consume();

    // ---- Processing (Optional) ----
    // Extract one "complete JSON object" and leave remaining partial data behind
    // e.g., useful if server streams multiple JSON objects back-to-back
    std::string extractNextJson();

    // Check if buffer currently contains a full JSON object
    bool hasCompleteJson() const;

    // ---- Buffer State ----
    // Clear the buffer fully
    void clear();

    // Return current buffer size in bytes
    size_t size() const;

private:
    // Internal raw data buffer (stores JSON text)
    std::string buffer_;

    // Optional mutex for thread safety
    // mutable std::mutex bufferMutex_;

    // Helper to detect boundaries of JSON objects
    // e.g., match braces or newline-delimited messages
    size_t findJsonBoundary() const;
};

