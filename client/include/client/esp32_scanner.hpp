#pragma once

#include <string>
#include <atomic>
#include <thread>
#include <functional>

class Esp32Scanner {
public:
    static constexpr const char* ESP32_DEFAULT_HOST = "192.168.4.1";
    static constexpr int ESP32_DEFAULT_PORT = 9000;
    static constexpr int SCAN_INTERVAL_MS = 5000;
    static constexpr int CONNECT_TIMEOUT_MS = 500;

    Esp32Scanner();
    ~Esp32Scanner();

    void start();
    void stop();

    bool isAvailable() const { return available_.load(); }

    // Callback is fired on the scanner thread when the availability changes.
    // The bool parameter is true when the ESP32 becomes reachable.
    void setOnAvailabilityChanged(std::function<void(bool)> cb);

private:
    void run();
    bool probe(const std::string& host, int port);

    std::atomic<bool> running_{false};
    std::atomic<bool> available_{false};
    std::thread thread_;
    std::function<void(bool)> callback_;
};
