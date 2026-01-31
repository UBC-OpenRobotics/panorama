#include "client/data_logger.hpp"
#include <iostream>
#include <chrono>
#include <iomanip>

DataLogger::DataLogger(const std::string& logFilePath)
    : logFilePath_(logFilePath) {
    openLogFile();
}

DataLogger::~DataLogger() {
    close();
}

bool DataLogger::openLogFile() {
    std::lock_guard<std::mutex> lock(fileMutex_);

    logFile_.open(logFilePath_, std::ios::out | std::ios::app);

    if (!logFile_.is_open()) {
        std::cerr << "Failed to open log file: " << logFilePath_ << std::endl;
        return false;
    }

    logFile_ << std::unitbuf; // Flush to init 

    std::cout << "Data logger initialized: " << logFilePath_ << std::endl;
    return true;
}

bool DataLogger::logJsonData(const std::string& jsonStr) {
    std::lock_guard<std::mutex> lock(fileMutex_);

    if (!logFile_.is_open()) {
        std::cerr << "Log file is not open" << std::endl;
        return false;
    }

    try {
        // Get current timestamp
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()) % 1000;

        std::tm tm; // Timestamp and json are one oneline for json format
        #ifdef _WIN32
            localtime_s(&tm, &time);
        #else
            localtime_r(&time, &tm);
        #endif

        logFile_ << std::put_time(&tm, "%Y-%m-%dT%H:%M:%S")
                 << '.' << std::setfill('0') << std::setw(3) << ms.count()
                 << " " << jsonStr << std::endl;

        // Check for errors
        if (logFile_.fail()) {
            std::cerr << "Error writing to log file" << std::endl;
            return false;
        }

        return true;
    } catch (const std::exception& e) {
        std::cerr << "Exception while logging data: " << e.what() << std::endl;
        return false;
    }
}

bool DataLogger::isOpen() const {
    std::lock_guard<std::mutex> lock(fileMutex_);
    return logFile_.is_open();
}

void DataLogger::close() {
    std::lock_guard<std::mutex> lock(fileMutex_);
    if (logFile_.is_open()) {
        logFile_.close();
    }
}
