#include "panorama_server.h"

PanoramaServer::PanoramaServer()
    : config{"", "", 0}, server(nullptr), client(), initialized(false) {}

void PanoramaServer::init(const PanoramaServerConfig& serverConfig) {
    config = serverConfig;

    if (server != nullptr) {
        delete server;
        server = nullptr;
    }

    WiFi.mode(WIFI_AP);
    WiFi.softAP(config.ssid, config.password);

    server = new WiFiServer(config.port);

    IPAddress ip = WiFi.softAPIP();
    Serial.printf("AP started: %s (%s)\n", config.ssid, ip.toString().c_str());
    initialized = true;
}

void PanoramaServer::start() {
    if (!initialized || server == nullptr) {
        return;
    }

    server->begin();
    server->setNoDelay(true);
}

void PanoramaServer::pollClient() {
    if (!initialized || server == nullptr) {
        return;
    }

    WiFiClient newClient = server->available();
    if (newClient) {
        if (client && client.connected()) {
            client.stop();
        }

        client = newClient;
        client.print(F("{\"type\":\"status\",\"msg\":\"connected\"}\n"));
        Serial.println("Backend connected");
    }
}

bool PanoramaServer::isClientConnected() const {
    return client && client.connected();
}

void PanoramaServer::sendPacket(const PanoramaPacket& packet) {
    if (!isClientConnected()) {
        return;
    }

    String json =
        "{"
            "\"sensor\":\"" + String(packet.sensor) + "\","
            "\"dataunit\":\"" + String(packet.dataunit) + "\","
            "\"data\":" + String(packet.data, 2) + ","
            "\"datatype\":\"" + String(packet.datatype) + "\","
            "\"sensorID\":" + String(packet.sensorID) + ","
            "\"timestamp\":" + String(packet.timestamp) +
        "}\n";

    client.print(json);
    Serial.print("Sent packet: ");
    Serial.print(json);
}

bool PanoramaServer::getCommand(PanoramaCommand& command) {
    if (!isClientConnected() || !client.available()) {
        return false;
    }

    command.cmd = client.readStringUntil('\n');
    command.cmd.trim();
    command.cmd.toUpperCase();

    Serial.println("Received command: " + command.cmd);
    return true;
}
