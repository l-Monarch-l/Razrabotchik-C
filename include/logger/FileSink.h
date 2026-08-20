#pragma once
#include "Sink.h"
#include <fstream>
#include <mutex>
#include <string>

namespace logger {
    class FileSink:public Sink {
    public: 
        explicit FileSink(const std::string& filename);
        ~FileSink() override = default;
        bool write(const std::string& formatted_message) override;
        
    private:
        std::ofstream file_;
        std::mutex write_mutex_;
    };
}