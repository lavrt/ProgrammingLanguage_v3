#include <iostream>

#include "frontendExceptions.h"
#include "treeExceptions.h"
#include "tokenizer.h"
#include "parser.h"
#include "tree.h"

int main(int argc, const char** argv) {
    try {
        Tokenizer tokenizer(argv[1]);
        Tree ast = Parser(tokenizer.GetTokens()).Parse();
        ast.Serialize(argv[2]);
        return 0;
    } catch (const FrontendExcept::FileException& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    } catch (const FrontendExcept::TokenizerException& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    } catch (const FrontendExcept::ParserException& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    } catch (const TreeExcept::TreeException& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    } catch (const TreeExcept::FileException& e) {
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
