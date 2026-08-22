#include "logger/Logger.h"
#include "logger/FileSink.h"
#include "logger/SocketSink.h"
#include "ThreadSafeQueue.h"
#include <iostream>
#include <thread>
#include <atomic>
#include <string>
#include <sstream>
#include <cstdlib>
#include <memory>

using namespace logger;

struct LogEntry {
    LogLevel level;
    std::string message;
};

LogLevel parseLevel(const std::string& s) {
    if (s == "DEBUG") return LogLevel::DEBUG;
    if (s == "INFO")  return LogLevel::INFO;
    if (s == "ERROR") return LogLevel::ERROR;
    return LogLevel::INFO;
}

std::pair<LogLevel, std::string> parseInput(const std::string& line, LogLevel defaultLevel) {
    if (line.size() >= 7 && line[0] == '[') {
        size_t end = line.find(']');
        if (end != std::string::npos) {
            std::string tag = line.substr(1, end - 1);
            std::string message = line.substr(end + 1);
            if (!message.empty() && message[0] == ' ') message.erase(0, 1);
            if (tag == "DEBUG") return {LogLevel::DEBUG, message};
            if (tag == "INFO")  return {LogLevel::INFO, message};
            if (tag == "ERROR") return {LogLevel::ERROR, message};
        }
    }
    return {defaultLevel, line};
}

void printUsage(const char* prog) {
    std::cerr << "Usage:\n"
              << "  " << prog << " <log_file> <DEBUG|INFO|ERROR>          (file mode)\n"
              << "  " << prog << " --socket <ip> <port> <DEBUG|INFO|ERROR> (socket mode)\n"
              << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        printUsage(argv[0]);
        return 1;
    }

    // Определяем режим и параметры
    bool socketMode = false;
    std::string ip;
    uint16_t port = 0;
    std::string logFile;
    std::string defaultLevelStr;

    if (std::string(argv[1]) == "--socket") {
        if (argc < 5) {
            printUsage(argv[0]);
            return 1;
        }
        socketMode = true;
        ip = argv[2];
        port = static_cast<uint16_t>(std::stoi(argv[3]));
        defaultLevelStr = argv[4];
    } else {
        logFile = argv[1];
        defaultLevelStr = argv[2];
    }

    LogLevel defaultLevel = parseLevel(defaultLevelStr);

    // Создаём соответствующий Sink
    std::unique_ptr<Sink> sink;
    if (socketMode) {
        sink = std::make_unique<SocketSink>(ip, port);
        std::cout << "Socket mode: подключение к " << ip << ":" << port << std::endl;
    } else {
        sink = std::make_unique<FileSink>(logFile);
        std::cout << "File mode: регистрация " << logFile << std::endl;
    }

    Logger logger;
    if (!logger.init(std::move(sink), defaultLevel)) {
        std::cerr << "Ошибка инициализации logger" << std::endl;
        return 1;
    }

    ThreadSafeQueue<LogEntry> queue;
    std::atomic<bool> stop{false};

    std::thread worker([&logger, &queue, &stop]() {
        while (!stop.load()) {
            LogEntry entry;
            if (queue.pop(entry)) {
                if (!logger.log(entry.level, entry.message)) {
                    std::cerr << "Ошибка записи лога" << std::endl;
                }
            } else {
                break;
            }
        }
    });

    std::cout << "Приложение запущено. Введите сообщение с [DEBUG], [INFO], [ERROR] префикс или уровень по умолчанию.\n";
    std::cout << "Введие 'exit' для выхода" << std::endl;

    std::string line;
    while (std::getline(std::cin, line)) {
        // Trim whitespace
        line.erase(0, line.find_first_not_of(" \t\n\r"));
        line.erase(line.find_last_not_of(" \t\n\r") + 1);

        if (line == "exit" || line == "quit") {
            break;
        }
        if (line.empty()) continue;

        auto [level, message] = parseInput(line, defaultLevel);
        queue.push({level, message});
    }

    stop.store(true);
    queue.close();
    worker.join();

    std::cout << "Приложение завершено" << std::endl;
    return 0;
}