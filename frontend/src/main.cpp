#include <vector>

#include "tokenizer.h"
#include "parser.h"
#include "tree.h"

int main() {
    std::vector<char*> tokens;
    tokenizer(tokens);

    tNode* root = runParser(tokens);
    saveTree(root);
    dump(root);

    freeTokens(tokens);
    treeDtor(root);

    return 0;
}
