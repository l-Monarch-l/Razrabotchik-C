#include "logger/FileSink.h"
#include <iostream>

namespace logger {
    FileSink::FileSink(const std::string& filename) {
        file_.open(filename, std::ios::app | std::ios::out);
        if (!file_.is_open()) {
            std::cerr << "[FileSink] ошибка открытия лог файла: " << filename << std::endl;
        }
    }

    bool FileSink::write(const std::string& formatted_message) {
        std::lock_guard<std::mutex> lock(write_mutex_);
        if (!file_.is_open()) {
            std::cerr << "[FileSink] запись: файл не открывается" << std::endl;
            return false;
        }
        file_ << formatted_message << std::endl;
        if (file_.fail()) {
            std::cerr << "[FileSink] запись: ошибка после чтения" << std::endl;
            file_.clear();
            return false;
        }
        file_.flush();
        if (file_.fail()) {
            std::cerr << "[FileSink] запись: ошибка после flush" << std::endl;
            file_.clear();
            return false;
        }
        return true;
    }
}