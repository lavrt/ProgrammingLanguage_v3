#include <stdio.h>

#include "headers.h"
#include "generator.h"
#include "vector.h"
#include "parser.h"
#include "tree.h"
#include "tokenizer.h"

int main() {
    Vector tokens = tokenizer();     
    tNode* root = runParser(tokens); 

    TCodeGen cg;
    CodeGenCtor(&cg);

    Elf64_Ehdr ehdr;
    CreateElfHeader(&ehdr);

    CodegenProgram(&cg, root, &ehdr);

    Elf64_Phdr phdr;
    CreateProgramHeader(&phdr, cg.size);

    dump(root);                      
    tokenVectorDtor(&tokens);        
    free(tokens.data);               
    treeDtor(root);                  

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

    return 0;
}
