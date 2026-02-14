#include "client/config_manager.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <sstream>

ConfigManager::ConfigManager() {}

ConfigManager::~ConfigManager() {}

ConfigManager& ConfigManager::getInstance() {
    static ConfigManager instance;
    return instance;
}

bool ConfigManager::setRuntimeDirectory(const std::string& path) {
    std::lock_guard<std::mutex> lock(mutex_);

    // Verify directory exists
    if (!std::filesystem::exists(path)) {
        std::cerr << "Directory does not exist: " << path << std::endl;
        return false;
    }

    if (!std::filesystem::is_directory(path)) {
        std::cerr << "Not a directory: " << path << std::endl;
        return false;
    }

    // Test write permissions
    if (!testDirectoryWritable(path)) {
        std::cerr << "Directory is not writable: " << path << std::endl;
        return false;
    }

    runtimeDir_ = path;

    // Create subdirectories
    if (!createDirectoryStructure()) {
        std::cerr << "Failed to create directory structure" << std::endl;
        return false;
    }

    return true;
}

std::string ConfigManager::getRuntimeDirectory() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return runtimeDir_;
}

bool ConfigManager::hasRuntimeDirectory() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return !runtimeDir_.empty();
}

std::string ConfigManager::getDataLogPath() const {
    std::lock_guard<std::mutex> lock(mutex_);
    if (runtimeDir_.empty()) {
        return "";
    }
    return runtimeDir_ + "/data/tcp_data.jsonl";
}

std::string ConfigManager::getConfigPath() const {
    std::lock_guard<std::mutex> lock(mutex_);
    if (runtimeDir_.empty()) {
        return "";
    }
    return runtimeDir_ + "/config.json";
}

bool ConfigManager::testDirectoryWritable(const std::string& path) {
    std::string testFile = path + "/.panorama_test";
    std::ofstream test(testFile);
    if (!test.is_open()) {
        return false;
    }
    test.close();
    std::filesystem::remove(testFile);
    return true;
}

bool ConfigManager::createDirectoryStructure() {
    try {
        std::filesystem::create_directories(runtimeDir_ + "/data");
        std::filesystem::create_directories(runtimeDir_ + "/logs");
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error creating directories: " << e.what() << std::endl;
        return false;
    }
}

bool ConfigManager::initializeConfig() {
    std::string configPath = getConfigPath();
    if (configPath.empty()) {
        std::cerr << "Runtime directory not set" << std::endl;
        return false;
    }

    // Check if theres an existing config file, if so use that instead since this would mean
    // user loaded up a pre-exisiting project
    if (std::filesystem::exists(configPath)) {
        if (!loadFromJson()) {
            std::cerr << "Failed to load config from JSON" << std::endl;
            return false;
        }
    } else {
        // default init
        configData_["runtime_directory"] = runtimeDir_;
        configData_["tcp_host"] = "127.0.0.1";
        configData_["tcp_port"] = "3000";
        configData_["tcp_auto_reconnect"] = "true";
        configData_["tcp_reconnect_delay_sec"] = "5";

        if (!saveToJson()) {
            std::cerr << "Failed to save initial config" << std::endl;
            return false;
        }
    }

    saveConfig("runtime_directory", runtimeDir_);

    return true;
}

bool ConfigManager::loadFromJson() {
    std::string configPath = getConfigPath();
    std::ifstream file(configPath);

    if (!file.is_open()) {
        return false;
    }

    std::string line;
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();

    size_t start = content.find('{');
    size_t end = content.rfind('}');
    if (start == std::string::npos || end == std::string::npos) {
        return false;
    }

    content = content.substr(start + 1, end - start - 1);

    std::istringstream stream(content);
    std::string pair;

    while (std::getline(stream, pair, ',')) {
        size_t colonPos = pair.find(':');
        if (colonPos == std::string::npos) {
            continue;
        }

        std::string key = pair.substr(0, colonPos);
        std::string value = pair.substr(colonPos + 1);

        auto trim = [](std::string& s) {
            s.erase(0, s.find_first_not_of(" \t\n\r\""));
            s.erase(s.find_last_not_of(" \t\n\r\"") + 1);
        };

        trim(key);
        trim(value);

        if (!key.empty()) {
            configData_[key] = value;
        }
    }

    return true;
}

bool ConfigManager::saveToJson() {
    std::string configPath = getConfigPath();
    std::ofstream file(configPath);

    if (!file.is_open()) {
        std::cerr << "Failed to open config file for writing: " << configPath << std::endl;
        return false;
    }

    // Write out json
    file << "{\n";
    bool first = true;
    for (const auto& [key, value] : configData_) {
        if (!first) {
            file << ",\n";
        }
        file << "  \"" << key << "\": \"" << value << "\"";
        first = false;
    }
    file << "\n}\n";

    file.close();
    return true;
}

bool ConfigManager::saveConfig(const std::string& key, const std::string& value) {
    std::lock_guard<std::mutex> lock(mutex_);

    configData_[key] = value;
    return saveToJson();
}

std::string ConfigManager::getConfig(const std::string& key) {
    std::lock_guard<std::mutex> lock(mutex_);

    auto it = configData_.find(key);
    if (it != configData_.end()) {
        return it->second;
    }
    return "";
}

bool ConfigManager::saveTcpSettings(const std::string& host, int port, bool autoReconnect, int reconnectDelay) {
    std::lock_guard<std::mutex> lock(mutex_);

    configData_["tcp_host"] = host;
    configData_["tcp_port"] = std::to_string(port);
    configData_["tcp_auto_reconnect"] = autoReconnect ? "true" : "false";
    configData_["tcp_reconnect_delay_sec"] = std::to_string(reconnectDelay);

    return saveToJson();
}

bool ConfigManager::getTcpSettings(std::string& host, int& port, bool& autoReconnect, int& reconnectDelay) {
    std::lock_guard<std::mutex> lock(mutex_);

    auto hostIt = configData_.find("tcp_host");
    auto portIt = configData_.find("tcp_port");
    auto autoReconnectIt = configData_.find("tcp_auto_reconnect");
    auto reconnectDelayIt = configData_.find("tcp_reconnect_delay_sec");

    if (hostIt == configData_.end() || portIt == configData_.end()) {
        return false;
    }

    host = hostIt->second;
    port = std::stoi(portIt->second);
    autoReconnect = (autoReconnectIt != configData_.end() && autoReconnectIt->second == "true");
    reconnectDelay = (reconnectDelayIt != configData_.end()) ? std::stoi(reconnectDelayIt->second) : 5;

    return true;
}
