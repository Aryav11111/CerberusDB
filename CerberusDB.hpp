#ifndef CERBERUSDB_HPP
#define CERBERUSDB_HPP

#include "MemTable.hpp"
#include "Wal.hpp"
#include "SSTable.hpp"
#include <vector>
#include <memory>
#include <sstream>

class CerberusDB {
private:
    MemTable memtable_;
    WriteAheadLog wal_;
    std::vector<std::string> sstable_files_;
    size_t flush_threshold_bytes_;
    size_t sstable_counter_{0};

    void FlushMemTable() {
        std::string sstable_name = "sstable_" + std::to_string(++sstable_counter_) + ".db";
        SSTable::Write(sstable_name, memtable_.GetEntriesUnsafe());
        sstable_files_.push_back(sstable_name);
        memtable_.Clear();
        wal_.Clear();
    }

public:
    explicit CerberusDB(size_t flush_threshold = 1024) 
        : wal_("cerberus.wal"), flush_threshold_bytes_(flush_threshold) {}

    void Put(const std::string& key, const std::string& value) {
        wal_.LogPut(key, value);
        memtable_.Put(key, value);

        if (memtable_.ByteSize() >= flush_threshold_bytes_) {
            FlushMemTable();
        }
    }

    void Delete(const std::string& key) {
        wal_.LogDelete(key);
        memtable_.Delete(key);
    }

    std::optional<std::string> Get(const std::string& key) {
        // 1. Check in-memory MemTable
        auto res = memtable_.Get(key);
        if (res.has_value()) {
            return res.value();
        }

        // 2. Search SSTables from newest to oldest
        for (auto it = sstable_files_.rbegin(); it != sstable_files_.rend(); ++it) {
            auto disk_res = SSTable::Read(*it, key);
            if (disk_res.has_value()) {
                if (disk_res.value() == MemTable::TOMBSTONE) {
                    return std::nullopt; // Key was soft deleted
                }
                return disk_res.value();
            }
        }

        return std::nullopt;
    }
};

#endif // CERBERUSDB_HPP