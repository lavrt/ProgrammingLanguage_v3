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

const char* const kDumpFileName = "./tmp/dump.gv";
const char* const kNameOfFileWithTree = "./tmp/tree.txt";

tNode* newNode(NodeType type, const std::string& value, tNode* left, tNode* right);
void treeDtor(tNode* node);
void dump(tNode* root);
tNode* copyNode(tNode* node);
bool subtreeContainsVariable(tNode* node);
void saveTree(tNode* root);
tNode* ReadTree(std::vector<std::pair<NodeType, std::string>>& nodes);

#endif // TREE_H
