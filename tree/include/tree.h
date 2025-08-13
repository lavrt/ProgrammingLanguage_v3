#ifndef TREE_H
#define TREE_H

#include <vector>
#include <string>
#include <memory>

#include "node.h"

class Tree {
private:
    std::unique_ptr<Node> root = nullptr;

    void DefiningGraphNodes(std::ofstream& file, Node* node) const;

    void DefiningGraphDependencies(std::ofstream& file, Node* node) const;

    void PreOrderTraversal(std::ofstream& file, Node* node) const;

    std::pair<std::unique_ptr<Node>, size_t> ParseTreeFromTokens(
        const std::vector<std::pair<NodeType, std::string>>& tokens, size_t pos = 0) const;

public:
    Tree() : root(nullptr) {}

    Tree(std::unique_ptr<Node> r) : root(std::move(r)) {}

    Node* GetRoot() const {
        return root.get();
    }

    void SetRoot(std::unique_ptr<Node> r) {
        root = std::move(r);
    }

    void Dump(const std::string& fileName) const;

    void Serialize(const std::string& fileName) const;

    void Deserialize(const std::string& fileName);
};

#endif // TREE_H
