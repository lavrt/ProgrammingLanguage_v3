#include "tree.h"
#include "generator.h"

int main() {
    Tree ast;
    ast.Deserialize("./tmp/tree.txt");
    ast.Dump("./tmp/dump_backend");

    CodeGen cg;
    cg.GenerateProgram(ast.GetRoot(), "./bin/output.elf");

    return 0;
}
