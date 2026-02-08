#pragma once

#include <string>
#include <fstream>
#include <mutex>

class DataLogger {
public:
    explicit DataLogger(const std::string& logFilePath);
    ~DataLogger();

    // Delete copy constructor and assignment operator
    DataLogger(const DataLogger&) = delete;
    DataLogger& operator=(const DataLogger&) = delete;

    // Log raw JSON string received from TCP
    bool logJsonData(const std::string& jsonStr);

    // Check if logger is ready
    bool isOpen() const;

    // Close the log file
    void close();

    std::string getLogFilePath();
    

private:
    std::string logFilePath_;
    std::ofstream logFile_;
    mutable std::mutex fileMutex_;

    bool openLogFile();
};
