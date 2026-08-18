#pragma once
#include <string>
#include <optional>
#include <iostream>

struct Node {
    std::string key;
    std::string value;

    // When deleting in a LSM engine, nodes have a tombstone
    // marker insead of freeing memory immediately
    bool isTombstone{false};

    Node* parent{nullptr};
    Node* left{nullptr};
    Node* right{nullptr};

    Node(std::string k, std::string v) : key(std::move(k)), value(std::move(v)) {}

};