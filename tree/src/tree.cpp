#include "tree.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <iostream>
#include <fstream>
#include <filesystem>

#define GRAPHVIZ 

// static ------------------------------------------------------------------------------------------

static const char* const kSpaceDelimiter = " ";

static void dumpTreeTraversal(tNode* node, std::ofstream& dumpFile);
static void dumpTreeTraversalWithArrows(tNode* node, std::ofstream& dumpFile);
static void saveTreeToFile(std::ofstream& file, tNode* node);
static std::pair<tNode*, size_t> ReadTreeFromFile(const std::vector<std::pair<NodeType, std::string>>& nodes, size_t pos);

// global ------------------------------------------------------------------------------------------

tNode* newNode(NodeType type, const std::string& value, tNode* left, tNode* right) {
    tNode* node = (tNode*)calloc(1, sizeof(tNode));
    assert(node);

    node->left = left;
    node->right = right;
    node->type = type;
    node->value = &value;

    return node;
}

void treeDtor(tNode* node) {
    assert(node);

    if (node->left) {
        treeDtor(node->left);
    }
    if (node->right) {
        treeDtor(node->right);
    }

    // memset(node, 0, sizeof(tNode));
    free(node);
}

void dump(tNode* root) {
    assert(root);

    std::ofstream dumpFile(kDumpFileName);
    assert(dumpFile);

    dumpFile << "digraph\n";
    dumpFile << "{\n    ";
    dumpFile << "rankdir = TB;\n    ";
    dumpFile << "node [shape=record,style = filled,penwidth = 2.5];\n    ";
    dumpFile << "bgcolor = \"#FDFBE4\";\n\n";

    dumpTreeTraversal(root, dumpFile);
    dumpTreeTraversalWithArrows(root, dumpFile);

    dumpFile << "}\n";

    dumpFile.close();

    #if defined GRAPHVIZ
        system("dot ./tmp/dump.gv -Tpng -o ./tmp/dump.png");
    #endif
}

bool subtreeContainsVariable(tNode* node) {
    if (!node) {
        return false;
    }
 
    static int rank = 0;
    static bool presenceOfVariable = false;

    if (node->type == Identifier) {
        presenceOfVariable = true;
    } else {
        rank++;
        subtreeContainsVariable(node->left);
        subtreeContainsVariable(node->right);
        rank--;
    }

    if (presenceOfVariable && !rank) {
        presenceOfVariable = false;
        return true;
    } else {
        return false;
    }
}

void saveTree(tNode* root) {
    std::ofstream file(kNameOfFileWithTree);
    assert(file);

    saveTreeToFile(file, root);

    file.close();
} 

tNode* ReadTree(std::vector<std::pair<NodeType, std::string>>& nodes) {
    std::ifstream file(kNameOfFileWithTree);
    assert(file);

    std::filesystem::path filePath = kNameOfFileWithTree;
    size_t fileSize = std::filesystem::file_size(filePath);

    char* dataArray = (char*)calloc(fileSize + 1, sizeof(char));
    assert(dataArray);

    file.read(dataArray, fileSize);

    file.close();

    for (size_t i = 0; i < fileSize; ++i) {
        std::string buffer;
        while (i < fileSize && !isspace(dataArray[i])) {
            buffer += dataArray[i++];
        }
        NodeType type = (NodeType)std::stoi(buffer);
        buffer.clear();

        ++i;
        while (i < fileSize && !isspace(dataArray[i])) {
            buffer += dataArray[i++];
        }
        nodes.push_back({type, buffer});
    }

    free(dataArray);
    
    return ReadTreeFromFile(nodes, 0).first;
}

// static ------------------------------------------------------------------------------------------

static void dumpTreeTraversal(tNode* node, std::ofstream& dumpFile) {
    assert(dumpFile);
    if (!node) {
        return;
    }

    static size_t rank = 0;
    dumpFile << "    node_" << node << " [rank=" << rank << ",label=\" { node: " << node;

    std::string type = kNodeTypeToString.at(node->type);
    if (type == keyLess || type == keyGreater || type == keyIdentical ||
        type == keyLessOrEqual || type == keyGreaterOrEqual || type == keyNotIdentical)
    {
        dumpFile << " | type: \\" << type << " | value: \\" << *node->value << " | ";
    } else {
        dumpFile << " | type: " << type << " | value: " << *node->value << " | ";
    }

    dumpFile << "{ left: " << node->left << " | right: " << node->right << " }} \"";

    if (node->type == Number) {
        dumpFile << ", color = \"#DBD4FF\"];\n";
    } else if (node->type == Identifier) {
        dumpFile << ", color = \"#EBAEE6\"];\n";
    } else if (node->type == Def) {
        dumpFile << ", color = \"#E7FFAC\"];\n";
    } else if (node->type == Calling) {
        dumpFile << ", color = \"#E8A79E\"];\n";
    } else {
        dumpFile << ", color = \"#E8D59E\"];\n";
    }

    if (node->left) {
        rank++;
        dumpTreeTraversal(node->left, dumpFile);
    }
    if (node->right) {
        rank++;
        dumpTreeTraversal(node->right, dumpFile);
    }
    rank--;
}

static void dumpTreeTraversalWithArrows(tNode* node, std::ofstream& dumpFile) {
    assert(dumpFile);
    if (!node) {
        return;
    }

    static int flag = 0;
    if (node->left) {
        if (flag++) {
            dumpFile << "-> node_" << node->left << " ";
        } else {
            dumpFile << "    node_" << node << " -> node_" << node->left << " ";
        }
        dumpTreeTraversalWithArrows(node->left, dumpFile);
    }
    if (node->right) {
        if (flag++) {
            dumpFile << "-> node_" << node->right << " ";
        } else {
            dumpFile << "    node_" << node << " -> node_" << node->right << " ";
        }
        dumpTreeTraversalWithArrows(node->right, dumpFile);
    }
    if (flag) {
        dumpFile << ";\n";
    }
    flag = 0;
}

static void saveTreeToFile(std::ofstream& file, tNode* node) {
    if (!node) {
        file << "0 NULL ";
        return;
    }

    file << node->type << " " << *node->value << " ";

    saveTreeToFile(file, node->left);
    saveTreeToFile(file, node->right);    
}

static std::pair<tNode*, size_t> ReadTreeFromFile(const std::vector<std::pair<NodeType, std::string>>& nodes, size_t pos) { 
    if (nodes[pos].first == Null) {
        return {nullptr, pos + 1};
    }

    tNode* node = newNode(nodes[pos].first, nodes[pos].second, nullptr, nullptr); 
    std::tie(node->left, pos) = ReadTreeFromFile(nodes, pos + 1);
    std::tie(node->right, pos) = ReadTreeFromFile(nodes, pos);
    return {node, pos};
} 
