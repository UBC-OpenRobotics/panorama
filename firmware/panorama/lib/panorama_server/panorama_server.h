#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include <stdint.h>

struct PanoramaPacket {
    const char* sensor;
    const char* dataunit;
    const char* datatype;
    uint16_t sensorID;
    float data;
    unsigned long timestamp;
};

struct PanoramaCommand {
    String cmd;
};

struct PanoramaServerConfig {
    const char* ssid;
    const char* password;
    uint16_t port;
};

class PanoramaServer {
public:
    PanoramaServer();

    void init(const PanoramaServerConfig& config);
    void start();
    void pollClient();
    bool isClientConnected() const;
    void sendPacket(const PanoramaPacket& packet);
    bool getCommand(PanoramaCommand& command);

private:
    PanoramaServerConfig config;
    WiFiServer* server;
    WiFiClient client;
    bool initialized;
};
