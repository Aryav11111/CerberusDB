#pragma once
#include "Types.hpp"
#include <fstream>
#include <vector>
#include <filesystem>
#include <stdexcept>

class WriteAheadLog {
private:
    std::string filepath_;
    std::ofstream writer_;

public:
    explicit WriteAheadLog(std::string filepath) : filepath_(std::move(filepath)) {
        writer_.open(filepath, std::ios::binary | std::ios::app);
        if (!writer_.is_open()) {
            throw std::runtime_error("Failed to open WAL file for writing: " + filepath);
        }
    }

    ~WriteAheadLog() {
        if (writer_.is_open()) writer_.close();
    }
}
