#ifndef TREE_H
#define TREE_H

#include "node.h"

#include <stdio.h>
#include <stdlib.h>
#include <vector>

#define FREE(ptr_) \
    do { free(ptr_); ptr_ = NULL; } while(0);
#define FCLOSE(ptr_) \
    do { fclose(ptr_); ptr_ = NULL; } while(0);

const char* const kDumpFileName = "./Frontend/dump/dump.gv";
const char* const kNameOfFileWithTree = "./Frontend/dump/tree.txt";

const char* const kFunction = "def";
const char* const kNumber = "number";
const char* const kVariable = "variable";
const char* const kOperation = "operation";
const char* const kCalling = "calling";
const char* const kBinary = "binary";

tNode* newNode(NodeType type, const char* value, tNode* left, tNode* right);
void treeDtor(tNode* node);
void dump(tNode* root);
tNode* copyNode(tNode* node);
bool subtreeContainsVariable(tNode* node);
void saveTree(tNode* root);
tNode* ReadTree(std::vector<std::pair<NodeType, char*>>& nodes);
void freeNodes(std::vector<std::pair<NodeType, char*>>& nodes);

#endif // TREE_H
