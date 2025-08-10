#ifndef TREE_H
#define TREE_H

#include "node.h"

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string>

#define FREE(ptr_) \
    do { free(ptr_); ptr_ = NULL; } while(0);
#define FCLOSE(ptr_) \
    do { fclose(ptr_); ptr_ = NULL; } while(0);

const char* const kDumpFileName = "./tmp/Dump.gv";
const char* const kNameOfFileWithTree = "./tmp/tree.txt";

TNode* NewNode(NodeType type, const std::string& value, TNode* left, TNode* right);
void TreeDtor(TNode* node);
void Dump(TNode* root);
void SaveTree(TNode* root);
TNode* ReadTree(std::vector<std::pair<NodeType, std::string>>& nodes);

#endif // TREE_H
