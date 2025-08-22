#include <iostream>

#include "tree.h"
#include "generator.h"
#include "backendExceptions.h"
#include "treeExceptions.h"

int main() {
    try {
        Tree ast;
        ast.Deserialize("./tmp/tree.txt");
        ast.Dump("./tmp/dump_backend");

        CodeGen cg;
        cg.GenerateProgram(ast.GetRoot(), "./bin/output.elf");

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
