#include "logger/SocketSink.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <iostream>

namespace logger {
    SocketSink::SocketSink(const std::string& server_ip, uint16_t port)
        : sock_fd_(-1), connected_(false) {
        sock_fd_ = socket(AF_INET, SOCK_STREAM, 0);
        if (sock_fd_ < 0) {
            std::cerr << "[SocketSink] ошибка создания сокета" << std::endl;
            return;
        }

        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);

        if (inet_pton(AF_INET, server_ip.c_str(), &addr.sin_addr) <= 0) {
            std::cerr << "[SocketSink] неверный IP adадресс" << std::endl;
            close_socket();
            return;
        }

        if (connect(sock_fd_, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
            std::cerr << "[SocketSink] ошибка подключения" << std::endl;
            close_socket();
            return;
        }
        connected_ = true;
    }

    SocketSink::~SocketSink() {
        close_socket();
    }

    bool SocketSink::write(const std::string& formatted_message) {
        if (!connected_ || sock_fd_ < 0) {
            return false;
        }
        std::lock_guard<std::mutex> lock(write_mutex_);
        std::string msg = formatted_message + '\n';
        ssize_t sent = send(sock_fd_, msg.c_str(), msg.size(), 0);
        if (sent < 0 || static_cast<size_t>(sent) != msg.size()) {
            return false;
        }
        return true;
    }

    void SocketSink::close_socket() {
        if (sock_fd_ >= 0) {
            close(sock_fd_);
            sock_fd_ = -1;
        }
        connected_ = false;
    }
}