#include <vector>

#include "tree.h"
#include "generator.h"
#include "headers.h"

// static ------------------------------------------------------------------------------------------

static const char* const kNameOfOutputFile = "./bin/output.elf";

// global ------------------------------------------------------------------------------------------

int main() {
    std::vector<std::pair<NodeType, char*>> nodes;
    tNode* root = ReadTree(nodes);

    TCodeGen cg;
    CodeGenCtor(&cg);

    Elf64_Ehdr ehdr;
    CreateElfHeader(&ehdr);

    CodegenProgram(&cg, root, &ehdr);

    Elf64_Phdr phdr;
    CreateProgramHeader(&phdr, cg.size);

    FILE* file = fopen(kNameOfOutputFile, "wb");
    if (!file) {
        fprintf(stderr, "Cannot open output file\n");
        exit(EXIT_FAILURE);
    }
    fwrite(&ehdr, kElfHeaderSize, 1, file);
    fwrite(&phdr, kProgramHeaderSize, 1, file);
    fwrite(cg.code, 1, cg.size, file);
    fclose(file);
    printf("Executable file created\n");

    CodeGenDtor(&cg); 

    freeNodes(nodes);
    treeDtor(root);

    return 0;
}
