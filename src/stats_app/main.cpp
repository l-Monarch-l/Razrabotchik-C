#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <atomic>
#include <chrono>
#include <mutex>
#include <sstream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

using namespace std::chrono_literals;

// Структура для хранения статистики
class Statistics {
public:
    struct Stats {
        uint64_t total = 0;
        uint64_t debug_count = 0;
        uint64_t info_count = 0;
        uint64_t error_count = 0;
        // Для длин
        uint64_t min_len = UINT64_MAX;
        uint64_t max_len = 0;
        double sum_len = 0;
        uint64_t count_len = 0;
        // Для последнего часа
        std::vector<time_t> timestamps; // будем хранить время каждого сообщения
    };

    Statistics() = default;

    // Обновить статистику одним сообщением
    void addEntry(const std::string& level, const std::string& message, time_t time) {
        std::lock_guard<std::mutex> lock(mtx_);
        stats_.total++;
        if (level == "DEBUG") stats_.debug_count++;
        else if (level == "INFO") stats_.info_count++;
        else if (level == "ERROR") stats_.error_count++;

        size_t len = message.length();
        if (len > 0) {
            stats_.count_len++;
            stats_.sum_len += len;
            if (len < stats_.min_len) stats_.min_len = len;
            if (len > stats_.max_len) stats_.max_len = len;
        }

        // Сохраняем временную метку для подсчёта за последний час
        stats_.timestamps.push_back(time);
        time_t cutoff = time - 3600;
        auto it = stats_.timestamps.begin();
        while (it != stats_.timestamps.end() && *it < cutoff) {
            it = stats_.timestamps.erase(it);
        }
    }

    // Получить копию статистики
    Stats getStats() const {
        std::lock_guard<std::mutex> lock(mtx_);
        return stats_;
    }

    // Проверить, изменилась ли статистика с момента последнего вывода
    bool hasChangedSinceLastReport() {
        std::lock_guard<std::mutex> lock(mtx_);
        if (last_reported_total_ != stats_.total ||
            last_reported_debug_ != stats_.debug_count ||
            last_reported_info_ != stats_.info_count ||
            last_reported_error_ != stats_.error_count ||
            last_reported_min_ != stats_.min_len ||
            last_reported_max_ != stats_.max_len) {
            // Обновляем сохранённые значения
            last_reported_total_ = stats_.total;
            last_reported_debug_ = stats_.debug_count;
            last_reported_info_ = stats_.info_count;
            last_reported_error_ = stats_.error_count;
            last_reported_min_ = stats_.min_len;
            last_reported_max_ = stats_.max_len;
            return true;
        }
        return false;
    }

    // Подсчитать количество за последний час
    uint64_t countLastHour(time_t now) const {
        std::lock_guard<std::mutex> lock(mtx_);
        time_t cutoff = now - 3600;
        auto it = stats_.timestamps.begin();
        while (it != stats_.timestamps.end() && *it < cutoff) {
            ++it;
        }
        return stats_.timestamps.end() - it;
    }

private:
    Stats stats_;
    mutable std::mutex mtx_;
    // Для отслеживания изменений
    uint64_t last_reported_total_ = 0;
    uint64_t last_reported_debug_ = 0;
    uint64_t last_reported_info_ = 0;
    uint64_t last_reported_error_ = 0;
    uint64_t last_reported_min_ = UINT64_MAX;
    uint64_t last_reported_max_ = 0;
};

// Парсинг строки лога: ожидается формат "[время] [УРОВЕНЬ] сообщение"
bool parseLogLine(const std::string& line, std::string& level, std::string& message, time_t& time) {
    // Ищем первую '[' для времени
    size_t pos1 = line.find('[');
    if (pos1 == std::string::npos) return false;
    size_t pos2 = line.find(']', pos1);
    if (pos2 == std::string::npos) return false;
    // Извлекаем время
    std::string time_str = line.substr(pos1+1, pos2-pos1-1);
    // Ищем вторую '[' для уровня
    size_t pos3 = line.find('[', pos2);
    if (pos3 == std::string::npos) return false;
    size_t pos4 = line.find(']', pos3);
    if (pos4 == std::string::npos) return false;
    level = line.substr(pos3+1, pos4-pos3-1);
    // Сообщение - всё после второго ']' и пробела
    size_t msg_start = pos4 + 1;
    if (msg_start < line.size() && line[msg_start] == ' ') msg_start++;
    message = line.substr(msg_start);
    // Время игнорируем, но для последнего часа будем использовать текущее время приёма
    time = std::time(nullptr);
    return true;
}

