#include <iostream>
#include <chrono>
#include <thread>
#include <random>

int main() {
    uint32_t seq = 0;
    const int sensor_id = 1;
    const char* sensor_name = "temperature";

    auto start = std::chrono::steady_clock::now();

    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(0.0f, 100.0f);

    while (true) {
        auto now = std::chrono::steady_clock::now();
        auto ts_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                         now - start).count();

        float value = dist(rng);

        std::cout
            << "{"
            << "\"sensor\":\"" << sensor_name << "\","
            << "\"sensor_id\":" << sensor_id << ","
            << "\"seq\":" << seq++ << ","
            << "\"timestamp_ms\":" << ts_ms << ","
            << "\"value\":" << value
            << "}"
            << std::endl;

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    return 0;
}
