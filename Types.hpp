#pragma once
#include <string>

enum class OpType : uint8_t {
  PUT = 1,
  DEL = 2 // tombstone
};

struct Record {
  OpType type;
  std::string key;
  std::string value;
};
