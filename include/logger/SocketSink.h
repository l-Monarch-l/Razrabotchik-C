#pragma once

#include "Sink.h"
#include <string>
#include <mutex>

namespace logger {
    class SocketSink : public Sink {
    public:
        SocketSink(const std::string& server_ip, uint16_t port);
        ~SocketSink() override;
        bool write(const std::string& formatted_message) override;

    private:
        int sock_fd_;
        std::mutex write_mutex_;
        bool connected_;
        bool connect_to_server();
        void close_socket();
    };

}