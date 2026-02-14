#pragma once

#include <string>
#include <mutex>
#include <map>

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

    // Config operations
    bool initializeConfig();
    bool saveConfig(const std::string& key, const std::string& value);
    std::string getConfig(const std::string& key);

    // Path helpers
    std::string getDataLogPath() const;
    std::string getConfigPath() const;

    // TCP settings
    bool saveTcpSettings(const std::string& host, int port, bool autoReconnect, int reconnectDelay);
    bool getTcpSettings(std::string& host, int& port, bool& autoReconnect, int& reconnectDelay);

    // Cleanup
    ~ConfigManager();

private:
    ConfigManager();

    std::string runtimeDir_;
    std::map<std::string, std::string> configData_;
    mutable std::mutex mutex_;

    bool createDirectoryStructure();
    bool testDirectoryWritable(const std::string& path);
    bool loadFromJson();
    bool saveToJson();
};