// Функция вывода статистики в консоль
void printStats(const Statistics::Stats& stats, uint64_t lastHour) {
    std::cout << "\n=== Статистика ===" << std::endl;
    std::cout << "Сообщений в общем: " << stats.total << std::endl;
    std::cout << "  DEBUG: " << stats.debug_count << std::endl;
    std::cout << "  INFO:  " << stats.info_count << std::endl;
    std::cout << "  ERROR: " << stats.error_count << std::endl;
    std::cout << "За последний час: " << lastHour << std::endl;
    if (stats.count_len > 0) {
        std::cout << "Длинна сообщений: min=" << stats.min_len
                  << ", max=" << stats.max_len
                  << ", avg=" << (stats.sum_len / stats.count_len) << std::endl;
    } else {
        std::cout << "Длинна сообщений: нет сообщений" << std::endl;
    }
    std::cout << "=================\n" << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc < 4) {
        std::cerr << "Применение: " << argv[0] << " <port> <N> <T>" << std::endl;
        std::cerr << "  port - TCP port to listen on" << std::endl;
        std::cerr << "  N - написание статистики после приёма N сообщений" << std::endl;
        std::cerr << "  T - таймаут (секунды) написания статистики если были изменения" << std::endl;
        return 1;
    }

    uint16_t port = static_cast<uint16_t>(std::stoi(argv[1]));
    int N = std::stoi(argv[2]);
    int T = std::stoi(argv[3]);

    // Создаём сокет
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd < 0) {
        std::cerr << "ошибка создания сокета" << std::endl;
        return 1;
    }

    int opt = 1;
    if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        std::cerr << "setsockopt ошибка" << std::endl;
        close(listen_fd);
        return 1;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    if (bind(listen_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        std::cerr << "не удалось выполнить привязку к порту " << port << std::endl;
        close(listen_fd);
        return 1;
    }

    if (listen(listen_fd, 1) < 0) {
        std::cerr << "подключение ошибка" << std::endl;
        close(listen_fd);
        return 1;
    }

    std::cout << "Приложение статистики на порту " << port << std::endl;

    // Принимаем одно соединение (библиотека будет подключаться один раз)
    int client_fd = accept(listen_fd, nullptr, nullptr);
    if (client_fd < 0) {
        std::cerr << "ошибка" << std::endl;
        close(listen_fd);
        return 1;
    }
    close(listen_fd);

    std::cout << "Клиент подключился" << std::endl;

    // Инициализируем статистику
    Statistics stats;
    std::atomic<bool> stop{false};
    std::atomic<bool> stats_changed{false};

    // Поток для таймера
    std::thread timer([&]() {
        while (!stop.load()) {
            std::this_thread::sleep_for(std::chrono::seconds(T));
            if (!stop.load()) {
                auto current_stats = stats.getStats();
                uint64_t lastHour = stats.countLastHour(std::time(nullptr));
                if (stats.hasChangedSinceLastReport()) {
                    printStats(current_stats, lastHour);
                }
            }
        }
    });

    // Основной цикл чтения из сокета
    char buffer[4096];
    std::string incoming;
    int message_count = 0;

    while (!stop.load()) {
        ssize_t bytes = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
        if (bytes <= 0) {
            break;
        }
        buffer[bytes] = '\0';
        incoming += buffer;

        // Разбиваем по '\n'
        size_t pos;
        while ((pos = incoming.find('\n')) != std::string::npos) {
            std::string line = incoming.substr(0, pos);
            incoming.erase(0, pos + 1);
            if (line.empty()) continue;

            std::cout << "[Полученно] " << line << std::endl;
            std::string level, message;
            time_t time;
            if (parseLogLine(line, level, message, time)) {
                stats.addEntry(level, message, time);
                message_count++;
                if (message_count % N == 0) {
                    auto current_stats = stats.getStats();
                    uint64_t lastHour = stats.countLastHour(time);
                    printStats(current_stats, lastHour);
                    stats.hasChangedSinceLastReport();
                }
            } else {
                std::cerr << "Осторожно: не удалось распарсить: " << line << std::endl;
            }
        }
    }

    stop.store(true);
    timer.join();
    close(client_fd);

    auto final_stats = stats.getStats();
    uint64_t lastHour = stats.countLastHour(std::time(nullptr));
    printStats(final_stats, lastHour);

    std::cout << "Приложение статистики завершено" << std::endl;
    return 0;
}