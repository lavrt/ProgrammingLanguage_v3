#include "tokenizer.h"
#include "parser.h"
#include "tree.h"

int main() {
    Tokenizer tokenizer("./examples/code.rt");
    Tree ast = Parser(tokenizer.GetTokens()).Parse();

    // ast.Dump("./tmp/dump_frontend");
    ast.Serialize("./tmp/tree.txt");

    return 0;
}
