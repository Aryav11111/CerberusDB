#ifndef SSTABLE_HPP
#define SSTABLE_HPP

#include <string>
#include <map>
#include <fstream>
#include <optional>

class SSTable {
public:
    static void Write(const std::string& filename, const std::map<std::string, std::string>& data) {
        std::ofstream out(filename, std::ios::binary);
        for (const auto& [k, v] : data) {
            out << k.size() << " " << k << " " << v.size() << " " << v << "\n";
        }
    }

    static std::optional<std::string> Read(const std::string& filename, const std::string& target_key) {
        std::ifstream in(filename, std::ios::binary);
        if (!in.is_open()) return std::nullopt;

        size_t k_len, v_len;
        std::string k, v;

        while (in >> k_len) {
            in.ignore(1); // Skip space
            k.resize(k_len);
            in.read(&k[0], k_len);

            in >> v_len;
            in.ignore(1); // Skip space
            v.resize(v_len);
            in.read(&v[0], v_len);
            in.ignore(1); // Skip newline

            if (k == target_key) {
                return v;
            }
        }
        return std::nullopt;
    }
};

#endif // SSTABLE_HPP