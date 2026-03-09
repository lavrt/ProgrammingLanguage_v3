#include <iostream>

#include "tree.h"
#include "generator.h"
#include "backendExceptions.h"
#include "treeExceptions.h"

int main(int argc, const char** argv) {
    try {
        Tree ast;
        ast.Deserialize(argv[1]);
        CodeGen cg;
        cg.GenerateProgram(ast.GetRoot(), argv[2]);
        return 0;
    } catch (const BackendExcept::FileException& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    } catch (const BackendExcept::CodeGeneratorException& e) {
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
