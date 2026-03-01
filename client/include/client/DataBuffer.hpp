#ifndef __DATABUFFER__
#define __DATABUFFER__
#include <string>
#include <vector>
#include <mutex>
#include <list>
#include "common/panorama_defines.hpp"
#include "client/buffer_base.hpp"
#include "client/data_logger.hpp"

class DataBuffer : public BufferBase<buffer_data_t> {
public:
    DataBuffer(const std::string& logFilePath);
    ~DataBuffer();

    // ---------------------------
    //       BUFFER HANDLING
    // ---------------------------

    // Append new raw JSON data received from server
    void writeData(buffer_data_t jsonChunk);
    
    // Replace buffer entirely with fresh raw JSON
    void setData(buffer_data_t jsonData);

    // Return the entire buffer content (without clearing)
    std::list<buffer_data_t> readAll() const;

    // Return and clear the buffer (consume)
    std::list<buffer_data_t> consume();

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
    std::string parseNextJson(/* ParsedData &out */);

    // Parse *all* complete JSON objects currently in the buffer.
    // Useful if the buffer contains multiple messages.
    void parseAll(/* std::vector<ParsedData> &out */);

    std::string toString(const buffer_data_t& item);

    std::string toStringAll();

    void printAll();

    void exportBuffer(std::string exportPath);

private:
    // Raw buffer storing incoming data
    std::list<buffer_data_t> buffer_;
    std::string logFilePath_;

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
#endif // __DATABUFFER__