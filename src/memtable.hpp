#pragma once
#include <string>
#include <optional>
#include <iostream>

struct Node {
    std::string key;
    std::string value;

    // When deleting in a LSM engine, nodes have a tombstone
    // marker insead of freeing memory immediately
    bool is_tombstone{false};

    Node* parent{nullptr};
    Node* left{nullptr};
    Node* right{nullptr};

    Node(std::string k, std::string v) : key(std::move(k)), value(std::move(v)) {}
};

class MemTable {
public:
    MemTable() : root(nullptr) {}

    ~MemTable() {
        destroy_tree(root)
    }

    // Insert or update a key-value pair
    void Put(const std::string& key, const std::string& value) {
        // Update if key already exists
        Node* existing = search(root, key);
        if (existing) {
            existing->value = value;
            existing->isTombstone = false; // Revive if previously deleted
            return;
        }

        // If key not found, use BST insertion
        Node* new_node = new Node(key, value);
        if (!root) {
            root = new_node;
            return;
        }

        Node* current = root;
        Node* parent = nullptr;
        while (current != nullptr) {
            parent = current;
            if (key < current->key) {
                current = current->left;
            } else {
                current = current->right;
            }
        }

        // Insert new node
        new_node->parent = parent;
        if (key < parent->key) {
            parent->left = new_node;
        } else {
            parent->right = new_node;
        }
    }

    // Retrieve the value using the key
    std::optional<std::string> Get(const std::string& key) const {
        Node* node = search(root, key);

        // Return the value only if it's found and not tombstoned
        if (node && !node->is_tombstone) {
            return node->value;
        }
        return std::nullopt;
    }

    // Soft delete a key by setting a tombstone flag
    void Delete(std::string& key) {
        // In an LSM engine, deleting a nonexistant key still writes a tombstone
        Node* node = search(root, key);
        if (node) {
            node->is_tombstone = true;
        } else {
            Put(key, "");
            Node* created = search(root, key);
            if (created) {
                created->is_tombstone = true;
            }
        }
    }
private:
    Node* root

    // BST search helper
    Node* search(Node* current, const std::string& key) const {
        while (current != nullptr) {
            if (key == current->key) {
                return current;
            } else if (key < current->key) {
                current = current->left;
            } else {
                current = current->right
            }
        }
        return nullptr;
    }

    // Memory cleanup
    void destroy_tree(Node* node) {
        if (node != nullptr) {
            destroy_tree(node->left);
            destroy_tree(node->right);
            delete node;
        }
    }
}