#pragma once
#include <string>

namespace logger {
    class Sink {
    public:
        virtual ~Sink() = default;
        virtual bool write(const std::string& formatted_message) = 0;
    };
}