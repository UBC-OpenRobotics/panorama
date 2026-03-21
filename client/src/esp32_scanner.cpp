#include "client/esp32_scanner.hpp"
#include <chrono>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #define CLOSE_SOCKET closesocket
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #include <fcntl.h>
    #include <poll.h>
    #include <cerrno>
    #define CLOSE_SOCKET close
    #define INVALID_SOCKET -1
#endif

Esp32Scanner::Esp32Scanner() {}

Esp32Scanner::~Esp32Scanner() {
    stop();
}

void Esp32Scanner::start() {
    if (running_.load()) return;
    running_ = true;
    thread_ = std::thread(&Esp32Scanner::run, this);
}

void Esp32Scanner::stop() {
    running_ = false;
    if (thread_.joinable()) {
        thread_.join();
    }
}

void Esp32Scanner::setOnAvailabilityChanged(std::function<void(bool)> cb) {
    callback_ = std::move(cb);
}

void Esp32Scanner::run() {
    while (running_) {
        bool reachable = probe(ESP32_DEFAULT_HOST, ESP32_DEFAULT_PORT);
        bool prev = available_.exchange(reachable);

        if (reachable != prev && callback_) {
            callback_(reachable);
        }

        for (int i = 0; i < SCAN_INTERVAL_MS / 100 && running_; ++i) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100)); // prevent stop() blocking for too long
        }
    }
}

bool Esp32Scanner::probe(const std::string& host, int port) {
#ifdef _WIN32
    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) return false;

    // Set this as non-blocking
    u_long mode = 1;
    ioctlsocket(sock, FIONBIO, &mode);

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(host.c_str());

    connect(sock, (sockaddr*)&addr, sizeof(addr));

    fd_set writefds;
    FD_ZERO(&writefds);
    FD_SET(sock, &writefds);
    timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = CONNECT_TIMEOUT_MS * 1000;

    int result = select(0, nullptr, &writefds, nullptr, &tv);
    closesocket(sock);
    return result > 0;
#else
    int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) return false;

    // Set this as non-blocking
    int flags = fcntl(sock, F_GETFL, 0);
    fcntl(sock, F_SETFL, flags | O_NONBLOCK);

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_pton(AF_INET, host.c_str(), &addr.sin_addr);

    int ret = connect(sock, (sockaddr*)&addr, sizeof(addr));
    if (ret == 0) {
        // Connected immediately
        CLOSE_SOCKET(sock);
        return true;
    }

    if (errno != EINPROGRESS) {
        CLOSE_SOCKET(sock);
        return false;
    }

    // Wait for connection, with timeout
    struct pollfd pfd;
    pfd.fd = sock;
    pfd.events = POLLOUT;
    int pollResult = poll(&pfd, 1, CONNECT_TIMEOUT_MS);

    bool connected = false;
    if (pollResult > 0 && (pfd.revents & POLLOUT)) {
        int err = 0;
        socklen_t len = sizeof(err);
        getsockopt(sock, SOL_SOCKET, SO_ERROR, &err, &len);
        connected = (err == 0);
    }

    CLOSE_SOCKET(sock);
    return connected;
#endif
}
