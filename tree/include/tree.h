#ifndef TREE_H
#define TREE_H

#include "node.h"

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include <memory>

class TTree {
private:
    std::unique_ptr<TNode> root = nullptr;

    void DefiningGraphNodes(std::ofstream& file, TNode* node) const;

    void DefiningGraphDependencies(std::ofstream& file, TNode* node) const;

    void PreOrderTraversal(std::ofstream& file, TNode* node) const;

public:
    TTree(std::unique_ptr<TNode> r) : root(std::move(r)) {}

    TNode* GetRoot() const {
        return root.get();
    }

    void Dump(const std::string& fileName) const;

    void Serialize(const std::string& fileName) const;

    void Deserialize(const std::string& fileName);
};

#define FREE(ptr_) \
    do { free(ptr_); ptr_ = NULL; } while(0);
#define FCLOSE(ptr_) \
    do { fclose(ptr_); ptr_ = NULL; } while(0);

const char* const kDumpFileName = "./tmp/dump.gv";
const char* const kNameOfFileWithTree = "./tmp/tree.txt";

// void Dump(TNode* root);
// void SaveTree(TNode* root);
// TNode* ReadTree(std::vector<std::pair<NodeType, std::string>>& nodes);

#endif // TREE_H
