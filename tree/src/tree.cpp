#include "tree.h"

#include <iostream>
#include <fstream>

void Tree::Dump(const std::string& fileName) const {
    std::ofstream file(fileName + ".gv");
    if (!file) {
        std::cerr << "The \"" << fileName + ".gv" << "\" file cannot be opened." << std::endl;
        exit(EXIT_FAILURE);
    }

    file << "digraph\n"
         << "{\n    "
         << "rankdir = TB;\n    "
         << "node [shape=record,style = filled,penwidth = 2.5];\n    "
         << "bgcolor = \"#FDFBE4\";\n\n";

    DefiningGraphNodes(file, this->GetRoot());
    DefiningGraphDependencies(file, this->GetRoot());

    file << "}\n";

    file.close();

    std::string dotCmd = "dot " + fileName + ".gv -Tpng -o " + fileName + ".png";
    std::system(dotCmd.c_str());
}

void Tree::DefiningGraphNodes(std::ofstream& file, Node* node) const {
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
    } else if (node->GetType() == Call) {
        file << ", color = \"#E8A79E\"];\n";
    } else {
        file << ", color = \"#E8D59E\"];\n";
    }

    if (node->GetLeft()) {
        rank++;
        DefiningGraphNodes(file, node->GetLeft());
    }
    if (node->GetRight()) {
        rank++;
        DefiningGraphNodes(file, node->GetRight());
    }
    rank--;
}

void Tree::DefiningGraphDependencies(std::ofstream& file, Node* node) const {
    static int flag = 0;
    if (node->GetLeft()) {
        if (flag++) {
            file << "-> node_" << node->GetLeft() << " ";
        } else {
            file << "    node_" << node << " -> node_" << node->GetLeft() << " ";
        }
        DefiningGraphDependencies(file, node->GetLeft());
    }
    if (node->GetRight()) {
        if (flag++) {
            file << "-> node_" << node->GetRight() << " ";
        } else {
            file << "    node_" << node << " -> node_" << node->GetRight() << " ";
        }
        DefiningGraphDependencies(file, node->GetRight());
    }
    if (flag) {
        file << ";\n";
    }
    flag = 0;
}

void Tree::Serialize(const std::string& fileName) const {
    std::ofstream file(fileName);
    if (!file) {
        std::cerr << "The \"" << fileName << "\" file cannot be opened." << std::endl;
        exit(EXIT_FAILURE);
    }

    PreOrderTraversal(file, this->GetRoot());

    file.close();
}

void Tree::PreOrderTraversal(std::ofstream& file, Node* node) const {
    if (!node) {
        file << "0 NULL ";
        return;
    }

    file << node->GetType() << " " << node->GetValue() << " ";

    PreOrderTraversal(file, node->GetLeft());
    PreOrderTraversal(file, node->GetRight());    
}

void Tree::Deserialize(const std::string& fileName) {
    std::ifstream file(fileName);
    if (!file) {
        std::cerr << "The \"" << fileName << "\" file cannot be opened." << std::endl;
        exit(EXIT_FAILURE);
    }

    std::string data {
        std::istreambuf_iterator<char>(file),
        std::istreambuf_iterator<char>()
    };

    file.close();

    if (data.empty()) {
        std::cerr << "Deserialization error." << std::endl;
        exit(EXIT_FAILURE);
    }

    std::vector<std::pair<NodeType, std::string>> tokens;
    for (size_t i = 0; i < data.size(); ++i) {
        std::string buffer;
        while (i < data.size() && !std::isspace(data[i])) {
            buffer += data[i++];
        }
        NodeType type = static_cast<NodeType>(std::stoi(buffer));
        buffer.clear();

        ++i;
        while (i < data.size() && !std::isspace(data[i])) {
            buffer += data[i++];
        }
        tokens.push_back({type, buffer});
    }

    this->SetRoot(ParseTreeFromTokens(tokens).first);
}
std::pair<std::unique_ptr<Node>, size_t> Tree::ParseTreeFromTokens(
    const std::vector<std::pair<NodeType, std::string>>& tokens, size_t pos) const
{
    if (tokens[pos].first == Null) {
        return {nullptr, pos + 1};
    }

    std::unique_ptr<Node> node = std::make_unique<Node>(tokens[pos].first, tokens[pos].second);

    auto [left, pos1] = ParseTreeFromTokens(tokens, pos + 1);
    node->SetLeft(std::move(left));
    pos = pos1;

    auto [right, pos2] = ParseTreeFromTokens(tokens, pos);
    node->SetRight(std::move(right));
    pos = pos2;

    return {std::move(node), pos};
}
