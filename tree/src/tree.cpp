#include "tree.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <iostream>
#include <fstream>
#include <filesystem>

#define GRAPHVIZ 

// static ------------------------------------------------------------------------------------------

// static void DumpTreeTraversal(TNode* node, std::ofstream& dumpFile);
// static void DumpTreeTraversalWithArrows(TNode* node, std::ofstream& dumpFile);
// static void SaveTreeToFile(std::ofstream& file, TNode* node);
// static std::pair<TNode*, size_t> ReadTreeFromFile(const std::vector<std::pair<NodeType, std::string>>& nodes, size_t pos);

// global ------------------------------------------------------------------------------------------

void TTree::Dump(const std::string& fileName) const {
    std::ofstream file(fileName);
    if (!file) {
        std::cerr << "The \"" << fileName << "\" file cannot be opened." << std::endl;
        exit(EXIT_FAILURE);
    }

    file << "digraph\n"
         << "{\n    "
         << "rankdir = TB;\n    "
         << "node [shape=record,style = filled,penwidth = 2.5];\n    "
         << "bgcolor = \"#FDFBE4\";\n\n";

    TTree::DefiningGraphNodes(file, this->GetRoot());
    TTree::DefiningGraphDependencies(file, this->GetRoot());

    file << "}\n";

    file.close();

    #if defined GRAPHVIZ
        system("dot ./tmp/dump.gv -Tpng -o ./tmp/dump.png"); // FIXME
    #endif
}

void TTree::DefiningGraphNodes(std::ofstream& file, TNode* node) const {
    static size_t rank = 0;
    file << "    node_" << node << " [rank=" << rank << ",label=\" { node: " << node;

    std::string type = kNodeTypeToString.at(node->GetType());
    if (type == keyLess || type == keyLessOrEqual ||
        type == keyGreater || type == keyGreaterOrEqual ||
        type == keyIdentical || type == keyNotIdentical)
    {
        file << " | type: \\" << type << " | value: \\" << node->GetValue() << " | ";
    } else {
        file << " | type: " << type << " | value: " << node->GetValue() << " | ";
    }

    file << "{ left: " << node->GetLeft() << " | right: " << node->GetRight() << " }} \"";

    if (node->GetType() == Number) {
        file << ", color = \"#DBD4FF\"];\n";
    } else if (node->GetType() == Identifier) {
        file << ", color = \"#EBAEE6\"];\n";
    } else if (node->GetType() == Def) {
        file << ", color = \"#E7FFAC\"];\n";
    } else if (node->GetType() == Calling) {
        file << ", color = \"#E8A79E\"];\n";
    } else {
        file << ", color = \"#E8D59E\"];\n";
    }

    if (node->GetLeft()) {
        rank++;
        TTree::DefiningGraphNodes(file, node->GetLeft());
    }
    if (node->GetRight()) {
        rank++;
        TTree::DefiningGraphNodes(file, node->GetRight());
    }
    rank--;
}

void TTree::DefiningGraphDependencies(std::ofstream& file, TNode* node) const {
    static int flag = 0;
    if (node->GetLeft()) {
        if (flag++) {
            file << "-> node_" << node->GetLeft() << " ";
        } else {
            file << "    node_" << node << " -> node_" << node->GetLeft() << " ";
        }
        TTree::DefiningGraphDependencies(file, node->GetLeft());
    }
    if (node->GetRight()) {
        if (flag++) {
            file << "-> node_" << node->GetRight() << " ";
        } else {
            file << "    node_" << node << " -> node_" << node->GetRight() << " ";
        }
        TTree::DefiningGraphDependencies(file, node->GetRight());
    }
    if (flag) {
        file << ";\n";
    }
    flag = 0;
}

void TTree::Serialize(const std::string& fileName) const {
    std::ofstream file(fileName);
    if (!file) {
        std::cerr << "The \"" << fileName << "\" file cannot be opened." << std::endl;
        exit(EXIT_FAILURE);
    }

    TTree::PreOrderTraversal(file, this->GetRoot());

    file.close();
}

void TTree::PreOrderTraversal(std::ofstream& file, TNode* node) const {
    if (!node) {
        file << "0 NULL ";
        return;
    }

    file << node->GetType() << " " << node->GetValue() << " ";

    TTree::PreOrderTraversal(file, node->GetLeft());
    TTree::PreOrderTraversal(file, node->GetRight());    
}

void TTree::Deserialize(const std::string& fileName) {
    std::ifstream file(fileName);
    if (!file) {
        std::cerr << "The \"" << fileName << "\" file cannot be opened." << std::endl;
        exit(EXIT_FAILURE);
    }

    std::string data {
        std::istreambuf_iterator<char>(file),
        std::istreambuf_iterator<char>()
    };

    // TODO errors handling

    file.close();

    // TODO
}

// TNode* ReadTree(std::vector<std::pair<NodeType, std::string>>& nodes) {
//     std::ifstream file(kNameOfFileWithTree);
//     assert(file);

//     std::filesystem::path filePath = kNameOfFileWithTree;
//     size_t fileSize = std::filesystem::file_size(filePath);

//     char* dataArray = (char*)calloc(fileSize + 1, sizeof(char));
//     assert(dataArray);

//     file.read(dataArray, fileSize);

//     file.close();

//     for (size_t i = 0; i < fileSize; ++i) {
//         std::string buffer;
//         while (i < fileSize && !isspace(dataArray[i])) {
//             buffer += dataArray[i++];
//         }
//         NodeType type = (NodeType)std::stoi(buffer);
//         buffer.clear();

//         ++i;
//         while (i < fileSize && !isspace(dataArray[i])) {
//             buffer += dataArray[i++];
//         }
//         nodes.push_back({type, buffer});
//     }

//     free(dataArray);
    
//     return ReadTreeFromFile(nodes, 0).first;
// }

// // static ------------------------------------------------------------------------------------------

// static std::pair<TNode*, size_t> ReadTreeFromFile(const std::vector<std::pair<NodeType, std::string>>& nodes, size_t pos) { 
//     if (nodes[pos].first == Null) {
//         return {nullptr, pos + 1};
//     }

//     TNode* node = NewNode(nodes[pos].first, nodes[pos].second, nullptr, nullptr); 
//     std::tie(node->left, pos) = ReadTreeFromFile(nodes, pos + 1);
//     std::tie(node->right, pos) = ReadTreeFromFile(nodes, pos);
//     return {node, pos};
// } 
