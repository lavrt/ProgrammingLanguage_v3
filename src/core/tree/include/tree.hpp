#ifndef TREE_H
#define TREE_H

#include <vector>
#include <string>
#include <memory>

#include "node.hpp"

class Tree {
    class NodePool {
    private:
        std::vector<std::unique_ptr<Node>> data_;

    public:
        template <class... Args>
        Node* Create(Args&&... args) {
            data_.push_back(std::make_unique<Node>(std::forward<Args>(args)...));
            return data_.back().get();
        }

        void Reserve(size_t n) {
            data_.reserve(n);
        }

        void Clear() {
            data_.clear();
        }
    };

public:
    Tree() : root(nullptr) {}

    Tree(Node* r) : root(r) {}

    template <class... Args>
    Node* Create(Args&&... args) {
        return pool.Create(std::forward<Args>(args)...);
    }

    Node* GetRoot() const {
        return root;
    }

    void SetRoot(Node* r) {
        root = r;
    }

    void Dump(const std::string& fileName) const;

    void Serialize(const std::string& fileName) const;

    void Deserialize(const std::string& fileName);
    
private:
    Node* root;
    NodePool pool;

    void DefiningGraphNodes(std::ofstream& file, Node* node) const;

    void DefiningGraphDependencies(std::ofstream& file, Node* node) const;

    void PreOrderTraversal(std::ofstream& file, Node* node) const;

    std::pair<Node*, size_t> ParseTreeFromTokens(
        const std::vector<std::pair<NodeType, std::string>>& tokens, size_t pos = 0);
};

#endif // TREE_H
