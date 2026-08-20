#include "logger/Logger.h"
#include <chrono>
#include <iomanip>
#include <sstream>
#include <iostream>

namespace logger {

static std::string current_time() {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    std::tm tm;
    localtime_r(&in_time_t, &tm);
    std::stringstream ss;
    ss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

bool Logger::init(std::unique_ptr<Sink> sink, LogLevel default_level) {
    if (!sink) {
        std::cerr << "[Logger] init: sink is nullptr" << std::endl;
        return false;
    }
    sink_ = std::move(sink);
    default_level_ = default_level;
    return true;
}

void Logger::setLevel(LogLevel level) {
    default_level_ = level;
}

bool Logger::log(LogLevel level, const std::string& message) {
    if (level < default_level_.load()) {
        return true;
    }

    std::string level_str;
    switch (level) {
        case LogLevel::DEBUG: level_str = "DEBUG"; break;
        case LogLevel::INFO:  level_str = "INFO";  break;
        case LogLevel::ERROR: level_str = "ERROR"; break;
        default: level_str = "UNKNOWN"; break;
    }

    std::string formatted = "[" + current_time() + "] [" + level_str + "] " + message;

    std::lock_guard<std::mutex> lock(write_mutex_);
    return sink_->write(formatted);
}

}