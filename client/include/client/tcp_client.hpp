#pragma once
#include <string>
#include <atomic>
#include <thread>
#include <memory>

class MessageModel;

class TcpClient {
public:
    TcpClient(const std::string& host, int port, std::shared_ptr<MessageModel> model);
    ~TcpClient();
    
    void start();
    void stop();
    
private:
    void run();
    void reconnect();
    void cleanup();
    
    std::string host_;
    int port_;
    std::shared_ptr<MessageModel> model_;
    std::atomic<bool> running_;
    std::thread clientThread_;
    
#ifdef _WIN32
    unsigned long long socket_; // SOCKET type on Windows
#else
    int socket_;
#endif
};