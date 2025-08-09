#include <vector>
#include <string>

#include "tokenizer.h"
#include "parser.h"
#include "tree.h"

int main() {
    std::vector<std::string> tokens;
    tokenizer(tokens);

    tNode* root = runParser(tokens);
    saveTree(root);
    // dump(root);

    treeDtor(root);

    return 0;
}
