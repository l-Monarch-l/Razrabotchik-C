#include "logger/Logger.h"
#include "logger/FileSink.h"
#include <iostream>
#include <thread>
#include <chrono>

int main() {
    using namespace logger;

    auto sink = std::make_unique<FileSink>("test.log");
    Logger logger;
    if (!logger.init(std::move(sink), LogLevel::INFO)) {
        std::cerr << "Failed to initialize logger" << std::endl;
        return 1;
    }

    logger.log(LogLevel::DEBUG, "Это сообщение DEBUG");
    logger.log(LogLevel::INFO,  "Информационное сообщение");
    logger.log(LogLevel::ERROR, "Сообщение об ошибке");

    logger.setLevel(LogLevel::DEBUG);
    logger.log(LogLevel::DEBUG, "Теперь DEBUG записывается");

    std::cout << "Логи записаны в test.log" << std::endl;
    return 0;
}