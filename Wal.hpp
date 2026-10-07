#pragma once
#include "Types.hpp"
#include <fstream>
#include <vector>
#include <filesystem>
#include <stdexcept>

class WriteAheadLog {
private:
    std::string filepath_;
    std::ofstream writer_; // to write to files

public:
    explicit WriteAheadLog(std::string filepath) : filepath_(std::move(filepath)) {
        writer_.open(filepath, std::ios::binary | std::ios::app); // binary and append mode
        if (!writer_.is_open()) {
            throw std::runtime_error("Failed to open WAL file for writing: " + filepath);
        }
    }

    // clean up memory
    ~WriteAheadLog() {
        if (writer_.is_open()) writer_.close();
    }

    void append(OpType type, const std::string& key, const std::string& value) {
        uint8_t op = static_cast<uint8_t>(type);
        uint32_t klen = static_cast<uint32_t>(key.size());
        uint32_t vlen = static_cast<uint32_t>(value.size());

        // makes reading easy by specifying exact length of key and value 
        writer_.write(reinterpret_cast<const char*>(&op), sizeof(op));
        writer_.write(reinterpret_cast<const char*>(&klen), sizeof(klen));
        // key.data() returns pointer to start of key and writes to the wal
        writer_.write(key.data(), klen);
        writer_.write(reinterpret_cast<const char*>(&vlen), sizeof(vlen));
        writer_.write(value.data(), vlen);
        // writes all necessary information (key length, value length, key data, value data)
        // in order to make it easier to read the data in the wal
        writer_.flush();
    }

}
