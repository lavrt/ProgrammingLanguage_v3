#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <iterator>

#include "tokenizer.h"
#include "parser.h"
#include "tree.h"

int main() {
    std::ifstream file(kNameOfFileWithCode);
    if (!file) {
        std::cerr << "The \"" << kNameOfFileWithCode << "\" file cannot be opened." << std::endl;
        exit(EXIT_FAILURE);
    }

    std::string data {
        std::istreambuf_iterator<char>(file),
        std::istreambuf_iterator<char>()
    };

    if (data.empty()) {
        std::cerr << "The \"" << kNameOfFileWithCode << "\" file is empty or cannot be read." << std::endl;
        exit(EXIT_FAILURE);
    }

    file.close();

    std::vector<std::string> tokens = Tokenizer(data);

    TNode* root = RunParser(tokens);
    SaveTree(root);
    // Dump(root);

    TreeDtor(root);

    return 0;
}
