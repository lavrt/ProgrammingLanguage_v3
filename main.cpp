#include <stdio.h>
#include <vector>

#include "headers.h"
#include "generator.h"
#include "vector.h"
#include "parser.h"
#include "tree.h"
#include "tokenizer.h"

int main() {
    std::vector<char*> tokens;
    tokenizer(tokens);

    tNode* root = runParser(tokens);
    saveTree(root); 


  




    tNode* newRoot = ReadTree();

    TCodeGen cg;
    CodeGenCtor(&cg);

    Elf64_Ehdr ehdr;
    CreateElfHeader(&ehdr);

    CodegenProgram(&cg, root, &ehdr);

    Elf64_Phdr phdr;
    CreateProgramHeader(&phdr, cg.size);

    FILE* file = fopen("output.elf", "wb");
    if (!file) {
        fprintf(stderr, "Cannot open output file\n");
        exit(1);
    }
    fwrite(&ehdr, sizeof(Elf64_Ehdr), 1, file);
    fwrite(&phdr, sizeof(Elf64_Phdr), 1, file);
    fwrite(cg.code, 1, cg.size, file);
    fclose(file);
    printf("ELF file created\n");

    CodeGenDtor(&cg); 

    return 0;
}
