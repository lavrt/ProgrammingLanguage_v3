#pragma once

#include <string>
#include <memory>

#include "node_types.hpp"

class Node {
public:
    Node(NodeType t, const std::string& v, std::unique_ptr<Node> l, std::unique_ptr<Node> r)
        : type(t), value(v), left(std::move(l)), right(std::move(r)) {}

    Node(NodeType t, const std::string& v)
        : type(t), value(v), left(nullptr), right(nullptr) {}

    Node() : left(nullptr), right(nullptr) {}

    void SetType(NodeType t) {
        type = t;
    }

    void SetValue(const std::string& v) {
        value = v;
    }

    void SetLeft(std::unique_ptr<Node> l) {
        left = std::move(l);
    }

    void SetRight(std::unique_ptr<Node> r) {
        right = std::move(r);
    }

    NodeType GetType() const noexcept {
        return type;
    }

    std::string GetValue() const noexcept {
        return value;
    }

    Node* GetLeft() const noexcept {
        return left.get();
    }

    Node* GetRight() const noexcept {
        return right.get();
    }

private:
    NodeType type;
    std::string value;
    std::unique_ptr<Node> left;
    std::unique_ptr<Node> right;
};
