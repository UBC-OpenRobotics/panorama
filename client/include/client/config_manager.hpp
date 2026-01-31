#pragma once

#include <string>
#include <mutex>
#include <sqlite3.h>

class ConfigManager {
public:
    // Get singleton instance
    static ConfigManager& getInstance();

    // Delete copy constructor and assignment operator
    ConfigManager(const ConfigManager&) = delete;
    ConfigManager& operator=(const ConfigManager&) = delete;

    // Directory management
    bool setRuntimeDirectory(const std::string& path);
    std::string getRuntimeDirectory() const;
    bool hasRuntimeDirectory() const;

    // Database operations
    bool initializeDatabase();
    bool saveConfig(const std::string& key, const std::string& value);
    std::string getConfig(const std::string& key);

    // Path helpers
    std::string getDataLogPath() const;
    std::string getDatabasePath() const;

    // TCP settings
    bool saveTcpSettings(const std::string& host, int port, bool autoReconnect, int reconnectDelay);
    bool getTcpSettings(std::string& host, int& port, bool& autoReconnect, int& reconnectDelay);

    // Cleanup
    ~ConfigManager();

private:
    ConfigManager();

    std::string runtimeDir_;
    sqlite3* db_;
    mutable std::mutex mutex_;

    bool createDirectoryStructure();
    bool testDirectoryWritable(const std::string& path);
    bool executeSql(const std::string& sql);
};
