#include <stdio.h>

#include "headers.h"
#include "generator.h"

int main() {
    unsigned char code[] = {
        0x48, 0xc7, 0xc0, 0x01, 0x00, 0x00, 0x00, // mov rax, 1
        0x48, 0xc7, 0xc7, 0x01, 0x00, 0x00, 0x00, // mov rdi, 1
        0x48, 0xc7, 0xc6, 0xa6, 0x00, 0x40, 0x00, // mov rsi, 0x400092
        0x48, 0xc7, 0xc2, 0x0e, 0x00, 0x00, 0x00, // mov rdx, 14
        0x0f, 0x05,                               // syscall
        0x48, 0xc7, 0xc0, 0x3c, 0x00, 0x00, 0x00, // mov rax, 60
        0x48, 0xc7, 0xc7, 0x00, 0x00, 0x00, 0x00, // mov rdi, 0
        0x0f, 0x05,                               // syscall
        0x48, 0x65, 0x6c, 0x6c, 0x6f, 0x2c, 0x20, // "Hello, World!\n"
        0x57, 0x6f, 0x72, 0x6c, 0x64, 0x21, 0x0a
    };
    size_t codeSize = sizeof(code);

    Elf64_Ehdr ehdr;
    CreateElfHeader(&ehdr);

    Elf64_Phdr phdr;
    CreateProgramHeader(&phdr, codeSize);

    FILE* file = fopen("output.elf", "wb");
    if (!file) {
        fprintf(stderr, "Cannot open output file\n");
        return 1;
    }

    fwrite(&ehdr, sizeof(Elf64_Ehdr), 1, file);
    fwrite(&phdr, sizeof(Elf64_Phdr), 1, file);
    fwrite(code, 1, codeSize, file);

    fclose(file);
    printf("ELF file created\n");

    return 0;
}
