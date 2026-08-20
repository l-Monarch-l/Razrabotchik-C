#include "logger/Logger.h"
#include "logger/FileSink.h"
#include <cassert>
#include <fstream>
#include <thread>
#include <vector>
#include <iostream>

using namespace logger;

void test_file_sink() {
    std::remove("test_output.log");

    auto sink = std::make_unique<FileSink>("test_output.log");
    Logger logger;
    assert(logger.init(std::move(sink), LogLevel::INFO));

    logger.log(LogLevel::DEBUG, "debug msg");
    logger.log(LogLevel::INFO,  "info msg");
    logger.log(LogLevel::ERROR, "error msg");

    std::ifstream file("test_output.log");
    std::string line;
    int count = 0;
    while (std::getline(file, line)) {
        count++;
        if (count == 1) assert(line.find("[INFO]") != std::string::npos);
        if (count == 2) assert(line.find("[ERROR]") != std::string::npos);
    }
    assert(count == 2);
    std::cout << "test_file_sink прошёл" << std::endl;
}

void test_level_change() {
    std::remove("test_output.log");
    auto sink = std::make_unique<FileSink>("test_output.log");
    Logger logger;
    logger.init(std::move(sink), LogLevel::ERROR);
    logger.log(LogLevel::INFO, "info should not appear");
    logger.setLevel(LogLevel::DEBUG);
    logger.log(LogLevel::DEBUG, "debug after change");

    std::ifstream file("test_output.log");
    std::string line;
    int count = 0;
    while (std::getline(file, line)) count++;
    assert(count == 1);
    std::cout << "test_level_change прошёл" << std::endl;
}

void test_multithreading() {
    std::remove("test_output.log");
    auto sink = std::make_unique<FileSink>("test_output.log");
    Logger logger;
    logger.init(std::move(sink), LogLevel::DEBUG);

    std::vector<std::thread> threads;
    for (int i = 0; i < 10; ++i) {
        threads.emplace_back([&logger, i]() {
            logger.log(LogLevel::INFO, "Thread " + std::to_string(i));
        });
    }
    for (auto& t : threads) t.join();

    std::ifstream file("test_output.log");
    int count = 0;
    std::string line;
    while (std::getline(file, line)) count++;
    assert(count == 10);
    std::cout << "test_multithreading прошёл" << std::endl;
}

int main() {
    test_file_sink();
    test_level_change();
    test_multithreading();
    std::cout << "все тесты пройдены" << std::endl;
    return 0;
}