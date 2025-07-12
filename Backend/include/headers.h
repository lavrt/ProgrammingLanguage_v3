#ifndef HEADERS_H
#define HEADERS_H

#include <elf.h>

void CreateElfHeader(Elf64_Ehdr* ehdr);
void CreateProgramHeader(Elf64_Phdr* phdr, uint64_t filesz);

#endif // HEADERS_H