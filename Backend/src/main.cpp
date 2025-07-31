#include <vector>

#include "tree.h"
#include "generator.h"
#include "headers.h"

int main() {
    std::vector<std::pair<NodeType, char*>> nodes;
    tNode* newRoot = ReadTree(nodes);

    TCodeGen cg;
    CodeGenCtor(&cg);

    Elf64_Ehdr ehdr;
    CreateElfHeader(&ehdr);

    CodegenProgram(&cg, newRoot, &ehdr);

    Elf64_Phdr phdr;
    CreateProgramHeader(&phdr, cg.size);

    FILE* file = fopen("output.elf", "wb");
    if (!file) {
        fprintf(stderr, "Cannot open output file\n");
        exit(EXIT_FAILURE);
    }
    fwrite(&ehdr, sizeof(Elf64_Ehdr), 1, file);
    fwrite(&phdr, sizeof(Elf64_Phdr), 1, file);
    fwrite(cg.code, 1, cg.size, file);
    fclose(file);
    printf("ELF file created\n");

    CodeGenDtor(&cg); 

    freeNodes(nodes);
    treeDtor(newRoot);

    return 0;
}
