#ifndef WAL_HPP
#define WAL_HPP

#include <string>
#include <fstream>
#include <mutex>
#include <iostream>

class WriteAheadLog {
private:
    std::ofstream log_file_;
    std::string file_path_;
    std::mutex wal_mutex_;

public:
    explicit WriteAheadLog(const std::string& path) : file_path_(path) {
        log_file_.open(path, std::ios::app | std::ios::binary);
    }

    ~WriteAheadLog() {
        if (log_file_.is_open()) {
            log_file_.close();
        }
    }

    void LogPut(const std::string& key, const std::string& value) {
        std::lock_guard<std::mutex> lock(wal_mutex_);
        if (!log_file_.is_open()) return;
        log_file_ << "PUT " << key.size() << " " << key << " " << value.size() << " " << value << "\n";
        log_file_.flush();
    }

    void LogDelete(const std::string& key) {
        std::lock_guard<std::mutex> lock(wal_mutex_);
        if (!log_file_.is_open()) return;
        log_file_ << "DEL " << key.size() << " " << key << "\n";
        log_file_.flush();
    }

    void Clear() {
        std::lock_guard<std::mutex> lock(wal_mutex_);
        if (log_file_.is_open()) log_file_.close();
        log_file_.open(file_path_, std::ios::out | std::ios::trunc | std::ios::binary);
    }
};

#endif // WAL_HPP