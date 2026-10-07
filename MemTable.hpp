#ifndef MEMTABLE_HPP
#define MEMTABLE_HPP

#include <string>
#include <map>
#include <shared_mutex>
#include <optional>
#include <cstddef>
#include <mutex>

class MemTable {
private:
    std::map<std::string, std::string> table_;
    mutable std::shared_mutex rw_lock_;
    size_t byte_size_{0};

public:
    static inline const std::string TOMBSTONE = "__CERBERUS_TOMBSTONE__";

    void Put(const std::string& key, const std::string& value) {
        std::unique_lock<std::shared_mutex> lock(rw_lock_);
        auto it = table_.find(key);
        if (it != table_.end()) {
            byte_size_ -= (it->first.size() + it->second.size());
        }
        table_[key] = value;
        byte_size_ += (key.size() + value.size());
    }

    void Delete(const std::string& key) {
        Put(key, TOMBSTONE);
    }

    std::optional<std::string> Get(const std::string& key) const {
        std::shared_lock<std::shared_mutex> lock(rw_lock_);
        auto it = table_.find(key);
        if (it != table_.end()) {
            if (it->second == TOMBSTONE) {
                return std::nullopt; // Key explicitly deleted
            }
            return it->second;
        }
        return std::nullopt;
    }

    size_t ByteSize() const {
        std::shared_lock<std::shared_mutex> lock(rw_lock_);
        return byte_size_;
    }

    const std::map<std::string, std::string>& GetEntriesUnsafe() const {
        return table_;
    }

    void Clear() {
        std::unique_lock<std::shared_mutex> lock(rw_lock_);
        table_.clear();
        byte_size_ = 0;
    }
};

#endif // MEMTABLE_HPP