#include "client/tcp_client.hpp"
#include "client/message_model.hpp"
#include "client/DataBuffer.hpp"
#include "common/panorama_utils.hpp"
#include <cstring>
#include <chrono>

#include "client/json_reader.hpp"

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
    typedef int socklen_t;
    #define SOCKET_ERROR_CODE WSAGetLastError()
    #define CLOSE_SOCKET closesocket
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #define SOCKET_ERROR_CODE errno
    #define CLOSE_SOCKET close
    #define INVALID_SOCKET -1
    #define SOCKET_ERROR -1
#endif

TcpClient::TcpClient(const std::string& host, int port, std::shared_ptr<MessageModel> model, std::shared_ptr<DataBuffer> dataBuffer)
    : host_(host), port_(port), model_(model), dataBuffer_(dataBuffer), running_(false), socket_(INVALID_SOCKET) {
#ifdef _WIN32
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif
}

TcpClient::~TcpClient() {
    stop();
#ifdef _WIN32
    WSACleanup();
#endif
}

void TcpClient::start() {
    running_ = true;
    clientThread_ = std::thread(&TcpClient::run, this);
}

void TcpClient::stop() {
    running_ = false;
    if (socket_ != INVALID_SOCKET) {
        CLOSE_SOCKET(socket_);
        socket_ = INVALID_SOCKET;
    }
    if (clientThread_.joinable()) {
        clientThread_.join();
    }
}

void TcpClient::cleanup() {
    if (socket_ != INVALID_SOCKET) {
        CLOSE_SOCKET(socket_);
        socket_ = INVALID_SOCKET;
    }
}

void TcpClient::run() {
    while (running_) {
        reconnect();
        
        if (socket_ == INVALID_SOCKET) {
            // Wait before retrying
            std::this_thread::sleep_for(std::chrono::seconds(5));
            continue;
        }
        JsonReader reader = JsonReader();
        // Read data from server
        char buffer[4096];
        while (running_) {
            int bytesRead = recv(socket_, buffer, sizeof(buffer) - 1, 0);
            if (bytesRead <= 0) {
                model_->addMessage("Disconnected from server");
                cleanup();
                break;
            }

            buffer[bytesRead] = '\0';
            std::string received(buffer);

            // Parse JSON and write to DataBuffer
            buffer_data_t parsedData = reader.exportToBuffer(received);
            dataBuffer_->writeData(parsedData);
            model_->addMessage("Data" + std::to_string(dataBuffer_->size()));
            model_->addMessage("Received: " + received);
        }
    }
}

void TcpClient::reconnect() {
    cleanup();
    
    socket_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (socket_ == INVALID_SOCKET) {
        model_->addMessage("Error: Failed to create socket");
        return;
    }
    
    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port_);
    
#ifdef _WIN32
    serverAddr.sin_addr.s_addr = inet_addr(host_.c_str());
#else
    inet_pton(AF_INET, host_.c_str(), &serverAddr.sin_addr);
#endif
    
    model_->addMessage("Connecting to " + host_ + ":" + std::to_string(port_) + "...");
    
    if (connect(socket_, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        model_->addMessage("Error: Failed to connect to server");
        cleanup();
        return;
    }
    
    model_->addMessage("Connected to server");
}