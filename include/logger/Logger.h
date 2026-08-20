#pragma once
#include "LogLevel.h"
#include "Sink.h"
#include <memory>
#include <atomic>
#include <mutex>
#include <string>

namespace logger {
    class Logger {
    public:
        Logger() = default;
        ~Logger() = default;

        bool init(std::unique_ptr<Sink> sink, LogLevel default_level);
        void setLevel(LogLevel level);
        bool log(LogLevel level, const std::string& message);

    private:
        std::unique_ptr<Sink> sink_;
        std::atomic<LogLevel> default_level_;
        std::mutex write_mutex_;
    };
}