#include "client/config_manager.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <cstdlib>

ConfigManager::ConfigManager() : db_(nullptr) {}

ConfigManager::~ConfigManager() {
    if (db_) {
        sqlite3_close(db_);
        db_ = nullptr;
    }
}

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

std::string ConfigManager::getDatabasePath() const {
    std::lock_guard<std::mutex> lock(mutex_);
    if (runtimeDir_.empty()) {
        return "";
    }
    return runtimeDir_ + "/config.db";
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

bool ConfigManager::initializeDatabase() {
    std::string dbPath = getDatabasePath();
    if (dbPath.empty()) {
        std::cerr << "Runtime directory not set" << std::endl;
        return false;
    }

    // Open database
    int rc = sqlite3_open(dbPath.c_str(), &db_);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to open database: " << sqlite3_errmsg(db_) << std::endl;
        return false;
    }

    // Enable thread-safety
    sqlite3_busy_timeout(db_, 5000);

    // Create schema
    const char* schema = R"(
        CREATE TABLE IF NOT EXISTS config (
            key TEXT PRIMARY KEY,
            value TEXT NOT NULL,
            updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
        );

        CREATE TABLE IF NOT EXISTS tcp_settings (
            id INTEGER PRIMARY KEY CHECK (id = 1),
            host TEXT NOT NULL DEFAULT '127.0.0.1',
            port INTEGER NOT NULL DEFAULT 3000,
            auto_reconnect BOOLEAN DEFAULT 1,
            reconnect_delay_sec INTEGER DEFAULT 5
        );

        CREATE TABLE IF NOT EXISTS logging_settings (
            id INTEGER PRIMARY KEY CHECK (id = 1),
            max_file_size_mb INTEGER DEFAULT 10,
            max_file_duration_min INTEGER DEFAULT 60,
            log_format TEXT DEFAULT 'jsonl'
        );

        CREATE TABLE IF NOT EXISTS sensors (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT NOT NULL,
            type TEXT NOT NULL,
            enabled BOOLEAN DEFAULT 1,
            color TEXT,
            unit TEXT
        );

        CREATE TABLE IF NOT EXISTS metadata (
            key TEXT PRIMARY KEY,
            value TEXT
        );

        INSERT OR IGNORE INTO metadata (key, value) VALUES
            ('db_version', '1.0'),
            ('created_at', datetime('now'));

        INSERT OR IGNORE INTO tcp_settings (id, host, port) VALUES (1, '127.0.0.1', 3000);
        INSERT OR IGNORE INTO logging_settings (id) VALUES (1);
    )";

    if (!executeSql(schema)) {
        return false;
    }

    // Save runtime directory to config
    saveConfig("runtime_directory", runtimeDir_);

    return true;
}

bool ConfigManager::executeSql(const std::string& sql) {
    char* errMsg = nullptr;
    int rc = sqlite3_exec(db_, sql.c_str(), nullptr, nullptr, &errMsg);

    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }

    return true;
}

bool ConfigManager::saveConfig(const std::string& key, const std::string& value) {
    std::lock_guard<std::mutex> lock(mutex_);

    if (!db_) {
        return false;
    }

    const char* sql = "INSERT OR REPLACE INTO config (key, value, updated_at) VALUES (?, ?, datetime('now'))";
    sqlite3_stmt* stmt;

    int rc = sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db_) << std::endl;
        return false;
    }

    sqlite3_bind_text(stmt, 1, key.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, value.c_str(), -1, SQLITE_TRANSIENT);

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return rc == SQLITE_DONE;
}

std::string ConfigManager::getConfig(const std::string& key) {
    std::lock_guard<std::mutex> lock(mutex_);

    if (!db_) {
        return "";
    }

    const char* sql = "SELECT value FROM config WHERE key = ?";
    sqlite3_stmt* stmt;
    std::string result;

    int rc = sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        return "";
    }

    sqlite3_bind_text(stmt, 1, key.c_str(), -1, SQLITE_TRANSIENT);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        const char* value = (const char*)sqlite3_column_text(stmt, 0);
        if (value) {
            result = value;
        }
    }

    sqlite3_finalize(stmt);
    return result;
}

bool ConfigManager::saveTcpSettings(const std::string& host, int port, bool autoReconnect, int reconnectDelay) {
    std::lock_guard<std::mutex> lock(mutex_);

    if (!db_) {
        return false;
    }

    const char* sql = "INSERT OR REPLACE INTO tcp_settings (id, host, port, auto_reconnect, reconnect_delay_sec) VALUES (1, ?, ?, ?, ?)";
    sqlite3_stmt* stmt;

    int rc = sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_text(stmt, 1, host.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 2, port);
    sqlite3_bind_int(stmt, 3, autoReconnect ? 1 : 0);
    sqlite3_bind_int(stmt, 4, reconnectDelay);

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return rc == SQLITE_DONE;
}

bool ConfigManager::getTcpSettings(std::string& host, int& port, bool& autoReconnect, int& reconnectDelay) {
    std::lock_guard<std::mutex> lock(mutex_);

    if (!db_) {
        return false;
    }

    const char* sql = "SELECT host, port, auto_reconnect, reconnect_delay_sec FROM tcp_settings WHERE id = 1";
    sqlite3_stmt* stmt;

    int rc = sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        return false;
    }

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        const char* hostStr = (const char*)sqlite3_column_text(stmt, 0);
        if (hostStr) {
            host = hostStr;
        }
        port = sqlite3_column_int(stmt, 1);
        autoReconnect = sqlite3_column_int(stmt, 2) != 0;
        reconnectDelay = sqlite3_column_int(stmt, 3);

        sqlite3_finalize(stmt);
        return true;
    }

    sqlite3_finalize(stmt);
    return false;
}
