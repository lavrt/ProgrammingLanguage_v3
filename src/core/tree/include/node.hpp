#pragma once

#include <string>
#include <memory>

#include "node_types.hpp"

class Node {
public:
    Node(NodeType t, const std::string& v, Node* l, Node* r)
        : type(t), value(v), left(l), right(r) {}

    Node(NodeType t, const std::string& v)
        : type(t), value(v), left(nullptr), right(nullptr) {}

    Node() : left(nullptr), right(nullptr) {}

    void SetType(NodeType t) {
        type = t;
    }

    void SetValue(const std::string& v) {
        value = v;
    }

    void SetLeft(Node* l) {
        left = l;
    }

    void SetRight(Node* r) {
        right = r;
    }

    NodeType GetType() const noexcept {
        return type;
    }

    std::string GetValue() const noexcept {
        return value;
    }

    Node* GetLeft() const noexcept {
        return left;
    }

    Node* GetRight() const noexcept {
        return right;
    }

private:
    NodeType type;
    std::string value;
    Node* left;
    Node* right;
};
