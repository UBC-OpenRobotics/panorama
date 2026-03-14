#include <chrono>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")
using SocketType = SOCKET;
#else
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
using SocketType = int;
#endif

namespace {
const char *ESP_IP = "192.168.4.1";
const uint16_t PORT = 9000;
const int BUFFER_SIZE = 1024;
const int RUN_SECONDS = 15;

void closeSocket(SocketType sock) {
#ifdef _WIN32
    closesocket(sock);
#else
    close(sock);
#endif
}

bool initSockets() {
#ifdef _WIN32
    WSADATA wsaData;
    return WSAStartup(MAKEWORD(2, 2), &wsaData) == 0;
#else
    return true;
#endif
}

void cleanupSockets() {
#ifdef _WIN32
    WSACleanup();
#endif
}

std::string extractJsonString(const std::string &json, const std::string &key) {
    const std::string pattern = "\"" + key + "\":\"";
    size_t start = json.find(pattern);
    if (start == std::string::npos) {
        return "";
    }

    start += pattern.size();
    size_t end = json.find('"', start);
    if (end == std::string::npos) {
        return "";
    }

    return json.substr(start, end - start);
}

bool extractJsonBool(const std::string &json, const std::string &key, bool &value) {
    const std::string pattern = "\"" + key + "\":";
    size_t start = json.find(pattern);
    if (start == std::string::npos) {
        return false;
    }

    start += pattern.size();
    if (json.compare(start, 4, "true") == 0) {
        value = true;
        return true;
    }
    if (json.compare(start, 5, "false") == 0) {
        value = false;
        return true;
    }
    return false;
}

bool extractJsonFloat(const std::string &json, const std::string &key, float &value) {
    const std::string pattern = "\"" + key + "\":";
    size_t start = json.find(pattern);
    if (start == std::string::npos) {
        return false;
    }

    start += pattern.size();
    size_t end = json.find_first_of(",}\r\n", start);
    std::string numberText = json.substr(start, end - start);
    std::stringstream parser(numberText);
    parser >> value;
    return !parser.fail();
}

void printUltrasonicPacket(const std::string &packet) {
    std::string sensor = extractJsonString(packet, "sensor");
    if (sensor != "ultrasonic") {
        std::cout << "Packet: " << packet;
        return;
    }

    std::string transport = extractJsonString(packet, "transport");
    bool ok = false;
    float distanceCm = 0.0f;
    bool hasOk = extractJsonBool(packet, "ok", ok);
    bool hasDistance = extractJsonFloat(packet, "value_cm", distanceCm);

    std::cout << "Ultrasonic";
    if (!transport.empty()) {
        std::cout << " [" << transport << "]";
    }

    if (hasOk && ok && hasDistance) {
        std::cout << " distance: " << std::fixed << std::setprecision(2) << distanceCm << " cm\n";
        return;
    }

    if (hasOk && !ok) {
        std::cout << " distance: no echo received\n";
        return;
    }

    std::cout << " raw: " << packet;
}
}  // namespace

int main() {
    if (!initSockets()) {
        std::cerr << "Failed to initialize sockets\n";
        return 1;
    }

    SocketType sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock < 0) {
        std::cerr << "Failed to create UDP socket\n";
        cleanupSockets();
        return 1;
    }

    timeval timeout{};
    timeout.tv_sec = 2;
    timeout.tv_usec = 0;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO,
               reinterpret_cast<const char *>(&timeout),
               sizeof(timeout));

    sockaddr_in espAddr{};
    espAddr.sin_family = AF_INET;
    espAddr.sin_port = htons(PORT);
    inet_pton(AF_INET, ESP_IP, &espAddr.sin_addr);

    auto sendCommand = [&](const std::string &cmd) {
        int sent = sendto(sock,
                          cmd.c_str(),
                          static_cast<int>(cmd.size()),
                          0,
                          reinterpret_cast<const sockaddr *>(&espAddr),
                          sizeof(espAddr));
        if (sent < 0) {
            std::cerr << "Failed to send command: " << cmd;
        } else {
            std::cout << "Sent: " << cmd;
        }
    };

    char buffer[BUFFER_SIZE];

    sendCommand("MODE UDP\n");
    int received = recvfrom(sock, buffer, BUFFER_SIZE - 1, 0, nullptr, nullptr);
    if (received > 0) {
        buffer[received] = '\0';
        std::cout << "Reply: " << buffer;
    } else {
        std::cout << "No MODE UDP ack received\n";
    }

    sendCommand("START 5\n");

    auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(RUN_SECONDS);
    while (std::chrono::steady_clock::now() < deadline) {
        received = recvfrom(sock, buffer, BUFFER_SIZE - 1, 0, nullptr, nullptr);
        if (received <= 0) {
            continue;
        }

        buffer[received] = '\0';
        printUltrasonicPacket(buffer);
    }

    sendCommand("STOP\n");
    sendCommand("END\n");

    closeSocket(sock);
    cleanupSockets();
    return 0;
}
