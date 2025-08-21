#include <iostream>

#include "exceptions.h"
#include "tokenizer.h"
#include "parser.h"
#include "tree.h"

int main() {
    try {
        Tokenizer tokenizer("./examples/code.rt");
        Tree ast = Parser(tokenizer.GetTokens()).Parse();

        ast.Dump("./tmp/dump_frontend");
        ast.Serialize("./tmp/tree.txt");

        return 0;
    } catch (const FileException& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    } catch (const TokenizerException& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    } catch (const ParserException& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Unexpected error: " << e.what() << std::endl;
        return 2;
    } catch (...) {
        std::cerr << "Unknown error occured" << std::endl;
        return 3;
    }
}
